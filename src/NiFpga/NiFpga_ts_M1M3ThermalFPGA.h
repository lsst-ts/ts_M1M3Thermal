/*
 * Generated with the FPGA Interface C API Generator 24.3
 * for NI-RIO 24.3 or later.
 */
#ifndef __NiFpga_ts_M1M3ThermalFPGA_h__
#define __NiFpga_ts_M1M3ThermalFPGA_h__

#ifndef NiFpga_Version
#define NiFpga_Version 243
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
static const char* const NiFpga_ts_M1M3ThermalFPGA_Signature = "E512D0EBAF61A28351D1B42190EDA7BA";

#if NiFpga_Cpp
extern "C" {
#endif

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_IndicatorBool_FlowMeterTimeout = 0x1801A,
    NiFpga_ts_M1M3ThermalFPGA_IndicatorBool_GlycoolTimeout = 0x1800E
} NiFpga_ts_M1M3ThermalFPGA_IndicatorBool;

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_IndicatorI32_FlowMeterRead = 0x18014,
    NiFpga_ts_M1M3ThermalFPGA_IndicatorI32_FlowMeterTransmitted = 0x18010,
    NiFpga_ts_M1M3ThermalFPGA_IndicatorI32_GlycoolRead = 0x18008,
    NiFpga_ts_M1M3ThermalFPGA_IndicatorI32_GlycoolTransmitted = 0x18004
} NiFpga_ts_M1M3ThermalFPGA_IndicatorI32;

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_FlowMeterRead = 8,
    NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_GlycoolRead = 6,
    NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8_U8ResponseFIFO = 0
} NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU8;

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU16_U16ResponseFIFO = 1
} NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoU16;

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoSgl_SGLResponseFIFO = 3
} NiFpga_ts_M1M3ThermalFPGA_TargetToHostFifoSgl;

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_FlowMeterWrite = 7,
    NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8_GlycoolWrite = 5
} NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU8;

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16_CommandFIFO = 9,
    NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16_RequestFIFO = 4
} NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU16;

typedef enum {
    NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU64_TimestampControlFIFO = 2
} NiFpga_ts_M1M3ThermalFPGA_HostToTargetFifoU64;

/* Indicator: ChassisTemperature */
static const NiFpga_FxpTypeInfo NiFpga_ts_M1M3ThermalFPGA_IndicatorFxp_ChassisTemperature_TypeInfo = {1, 32,
                                                                                                      16};

/* Use NiFpga_ReadU32() to access ChassisTemperature */
static const uint32_t NiFpga_ts_M1M3ThermalFPGA_IndicatorFxp_ChassisTemperature_Resource = 0x18000;

#if NiFpga_Cpp
}
#endif

#endif
