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
static const char* const NiFpga_ts_M1M3ThermalFPGA_Signature = "BFEB5750E6D1087104B0284AFD946268";

#if NiFpga_Cpp
extern "C"
{
#endif

typedef enum
{
   NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_MPUResponseFIFO = 4,
   NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_U8ResponseFIFO = 0,
} NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8;

typedef enum
{
   NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU16_U16ResponseFIFO = 1,
} NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU16;

typedef enum
{
   NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_MPUCommandsFIFO = 5,
} NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8;

typedef enum
{
   NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16_CommandFIFO = 6,
   NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16_RequestFIFO = 3,
} NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16;

typedef enum
{
   NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU64_TimestampControlFIFO = 2,
} NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU64;


#if NiFpga_Cpp
}
#endif

#endif
