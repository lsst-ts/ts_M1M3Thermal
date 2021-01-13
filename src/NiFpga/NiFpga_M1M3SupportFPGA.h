/*
 * Generated with the FPGA Interface C API Generator 19.0
 * for NI-RIO 19.0 or later.
 */
#ifndef __NiFpga_M1M3SupportFPGA_h__
#define __NiFpga_M1M3SupportFPGA_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 190
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_M1M3SupportFPGA_Bitfile;
 */
#define NiFpga_M1M3SupportFPGA_Bitfile "NiFpga_M1M3SupportFPGA.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_M1M3SupportFPGA_Signature = "CFBE95E3F3E2220EA7D6D246D08D87B2";

#if NiFpga_Cpp
extern "C"
{
#endif

typedef enum
{
   NiFpga_M1M3SupportFPGA_TargetToHostFifoU8_U8ResponseFIFO = 0,
} NiFpga_M1M3SupportFPGA_TargetToHostFifoU8;

typedef enum
{
   NiFpga_M1M3SupportFPGA_TargetToHostFifoU16_U16ResponseFIFO = 1,
} NiFpga_M1M3SupportFPGA_TargetToHostFifoU16;

typedef enum
{
   NiFpga_M1M3SupportFPGA_TargetToHostFifoU64_HealthAndStatusDataFIFO = 4,
} NiFpga_M1M3SupportFPGA_TargetToHostFifoU64;

typedef enum
{
   NiFpga_M1M3SupportFPGA_HostToTargetFifoU16_CommandFIFO = 6,
   NiFpga_M1M3SupportFPGA_HostToTargetFifoU16_HealthAndStatusControlFIFO = 5,
   NiFpga_M1M3SupportFPGA_HostToTargetFifoU16_RequestFIFO = 3,
} NiFpga_M1M3SupportFPGA_HostToTargetFifoU16;

typedef enum
{
   NiFpga_M1M3SupportFPGA_HostToTargetFifoU64_TimestampControlFIFO = 2,
} NiFpga_M1M3SupportFPGA_HostToTargetFifoU64;


#if NiFpga_Cpp
}
#endif

#endif
