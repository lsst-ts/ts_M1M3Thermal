/*
 * Flow Meter telemetry handling class.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software
 * Systems. This product includes software developed by the Vera C.Rubin
 * Observatory Project (https://www.lsst.org). See the COPYRIGHT file at the
 * top-level directory of this distribution for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <arpa/inet.h>
#include <cmath>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

#include <modbus.h>
#include <spdlog/spdlog.h>

#include <IFPGA.h>
#include <TSPublisher.h>
#include <Telemetry/FlowMeterThread.h>

using namespace LSST::M1M3::TS::Telemetry;
using namespace std::chrono_literals;

#define NB_CONNECTION 5

FlowMeterThread::FlowMeterThread(std::shared_ptr<Transports::Transport> transport,
                                 const std::string& server_address, int server_port)
        : _server_address(server_address), _server_port(server_port) {
    _transport = transport;
}

void FlowMeterThread::run(std::unique_lock<std::mutex>& lock) {
    SPDLOG_DEBUG("Running Flow Meter Thread.");

    ctx = modbus_new_tcp(_server_address.empty() ? nullptr : _server_address.c_str(), _server_port);

    uint8_t req[MODBUS_TCP_MAX_ADU_LENGTH];

    server_socket = modbus_tcp_listen(ctx, NB_CONNECTION);
    if (server_socket == -1) {
        modbus_free(ctx);
        throw std::runtime_error(fmt::format("Unable to listen TCP connection: {}.", modbus_strerror(errno)));
    }

    // Dynamic list of file descriptors to monitor
    std::vector<struct pollfd> pollfds;
    pollfds.push_back({server_socket, POLLIN, 0});

    SPDLOG_INFO("Started ModBus server on port {}.", _server_port);

    while (keepRunning) {
        auto end = std::chrono::steady_clock::now() + 100ms;

        // poll() timeout is in milliseconds
        int num_sockets = poll(pollfds.data(), static_cast<nfds_t>(pollfds.size()), 1);

        if (num_sockets == -1) {
            if (errno == EINTR) {
                continue;  // Interrupted by a signal, safely retry
            }
            _close_connection();
            throw std::runtime_error(fmt::format("Server poll() failure: {}.", modbus_strerror(errno)));
        } else if (num_sockets > 0) {
            /* Run through existing connections looking for events */
            size_t num_fds = pollfds.size();
            for (size_t i = 0; i < num_fds; ++i) {
                if (!(pollfds[i].revents & (POLLIN | POLLHUP | POLLERR))) {
                    continue;
                }

                if (pollfds[i].fd == server_socket) {
                    /* A client is asking for a new connection */
                    struct sockaddr_in clientaddr;
                    socklen_t addrlen = sizeof(clientaddr);
                    memset(&clientaddr, 0, addrlen);

                    int newfd = accept(server_socket, (struct sockaddr*)&clientaddr, &addrlen);
                    if (newfd == -1) {
                        SPDLOG_ERROR(fmt::format("Server accept error: {}.", modbus_strerror(errno)));
                    } else {
                        // Dynamically append new socket
                        pollfds.push_back({newfd, POLLIN, 0});
                        SPDLOG_INFO("New Flow Meter connection from {}:{} on socket {}.",
                                    inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
                    }
                } else {
                    int master_socket = pollfds[i].fd;
                    modbus_set_socket(ctx, master_socket);
                    int rc = modbus_receive(ctx, req);

                    // TCP/IP request comes with 7 byte header
                    if (rc > 8) {
                        auto device_id = req[6];
                        auto function_code = req[7];
                        SPDLOG_DEBUG("Request function: unit ID {} function {} - length {}.", device_id,
                                     function_code, rc);

                        if (function_code != 3) {
                            modbus_reply_exception(ctx, req, MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
                        } else {
                            _process_read_registers(req, rc);
                        }
                    } else if (rc == -1) {
                        /* Connection closed or error occurred */
                        SPDLOG_INFO("Connection closed on socket {}.", master_socket);
                        close(master_socket);

                        // Fast O(1) removal from vector: swap with last element and pop
                        pollfds[i] = pollfds.back();
                        pollfds.pop_back();
                        --i;        // Re-check current index since last element moved here
                        --num_fds;  // Decrease current loop boundary
                    }
                }
            }
        }

        runCondition.wait_until(lock, end);
    }
    SPDLOG_DEBUG("Flow Meter Thread Stopped.");

    modbus_close(ctx);
    modbus_free(ctx);
}

void FlowMeterThread::_process_read_registers(uint8_t* req, int rc) {
    uint16_t address = ntohs(*reinterpret_cast<uint16_t*>(req + 8));
    uint16_t count = ntohs(*reinterpret_cast<uint16_t*>(req + 10));

    SPDLOG_TRACE("Processing read registry request - address {} count {}.", address, count);

    modbus_mapping_t* mb_mapping = modbus_mapping_new_start_address(0, 0, 0, 0, address, count, 0, 0);
    if (mb_mapping == NULL) {
        modbus_free(ctx);
        throw std::runtime_error(fmt::format("Failed to allocate mapping: {}.", modbus_strerror(errno)));
    }

    try {
        clear();
        readHoldingRegisters(address, count, 255);

        _transport->commands(*this, 2s, this);

        for (uint16_t i = 0; i < count; i++) {
            uint16_t value = getRegister(address + i);
            memcpy(mb_mapping->tab_registers + i, &value, 2);
        }
    } catch (std::runtime_error& er) {
        SPDLOG_WARN("Error processing read register request {} {}: {}.", address, count, er.what());
        modbus_reply_exception(ctx, req, MODBUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE);
    }

    modbus_reply(ctx, req, rc, mb_mapping);

    modbus_mapping_free(mb_mapping);
}

void FlowMeterThread::_close_connection() {
    if (server_socket != -1) {
        close(server_socket);
    }

    modbus_free(ctx);
}
