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

#ifndef INSCIGHT_ENTRY_H
#define INSCIGHT_ENTRY_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

namespace inscight {

typedef size_t id_t;

typedef unsigned long long real_time_t;
typedef unsigned long long sysc_time_t;

real_time_t real_time_stamp();
sysc_time_t sysc_time_stamp();

enum proc_kind {
    KIND_METHOD = 0,
    KIND_THREAD,
    KIND_CTHREAD,
};

const char* proc_str(proc_kind kind);

enum module_phase {
    PHASE_CONSTRUCTION = 0,
    PHASE_BEFORE_END_OF_ELABORATION,
    PHASE_END_OF_ELABORATION,
    PHASE_START_OF_SIMULATION,
};

const char* phase_str(module_phase phase);

enum binding_kind {
    BIND_NORMAL = 0,
    BIND_HIERARCHY,
};

const char* binding_str(binding_kind kind);

enum protocol_kind {
    PROTO_UNKNOWN,
    PROTO_SIGNAL,
    PROTO_TLM,
    PROTO_GPIO,
    PROTO_CLK,
    PROTO_PCI,
    PROTO_I2C,
    PROTO_SPI,
    PROTO_SD,
    PROTO_SERIAL,
    PROTO_VIRTIO,
    PROTO_ETHERNET,
    PROTO_CAN,
    PROTO_USB,
};

const char* protocol_str(protocol_kind kind);
protocol_kind protocol_from_str(const char* s);

enum entry_kind {
    MODULE_CREATED = 0,
    PROCESS_CREATED,
    PORT_CREATED,
    EVENT_CREATED,
    CHANNEL_CREATED,

    PORT_BOUND,

    MODULE_PHASE_STARTED,
    MODULE_PHASE_FINISHED,

    PROCESS_START,
    PROCESS_YIELD,

    EVENT_NOTIFY_IMMEDIATE,
    EVENT_NOTIFY_DELTA,
    EVENT_NOTIFY_TIMED,
    EVENT_CANCEL,

    CHANNEL_UPDATE_START,
    CHANNEL_UPDATE_COMPLETE,

    CPU_IDLE_ENTER,
    CPU_IDLE_LEAVE,

    CPU_CALL_STACK,

    TRANSACTION_TRACE_FW,
    TRANSACTION_TRACE_BW,

    LOG_MESSAGE,
};

struct entry {
    entry_kind kind;
    id_t id;
    uintptr_t arg0;
    uintptr_t arg1;
    uintptr_t arg2;
    uintptr_t arg3;

    template <typename A = uintptr_t, typename B = uintptr_t,
              typename C = uintptr_t, typename D = uintptr_t>
    entry(entry_kind k, id_t obj, A a = A(), B b = B(), C c = C(), D d = D()):
        kind(k),
        id(obj),
        arg0((uintptr_t)a),
        arg1((uintptr_t)b),
        arg2((uintptr_t)c),
        arg3((uintptr_t)d) {}
};

} // namespace inscight

#endif
