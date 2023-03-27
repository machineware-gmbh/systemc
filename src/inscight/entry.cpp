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

#include <time.h>
#include <systemc>

#include "inscight/context.h"

namespace inscight {

static real_time_t _real_time_stamp() {
    struct timespec tp = {};
    if (clock_gettime(CLOCK_MONOTONIC, &tp)) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }

    return (real_time_t)tp.tv_sec * 1000000000ull + (real_time_t)tp.tv_nsec;
}

real_time_t real_time_stamp() {
    static real_time_t start = _real_time_stamp();
    return _real_time_stamp() - start;
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

} // namespace inscight
