
/******************************************************************************
 *                                                                            *
 * Copyright (C) 2022-2026 MachineWare GmbH                                   *
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

#include "inscight/tracing_ifs.h"

namespace inscight {

std::unordered_map<id_t, fw_transport_if_b*> ifs;

fw_transport_if_b::fw_transport_if_b(id_t owner): owner(owner) {
    if (ifs.count(owner))
        delete ifs[owner];
    ifs[owner] = this;
}

fw_transport_if_b::~fw_transport_if_b() {
    ifs.erase(owner);
}

void fw_transport_if_b::remove(id_t owner) {
    if (ifs.count(owner))
        delete ifs[owner];
}

} // namespace inscight
