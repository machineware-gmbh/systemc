/******************************************************************************
 *                                                                            *
 * Copyright (C) 2022-2025 MachineWare GmbH                                   *
 *                                                                            *
 * This file is licensed for non-commercial use only.                         *
 * You may use, modify, and distribute this file for personal or educational  *
 * purposes, but any commercial use, including but not limited to selling,    *
 * licensing, or integrating this code into proprietary software, is strictly *
 * prohibited unless otherwise agreed to in writing by MachineWare GmbH.      *
 *                                                                            *
 * THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   *
 * OR IMPLIED. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE   *
 * FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY ARISING FROM THE USE OF         *
 * THIS SOFTWARE.                                                             *
 *                                                                            *
 ******************************************************************************/

#include <systemc>
#include <chrono>

#include "inscight/context.h"

namespace inscight {

template <>
sysc_time_t to_picos(const sc_core::sc_time& t) {
    static const sc_core::sc_time pico(1.0, sc_core::SC_PS);
    return t / pico;
}

real_time_t real_time_stamp() {
    static auto start = std::chrono::steady_clock::now();
    auto delta = std::chrono::steady_clock::now() - start;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count();
}

sysc_time_t sysc_time_stamp() {
    return to_picos(sc_core::sc_time_stamp());
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

const char* kthread_event_str(kthread_event event) {
    switch (event) {
    case KTHREAD_STARTED:
        return "KTHREAD_STARTED";
    case KTHREAD_SUSPENDED:
        return "KTHREAD_SUSPENDED";
    case KTHREAD_RESUMED:
        return "KTHREAD_RESUMED";
    case KTHREAD_THROTTLED:
        return "KTHREAD_THROTTLED";
    case KTHREAD_UNTHROTTLED:
        return "KTHREAD_UNTHROTTLED";
    case KTHREAD_EXITED:
        return "KTHREAD_EXITED";
    default:
        return "KTHREAD_UNKNOWN";
    }
}

const char* irq_event_str(irq_event event) {
    switch (event) {
    case IRQ_LEVEL_LO:
        return "IRQ_LEVEL_LO";
    case IRQ_LEVEL_HI:
        return "IRQ_LEVEL_HI";
    case IRQ_EDGE_FALL:
        return "IRQ_EDGE_FALL";
    case IRQ_EDGE_RISE:
        return "IRQ_EDGE_RISE";
    case IRQ_MSI:
        return "IRQ_MSI";
    default:
        return "KTHREAD_UNKNOWN";
    }
}

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
