/******************************************************************************
 *                                                                            *
 * Copyright 2023 MachineWare GmbH                                            *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is unpublished proprietary work owned by MachineWare GmbH. It may be  *
 * used, modified and distributed in accordance to the license specified by   *
 * the license file in the root directory of this project.                    *
 *                                                                            *
 ******************************************************************************/

#include <systemc>
#include <chrono>

#include "inscight/context.h"

namespace inscight {

real_time_t real_time_stamp() {
    static auto start = std::chrono::steady_clock::now();
    auto delta = std::chrono::steady_clock::now() - start;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count();
}

sysc_time_t sysc_time_stamp() {
    static const sc_core::sc_time pico(1.0, sc_core::SC_PS);
    return sc_core::sc_time_stamp() / pico;
}

const char* proc_str(proc_kind kind) {
    switch (kind) {
    case KIND_METHOD:
        return "METHOD";
    case KIND_THREAD:
        return "THREAD";
    case KIND_CTHREAD:
        return "CTHREAD";
    default:
        return "UNKNOWN";
    }
}

const char* phase_str(module_phase phase) {
    switch (phase) {
    case PHASE_CONSTRUCTION:
        return "CONSTRUCTION";
    case PHASE_BEFORE_END_OF_ELABORATION:
        return "BEFORE_END_OF_ELABORATION";
    case PHASE_END_OF_ELABORATION:
        return "END_OF_ELABORATION";
    case PHASE_START_OF_SIMULATION:
        return "START_OF_SIMULATION";
    default:
        return "UNKNOWN";
    }
}

const char* binding_str(binding_kind kind) {
    switch (kind) {
    case BIND_NORMAL:
        return "NORMAL";
    case BIND_HIERARCHY:
        return "HIERARCHY";
    default:
        return "UNKNOWN";
    }
}

const char* protocol_str(protocol_kind kind) {
    switch (kind) {
    case PROTO_SIGNAL:
        return "SIGNAL";
    case PROTO_TLM:
        return "TLM";
    case PROTO_GPIO:
        return "GPIO";
    case PROTO_CLK:
        return "CLK";
    case PROTO_PCI:
        return "PCI";
    case PROTO_I2C:
        return "I2C";
    case PROTO_SPI:
        return "SPI";
    case PROTO_SD:
        return "SD";
    case PROTO_SERIAL:
        return "SERIAL";
    case PROTO_VIRTIO:
        return "VIRTIO";
    case PROTO_ETHERNET:
        return "ETHERNET";
    case PROTO_CAN:
        return "CAN";
    case PROTO_USB:
        return "USB";
    case PROTO_UNKNOWN:
    default:
        return "UNKNOWN";
    }
};

protocol_kind protocol_from_str(const char* s) {
    if (strstr(s, "sc_signal"))
        return PROTO_SIGNAL;
    if (strstr(s, "tlm"))
        return PROTO_TLM;
    if (strstr(s, "gpio_payload"))
        return PROTO_GPIO;
    if (strstr(s, "clk_payload"))
        return PROTO_CLK;
    if (strstr(s, "pci_payload"))
        return PROTO_PCI;
    if (strstr(s, "i2c_payload"))
        return PROTO_I2C;
    if (strstr(s, "spi_payload"))
        return PROTO_SPI;
    if (strstr(s, "sd_protocol_types"))
        return PROTO_SD;
    if (strstr(s, "serial_payload"))
        return PROTO_SERIAL;
    if (strstr(s, "vq_message"))
        return PROTO_VIRTIO;
    if (strstr(s, "eth_frame"))
        return PROTO_ETHERNET;
    if (strstr(s, "can_frame"))
        return PROTO_CAN;
    if (strstr(s, "usb_packet"))
        return PROTO_USB;
    return PROTO_UNKNOWN;
}

} // namespace inscight
