/*
 * Generated with the FPGA Interface C API Generator 19.0
 * for NI-RIO 19.0 or later.
 */
#ifndef __NiFpga_ts_M1M3ThermalFPGA_h__
#define __NiFpga_ts_M1M3ThermalFPGA_h__

#ifndef NiFpga_Version
#define NiFpga_Version 190
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_ts_M1M3ThermalFPGA_Bitfile;
 */
#define NiFpga_ts_M1M3ThermalFPGA_Bitfile "NiFpga_ts_M1M3ThermalFPGA.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_ts_M1M3ThermalFPGA_Signature = "38E78EA57D7C8BE90C1716AC2C8560A6";

#if NiFpga_Cpp
extern "C" {
#endif

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_MPUResponseFIFO = 5,
    NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_U8ResponseFIFO = 0,
} NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8;

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU16_U16ResponseFIFO = 1,
} NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU16;

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoSgl_SGLResponseFIFO = 3,
} NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoSgl;

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_MPUCommandsFIFO = 6,
} NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8;

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16_CommandFIFO = 7,
    NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16_RequestFIFO = 4,
} NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16;

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU64_TimestampControlFIFO = 2,
} NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU64;

#if !NiFpga_VxWorks

/* Indicator: FlowMeterError */
/* Use NiFpga_ReadArrayU8() to access FlowMeterError */
const uint32_t NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_FlowMeterError_Resource = 0x18004;
const uint32_t NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_FlowMeterError_PackedSizeInBytes = 5;

typedef struct NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_FlowMeterError_Type {
    NiFpga_Bool status;
    int32_t code;
} NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_FlowMeterError_Type;

void NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_FlowMeterError_UnpackCluster(
        const uint8_t* const packedData,
        NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_FlowMeterError_Type* const destination);

void NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_FlowMeterError_PackCluster(
        uint8_t* const packedData,
        const NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_FlowMeterError_Type* const source);

/* Indicator: VFDError */
/* Use NiFpga_ReadArrayU8() to access VFDError */
const uint32_t NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_VFDError_Resource = 0x18000;
const uint32_t NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_VFDError_PackedSizeInBytes = 5;

typedef struct NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_VFDError_Type {
    NiFpga_Bool status;
    int32_t code;
} NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_VFDError_Type;

void NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_VFDError_UnpackCluster(
        const uint8_t* const packedData,
        NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_VFDError_Type* const destination);

void NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_VFDError_PackCluster(
        uint8_t* const packedData,
        const NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_VFDError_Type* const source);

#endif /* !NiFpga_VxWorks */

#if NiFpga_Cpp
}
#endif

#endif
