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

#ifndef INSCIGHT_DATABASE_CSV_H
#define INSCIGHT_DATABASE_CSV_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "inscight/entry.h"
#include "inscight/database.h"

namespace inscight {

class database_csv : public database
{
private:
    std::ofstream m_db_meta;
    std::ofstream m_db_modules;
    std::ofstream m_db_processes;
    std::ofstream m_db_ports;
    std::ofstream m_db_events;
    std::ofstream m_db_channels;
    std::ofstream m_db_elab;
    std::ofstream m_db_scheduling;
    std::ofstream m_db_notify;
    std::ofstream m_db_update;
    std::ofstream m_db_bindings;
    std::ofstream m_db_cpuidle;
    std::ofstream m_db_cpustack;
    std::ofstream m_db_transactions;
    std::ofstream m_db_logmsg;

protected:
    virtual void gen_meta(const meta_info& info) override;

    virtual void module_created(id_t obj, const char* name, const char* kind) override;
    virtual void process_created(id_t obj, const char* name, proc_kind kind) override;
    virtual void port_created(id_t obj, const char* name) override;
    virtual void event_created(id_t obj, const char* name) override;
    virtual void channel_created(id_t obj, const char* name, const char* kind) override;

    virtual void port_bound(id_t from, id_t to, binding_kind kind, protocol_kind proto) override;

    virtual void module_phase_started(id_t obj, module_phase phase, real_time_t t) override;
    virtual void module_phase_finished(id_t obj, module_phase phase, real_time_t t) override;

    virtual void process_start(id_t obj, real_time_t rt, sysc_time_t st) override;
    virtual void process_yield(id_t obj, real_time_t rt, sysc_time_t st) override;

    virtual void event_notify_immediate(id_t obj, real_time_t rt, sysc_time_t st) override;
    virtual void event_notify_delta(id_t obj, real_time_t rt, sysc_time_t st) override;
    virtual void event_notify_timed(id_t obj, real_time_t rt, sysc_time_t st, sysc_time_t delay) override;
    virtual void event_cancel(id_t obj, real_time_t rt, sysc_time_t st) override;

    virtual void channel_update_start(id_t obj, real_time_t rt, sysc_time_t st) override;
    virtual void channel_update_complete(id_t obj, real_time_t rt, sysc_time_t st) override;

    virtual void cpu_idle_enter(id_t obj, sysc_time_t st) override;
    virtual void cpu_idle_leave(id_t obj, sysc_time_t st) override;

    virtual void cpu_call_stack(id_t obj, sysc_time_t st, size_t level, unsigned long long addr, const char* sym) override;

    virtual void transaction_trace_fw(id_t obj, sysc_time_t st, protocol_kind proto, const char* json) override;
    virtual void transaction_trace_bw(id_t obj, sysc_time_t st, protocol_kind proto, const char* json) override;

    virtual void log_message(sysc_time_t st, int loglevel, const char* sender, const char* message) override;
public:
    database_csv(const std::string& options);
    virtual ~database_csv();
};

} // namespace inscight

#endif
