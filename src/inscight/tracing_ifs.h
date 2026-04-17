
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

#ifndef INSCIGHT_TRACING_IFS_H
#define INSCIGHT_TRACING_IFS_H

#include "tlm_core/tlm_2/tlm_2_interfaces/tlm_fw_bw_ifs.h"

namespace inscight {

template <typename TYPES = tlm::tlm_base_protocol_types>
class fw_transport_if : public tlm::tlm_fw_transport_if<TYPES>
{
public:
    virtual ~fw_transport_if() {
        all_ifs().erase(m_owner);
    }

    void b_transport(typename TYPES::tlm_payload_type& trans,
                     sc_core::sc_time& t) override {
        INSCIGHT_BTRANSPORT_FW(m_owner, trans);
        m_inner.b_transport(trans, t);
        INSCIGHT_BTRANSPORT_BW(m_owner, trans);
    }

    tlm::tlm_sync_enum nb_transport_fw(typename TYPES::tlm_payload_type& trans,
                                       typename TYPES::tlm_phase_type& phase,
                                       sc_core::sc_time& t) override {
        return m_inner.nb_transport_fw(trans, phase, t);
    }

    bool get_direct_mem_ptr(typename TYPES::tlm_payload_type& trans,
                            tlm::tlm_dmi& dmi_data) {
        return m_inner.get_direct_mem_ptr(trans, dmi_data);
    }

    unsigned int transport_dbg(typename TYPES::tlm_payload_type& trans) {
        return m_inner.transport_dbg(trans);
    }

    static fw_transport_if<TYPES>* create(tlm::tlm_fw_transport_if<TYPES>& inner, id_t owner) {
        auto*& wrapper = all_ifs()[owner];
        if (wrapper == nullptr)
            wrapper = new fw_transport_if<TYPES>(inner, owner);
        return wrapper;
    }

    static void remove(id_t owner) {
        auto& all = all_ifs();
        auto it = all.find(owner);
        if (it != all.end())
            delete it->second;
    }

private:
    tlm::tlm_fw_transport_if<TYPES>& m_inner;
    id_t m_owner;

    fw_transport_if(tlm::tlm_fw_transport_if<TYPES>& inner, id_t owner):
        m_inner(inner), m_owner(owner) {}

    static std::unordered_map<id_t, fw_transport_if<TYPES>*>& all_ifs() {
        static std::unordered_map<id_t, fw_transport_if<TYPES>*> all;
        return all;
    }
};

} // namespace inscight

#endif
