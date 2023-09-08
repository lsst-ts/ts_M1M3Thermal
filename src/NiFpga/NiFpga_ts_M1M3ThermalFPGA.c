#include "NiFpga_ts_M1M3ThermalFPGA.h"

#if !NiFpga_VxWorks

void NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_FlowMeterError_UnpackCluster(
        const uint8_t* const packedData,
        NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_FlowMeterError_Type* const destination) {
    (*destination).status = 0;
    (*destination).status |= ((packedData[0] >> 7) & 0x1);
    (*destination).code = 0;
    (*destination).code |= (packedData[0] & 0x7FULL) << 25;
    (*destination).code |= (packedData[1] & 0xFF) << 17;
    (*destination).code |= (packedData[2] & 0xFF) << 9;
    (*destination).code |= (packedData[3] & 0xFF) << 1;
    (*destination).code |= ((packedData[4] >> 7) & 0x1);
}

void NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_FlowMeterError_PackCluster(
        uint8_t* const packedData,
        const NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_FlowMeterError_Type* const source) {
    packedData[0] = (uint8_t)(((*source).status & 0x1) << 7);
    packedData[0] |= (uint8_t)(((*source).code >> 25) & 0x7F);
    packedData[1] = (uint8_t)(((*source).code >> 17) & 0xFF);
    packedData[2] = (uint8_t)(((*source).code >> 9) & 0xFF);
    packedData[3] = (uint8_t)(((*source).code >> 1) & 0xFF);
    packedData[4] = (uint8_t)(((*source).code & 0x1) << 7);
}

void NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_VFDError_UnpackCluster(
        const uint8_t* const packedData,
        NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_VFDError_Type* const destination) {
    (*destination).status = 0;
    (*destination).status |= ((packedData[0] >> 7) & 0x1);
    (*destination).code = 0;
    (*destination).code |= (packedData[0] & 0x7FULL) << 25;
    (*destination).code |= (packedData[1] & 0xFF) << 17;
    (*destination).code |= (packedData[2] & 0xFF) << 9;
    (*destination).code |= (packedData[3] & 0xFF) << 1;
    (*destination).code |= ((packedData[4] >> 7) & 0x1);
}

void NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_VFDError_PackCluster(
        uint8_t* const packedData,
        const NiFpga_ts_M1M3ThermalFPGA_IndicatorCluster_VFDError_Type* const source) {
    packedData[0] = (uint8_t)(((*source).status & 0x1) << 7);
    packedData[0] |= (uint8_t)(((*source).code >> 25) & 0x7F);
    packedData[1] = (uint8_t)(((*source).code >> 17) & 0xFF);
    packedData[2] = (uint8_t)(((*source).code >> 9) & 0xFF);
    packedData[3] = (uint8_t)(((*source).code >> 1) & 0xFF);
    packedData[4] = (uint8_t)(((*source).code & 0x1) << 7);
}

#endif /* !NiFpga_VxWorks */
