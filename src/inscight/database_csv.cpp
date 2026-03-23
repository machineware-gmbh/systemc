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

#include "inscight/database_csv.h"

#include <string.h>

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace inscight {

static std::string escape(const std::string& s) {
    std::stringstream ss;
    for (auto c : s) {
        for (auto esc : ",\\") {
            if (c == esc)
                ss << '\\';
        }
        ss << c;
    }

    return ss.str();
}

void database_csv::gen_meta(const meta_info& info) {
    m_db_meta << info.pid << "," << escape(info.path) << ","
              << escape(info.user) << ","  << escape(info.version) << ","
              << info.timestamp << "," << std::endl;
}

void database_csv::module_created(id_t obj, const char* nm, const char* kind) {
    m_db_modules << obj << "," << escape(nm) << "," << escape(kind) << std::endl;
}

void database_csv::process_created(id_t obj, const char* nm, proc_kind kind) {
    m_db_processes << obj << "," << escape(nm) << "," << proc_kind(kind) << std::endl;
}

void database_csv::port_created(id_t obj, const char* name) {
    m_db_ports << obj << "," << escape(name) << std::endl;
}

void database_csv::event_created(id_t obj, const char* name) {
    m_db_events << obj << "," << escape(name) << std::endl;
}

void database_csv::channel_created(id_t obj, const char* nm, const char* kind) {
    m_db_channels << obj << "," << escape(nm) << "," << escape(kind) << std::endl;
}

void database_csv::port_bound(id_t from, id_t to, binding_kind kind, protocol_kind proto) {
    m_db_bindings << from << "," << to << "," << binding_str(kind) << "," << protocol_str(proto) << std::endl;
}

void database_csv::module_phase_started(id_t obj, module_phase phase, real_time_t t) {
    m_db_elab << t << ",START," << obj << "," << phase_str(phase) << std::endl;
}

void database_csv::module_phase_finished(id_t obj, module_phase phase, real_time_t t) {
    m_db_elab << t << ",FINISH," << obj << "," << phase_str(phase) << std::endl;
}

void database_csv::process_start(id_t obj, real_time_t rt, sysc_time_t st) {
    m_db_scheduling << rt << ",START," << obj << "," << st << std::endl;
}

void database_csv::process_yield(id_t obj, real_time_t rt, sysc_time_t st) {
    m_db_scheduling << rt << ",YIELD," << obj << "," << st << std::endl;
}

void database_csv::event_notify_immediate(id_t obj, real_time_t rt, sysc_time_t st) {
    m_db_notify << rt << ",IMMEDIATE," << obj << "," << st << ",0" << std::endl;
}

void database_csv::event_notify_delta(id_t obj, real_time_t rt, sysc_time_t st) {
    m_db_notify << rt << ",DELTA," << obj << "," << st << ",0" << std::endl;
}

void database_csv::event_notify_timed(id_t obj, real_time_t rt, sysc_time_t st, sysc_time_t delay) {
    m_db_notify << rt << ",TIMED," << obj << "," << st << "," << delay << std::endl;
}

void database_csv::event_cancel(id_t obj, real_time_t rt, sysc_time_t st) {
    m_db_notify << rt << ",CANCEL," << obj << "," << st << ",0" << std::endl;
}

void database_csv::channel_update_start(id_t obj, real_time_t rt, sysc_time_t st) {
    m_db_update << rt << ",START," << obj << "," << st << std::endl;
}

void database_csv::channel_update_complete(id_t obj, real_time_t rt, sysc_time_t st) {
    m_db_update << rt << ",COMPLETE," << obj << "," << st << std::endl;
}

void database_csv::cpu_idle_enter(id_t obj, sysc_time_t st) {
    m_db_cpuidle << st << "," << obj << "," << "IDLE_ENTER" << std::endl;
}

void database_csv::cpu_idle_leave(id_t obj, sysc_time_t st) {
    m_db_cpuidle << st << "," << obj << "," << "IDLE_LEAVE" << std::endl;
}

void database_csv::cpu_call_stack(id_t obj, sysc_time_t st, size_t level, unsigned long long addr, const char* sym) {
    m_db_cpustack << st << "," << obj << "," << level << ",0x" << std::hex << addr << std::dec << ",\"" << sym << "\"" << std::endl;
}

void database_csv::transaction_trace_fw(id_t obj, sysc_time_t st, protocol_kind proto, const char* json) {
    m_db_transactions << st << "," << obj << ",fw," << protocol_str(proto) << ","  << json << std::endl;
}

void database_csv::transaction_trace_bw(id_t obj, sysc_time_t st, protocol_kind proto, const char* json) {
    m_db_transactions << st << "," << obj << ",bw," << protocol_str(proto) << ",\""  << json << "\"" << std::endl;
}

void database_csv::log_message(sysc_time_t st, int loglevel, const char* sender, const char* message) {
    m_db_logmsg << st << "," << loglevel << ",\"" << sender  << "\",\"" << message << "\"" << std::endl;
}

void database_csv::quantum_update(sysc_time_t st, sysc_time_t oldq, sysc_time_t newq) {
    m_db_quantum << st << "," << oldq << "," << newq << std::endl;
}

void database_csv::handle_kthread_event(real_time_t rt, kthread_event event) {
    m_db_kthread << rt << "," << kthread_event_str(event) << std::endl;
}

void database_csv::handle_irq_event(id_t obj, real_time_t rt, sysc_time_t st, size_t irqid, irq_event event) {
    m_db_irq << rt << "," << st << ",irq" << irqid << "," << irq_event_str(event) << std::endl;
}

void database_csv::handle_btransport_fw(id_t port, id_t payload, id_t thread, real_time_t rt, sysc_time_t st) {
    m_db_btransport << port << "," << payload << "," << thread << "," << rt << "," << st << "," << 0 << std::endl;
}

void database_csv::handle_btransport_bw(id_t port, id_t payload, id_t thread, real_time_t rt, sysc_time_t st) {
    m_db_btransport << port << "," << payload << "," << thread << "," << rt << "," << st << "," << 1 << std::endl;
}

static const char* dbname(const std::string& options, const char* nm) {
    static char name[256];
    memset(name, 0, sizeof(name));
    snprintf(name, sizeof(name), "%s.%d.csv", nm, (int)getpid());
    return name;
}

database_csv::database_csv(const std::string& options):
    database(options),
    m_db_meta(dbname(options, "meta")),
    m_db_modules(dbname(options, "modules")),
    m_db_processes(dbname(options, "processes")),
    m_db_ports(dbname(options, "ports")),
    m_db_events(dbname(options, "events")),
    m_db_channels(dbname(options, "channels")),
    m_db_elab(dbname(options, "elab")),
    m_db_scheduling(dbname(options, "scheduling")),
    m_db_notify(dbname(options, "notify")),
    m_db_update(dbname(options, "update")),
    m_db_bindings(dbname(options, "bindings")),
    m_db_cpuidle(dbname(options, "cpuidle")),
    m_db_cpustack(dbname(options, "cpustack")),
    m_db_transactions(dbname(options, "transactions")),
    m_db_logmsg(dbname(options, "logmsg")),
    m_db_quantum(dbname(options, "quantum")),
    m_db_kthread(dbname(options, "kthread")),
    m_db_irq(dbname(options, "irq")),
    m_db_btransport(dbname(options, "btransport")) {
}

database_csv::~database_csv() {
    stop();
}

} // namespace inscight
