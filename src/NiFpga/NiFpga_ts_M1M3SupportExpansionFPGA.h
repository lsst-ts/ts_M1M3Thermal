/*
 * Generated with the FPGA Interface C API Generator 19.0
 * for NI-RIO 19.0 or later.
 */
#ifndef __NiFpga_ts_M1M3SupportExpansionFPGA_h__
#define __NiFpga_ts_M1M3SupportExpansionFPGA_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 190
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_ts_M1M3SupportExpansionFPGA_Bitfile;
 */
#define NiFpga_ts_M1M3SupportExpansionFPGA_Bitfile "NiFpga_ts_M1M3SupportExpansionFPGA.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_ts_M1M3SupportExpansionFPGA_Signature = "83550EEFE36B089CB5745FB593A813FA";

#if NiFpga_Cpp
extern "C"
{
#endif

typedef enum
{
   NiFpga_ts_M1M3SupportExpansionFPGA_IndicatorU32_Slot2 = 0x18008,
} NiFpga_ts_M1M3SupportExpansionFPGA_IndicatorU32;

typedef enum
{
   NiFpga_ts_M1M3SupportExpansionFPGA_ControlBool_Sample = 0x18002,
} NiFpga_ts_M1M3SupportExpansionFPGA_ControlBool;

typedef enum
{
   NiFpga_ts_M1M3SupportExpansionFPGA_IndicatorArraySgl_Slot1 = 0x18004,
} NiFpga_ts_M1M3SupportExpansionFPGA_IndicatorArraySgl;

typedef enum
{
   NiFpga_ts_M1M3SupportExpansionFPGA_IndicatorArraySglSize_Slot1 = 6,
} NiFpga_ts_M1M3SupportExpansionFPGA_IndicatorArraySglSize;


#if NiFpga_Cpp
}
#endif

#endif
