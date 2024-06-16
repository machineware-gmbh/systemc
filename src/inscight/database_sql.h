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

#ifndef INSCIGHT_DATABASE_SQL_H
#define INSCIGHT_DATABASE_SQL_H

#include <string>
#include <inscight/sqlite/sqlite3.h>

#include "inscight/entry.h"
#include "inscight/database.h"

namespace inscight {

class database_sql : public database
{
private:
    class db
    {
    private:
        sqlite3* m_db;

    public:
        sqlite3* handle() const { return m_db; }
        const char* error() const { return sqlite3_errmsg(m_db); }
        db();
        ~db();
        void open();
    };

    class stmt
    {
    private:
        db& m_db;
        sqlite3_stmt* m_stmt;
        std::string m_text;

    public:
        const char* text() const { return m_text.c_str(); }

        stmt(db& db, const std::string& text);
        ~stmt();

        void prepare();
        void bind(size_t idx, unsigned long long val);
        void bind(size_t idx, const char* str);
        void execute();
    };

    db m_db;

    stmt m_stmt_tx_begin;
    stmt m_stmt_tx_end;

    stmt m_stmt_insert_module;
    stmt m_stmt_insert_process;
    stmt m_stmt_insert_port;
    stmt m_stmt_insert_event;
    stmt m_stmt_insert_channel;
    stmt m_stmt_insert_elab;
    stmt m_stmt_insert_schedule;
    stmt m_stmt_insert_notify;
    stmt m_stmt_insert_update;
    stmt m_stmt_insert_binding;
    stmt m_stmt_insert_cpuidle;
    stmt m_stmt_insert_cpustack;
    stmt m_stmt_insert_transaction;
    stmt m_stmt_insert_logmsg;

    void exec(const std::string& cmd);

protected:
    virtual void init() override;
    virtual void begin(size_t n) override;
    virtual void end(size_t n) override;

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
    database_sql(const std::string& options);
    virtual ~database_sql();
};

} // namespace inscight

#endif
