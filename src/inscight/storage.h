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

#ifndef INSCIGHT_STORAGE_H
#define INSCIGHT_STORAGE_H

#include <unordered_map>

#include "tlm_core/tlm_2/tlm_2_interfaces/tlm_fw_bw_ifs.h"

namespace inscight {

typedef std::unique_ptr<tlm::tlm_tracing_fw_transport_if_b> tlm_tracing_fw_transport_if_ptr;

extern std::unordered_map<void*, tlm_tracing_fw_transport_if_ptr> tlm_tracing_fw_transport_if;

} // namespace inscight

#endif
