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

#include "inscight/database_sql.h"

#include <string.h>
#include <sstream>

#ifdef _MSC_VER
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define SQL_ERROR(...) do {       \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n");        \
    fflush(stderr);               \
    abort();                      \
} while (0)

#define SQL_ERROR_ON(ret, ...) do { \
    if (ret != SQLITE_OK) {         \
        SQL_ERROR(__VA_ARGS__);     \
    }                               \
} while (0)

namespace inscight {

database_sql::db::db():
    m_db() {
}

database_sql::db::~db() {
    if (m_db != nullptr)
        sqlite3_close(m_db);
}

void database_sql::db::open() {
    static char name[256];
    memset(name, 0, sizeof(name));
    snprintf(name, sizeof(name), "sim.%d.db", (int)getpid());
    int ret = sqlite3_open(name, &m_db);
    SQL_ERROR_ON(ret, "failed to open database: %s", sqlite3_errmsg(m_db));
}

database_sql::stmt::stmt(db& d, const std::string& text):
    m_db(d),
    m_stmt(nullptr),
    m_text(text) {
}

database_sql::stmt::~stmt() {
    if (m_stmt)
        sqlite3_finalize(m_stmt);
}

void database_sql::stmt::prepare() {
    int ret = sqlite3_prepare_v2(m_db.handle(), text(), -1, &m_stmt, nullptr);
    SQL_ERROR_ON(ret, "failed to prepare '%s': %s", text(), m_db.error());
}

void database_sql::stmt::bind(size_t idx, unsigned long long val) {
    if (m_stmt == nullptr)
        prepare();

    int ret = sqlite3_bind_int64(m_stmt, idx, val);
    SQL_ERROR_ON(ret, "failed to bind arg %zu: %s", idx, m_db.error());
}

void database_sql::stmt::bind(size_t idx, const char* str) {
    if (m_stmt == nullptr)
        prepare();

    int ret = sqlite3_bind_text(m_stmt, idx, str, -1, SQLITE_TRANSIENT);
    SQL_ERROR_ON(ret, "failed to bind arg %zu: %s", idx, m_db.error());
}

void database_sql::stmt::execute() {
    if (m_stmt == nullptr)
        prepare();

    int ret = sqlite3_step(m_stmt);
    if (ret != SQLITE_DONE)
        SQL_ERROR("failed to evaluate '%s': %s", text(), m_db.error());

    ret = sqlite3_clear_bindings(m_stmt);
    SQL_ERROR_ON(ret, "failed to clear command bindings");
    ret = sqlite3_reset(m_stmt);
    SQL_ERROR_ON(ret, "failed to reset command");
}

void database_sql::exec(const std::string& cmd) {
    stmt x(m_db, cmd);
    x.execute();
}

void database_sql::init() {
    m_db.open();

    exec("CREATE TABLE meta(pid BIGINT PRIMARY KEY, path STRING, user STRING, version STRING, time DATETIME);");
    exec("CREATE TABLE modules(id BIGINT PRIMARY KEY, name STRING, kind STRING);");
    exec("CREATE TABLE processes(id BIGINT PRIMARY KEY, name STRING, kind INTEGER);");
    exec("CREATE TABLE ports(id BIGINT PRIMARY KEY, name STRING);");
    exec("CREATE TABLE events(id BIGINT PRIMARY KEY, name STRING);");
    exec("CREATE TABLE channels(id BIGINT PRIMARY KEY, name STRING, kind STRING);");
    exec("CREATE TABLE elab(id INTEGER PRIMARY KEY, rt BIGINT, module BIGINT, phase INTEGER, status INTEGER);");
    exec("CREATE TABLE sched(id INTEGER PRIMARY KEY, rt BIGINT, proc BIGINT, status INTEGER, st BIGINT);");
    exec("CREATE TABLE notify(id INTEGER PRIMARY KEY, rt BIGINT, event BIGINT, kind INTEGER, st BIGINT, delay BIGINT);");
    exec("CREATE TABLE updates(id INTEGER PRIMARY KEY, rt BIGINT, channel BIGINT, status INTEGER, st BIGINT);");
    exec("CREATE TABLE bindings(id INTEGER PRIMARY KEY, from_port BIGINT, to_port BIGINT, kind INTEGER, proto INTEGER);");
    exec("CREATE TABLE cpuidle(id BIGINT PRIMARY KEY, st BIGINT, cpu BIGINT, idle INTEGER);");
    exec("CREATE TABLE cpustack(id BIGINT PRIMARY KEY, st BIGINT, cpu BIGINT, level INTEGER, addr BIGINT, sym TEXT NOT NULL);");
    exec("CREATE TABLE transactions(id BIGINT PRIMARY KEY, st BIGINT, dir INTEGER, port BIGINT, proto INTEGER, json TEXT NOT NULL);");
    exec("CREATE TABLE logmsg(id BIGINT PRIMARY KEY, st BIGINT, loglvl INTEGER, sender TEXT NOT NULL, msg TEXT NOT NULL);");
}

void database_sql::begin(size_t n) {
    m_stmt_tx_begin.execute();
}

void database_sql::end(size_t n) {
    m_stmt_tx_end.execute();
}

void database_sql::gen_meta(const meta_info& info) {
    std::stringstream ss;
    ss << "INSERT INTO meta (pid, path, user, version, time) VALUES ("
       << info.pid << ",'" << info.path << "','" << info.user << "','"
       << info.version << "'," << info.timestamp << ");";
    exec(ss.str());
}

void database_sql::module_created(id_t obj, const char* name, const char* kind) {
    m_stmt_insert_module.bind(1, obj);
    m_stmt_insert_module.bind(2, name);
    m_stmt_insert_module.bind(3, kind);
    m_stmt_insert_module.execute();
}

void database_sql::process_created(id_t obj, const char* name, proc_kind kind) {
    m_stmt_insert_process.bind(1, obj);
    m_stmt_insert_process.bind(2, name);
    m_stmt_insert_process.bind(3, (int)kind);
    m_stmt_insert_process.execute();
}

void database_sql::port_created(id_t obj, const char* name) {
    m_stmt_insert_port.bind(1, obj);
    m_stmt_insert_port.bind(2, name);
    m_stmt_insert_port.execute();
}

void database_sql::event_created(id_t obj, const char* name) {
    m_stmt_insert_event.bind(1, obj);
    m_stmt_insert_event.bind(2, name);
    m_stmt_insert_event.execute();
}

void database_sql::channel_created(id_t obj, const char* name, const char* kind) {
    m_stmt_insert_channel.bind(1, obj);
    m_stmt_insert_channel.bind(2, name);
    m_stmt_insert_channel.bind(3, kind);
    m_stmt_insert_channel.execute();
}

void database_sql::port_bound(id_t from, id_t to, binding_kind kind, protocol_kind proto) {
    m_stmt_insert_binding.bind(1, from);
    m_stmt_insert_binding.bind(2, to);
    m_stmt_insert_binding.bind(3, kind);
    m_stmt_insert_binding.bind(4, proto);
    m_stmt_insert_binding.execute();
}

void database_sql::module_phase_started(id_t obj, module_phase phase, real_time_t rt) {
    m_stmt_insert_elab.bind(1, rt);
    m_stmt_insert_elab.bind(2, obj);
    m_stmt_insert_elab.bind(3, (int)phase);
    m_stmt_insert_elab.bind(4, 0ull);
    m_stmt_insert_elab.execute();
}

void database_sql::module_phase_finished(id_t obj, module_phase phase, real_time_t rt) {
    m_stmt_insert_elab.bind(1, rt);
    m_stmt_insert_elab.bind(2, obj);
    m_stmt_insert_elab.bind(3, (int)phase);
    m_stmt_insert_elab.bind(4, 1ull);
    m_stmt_insert_elab.execute();
}

void database_sql::process_start(id_t obj, real_time_t rt, sysc_time_t st) {
    m_stmt_insert_schedule.bind(1, rt);
    m_stmt_insert_schedule.bind(2, obj);
    m_stmt_insert_schedule.bind(3, 0ull);
    m_stmt_insert_schedule.bind(4, st);
    m_stmt_insert_schedule.execute();
}

void database_sql::process_yield(id_t obj, real_time_t rt, sysc_time_t st) {
    m_stmt_insert_schedule.bind(1, rt);
    m_stmt_insert_schedule.bind(2, obj);
    m_stmt_insert_schedule.bind(3, 1ull);
    m_stmt_insert_schedule.bind(4, st);
    m_stmt_insert_schedule.execute();
}

void  database_sql::event_notify_immediate(id_t obj, real_time_t rt, sysc_time_t st) {
    m_stmt_insert_notify.bind(1, rt);
    m_stmt_insert_notify.bind(2, obj);
    m_stmt_insert_notify.bind(3, 0ull);
    m_stmt_insert_notify.bind(4, st);
    m_stmt_insert_notify.bind(5, 0ull);
    m_stmt_insert_notify.execute();
}

void  database_sql::event_notify_delta(id_t obj, real_time_t rt, sysc_time_t st) {
    m_stmt_insert_notify.bind(1, rt);
    m_stmt_insert_notify.bind(2, obj);
    m_stmt_insert_notify.bind(3, 1ull);
    m_stmt_insert_notify.bind(4, st);
    m_stmt_insert_notify.bind(5, 0ull);
    m_stmt_insert_notify.execute();
}

void  database_sql::event_notify_timed(id_t obj, real_time_t rt, sysc_time_t st, sysc_time_t delay) {
    m_stmt_insert_notify.bind(1, rt);
    m_stmt_insert_notify.bind(2, obj);
    m_stmt_insert_notify.bind(3, 2ull);
    m_stmt_insert_notify.bind(4, st);
    m_stmt_insert_notify.bind(5, delay);
    m_stmt_insert_notify.execute();
}

void  database_sql::event_cancel(id_t obj, real_time_t rt, sysc_time_t st) {
    m_stmt_insert_notify.bind(1, rt);
    m_stmt_insert_notify.bind(2, obj);
    m_stmt_insert_notify.bind(3, 3ull);
    m_stmt_insert_notify.bind(4, st);
    m_stmt_insert_notify.bind(5, 0ull);
    m_stmt_insert_notify.execute();
}

void database_sql::channel_update_start(id_t obj, real_time_t rt, sysc_time_t st) {
    m_stmt_insert_update.bind(1, rt);
    m_stmt_insert_update.bind(2, obj);
    m_stmt_insert_update.bind(3, 0ull);
    m_stmt_insert_update.bind(4, st);
    m_stmt_insert_update.execute();
}

void database_sql::channel_update_complete(id_t obj, real_time_t rt, sysc_time_t st) {
    m_stmt_insert_update.bind(1, rt);
    m_stmt_insert_update.bind(2, obj);
    m_stmt_insert_update.bind(3, 1ull);
    m_stmt_insert_update.bind(4, st);
    m_stmt_insert_update.execute();
}

void database_sql::cpu_idle_enter(id_t obj, sysc_time_t st) {
    m_stmt_insert_cpuidle.bind(1, st);
    m_stmt_insert_cpuidle.bind(2, obj);
    m_stmt_insert_cpuidle.bind(3, 1ull);
    m_stmt_insert_cpuidle.execute();
}

void database_sql::cpu_idle_leave(id_t obj, sysc_time_t st) {
    m_stmt_insert_cpuidle.bind(1, st);
    m_stmt_insert_cpuidle.bind(2, obj);
    m_stmt_insert_cpuidle.bind(3, 0ull);
    m_stmt_insert_cpuidle.execute();
}

void database_sql::cpu_call_stack(id_t obj, sysc_time_t st, size_t level, unsigned long long addr, const char* sym) {
    m_stmt_insert_cpustack.bind(1, st);
    m_stmt_insert_cpustack.bind(2, obj);
    m_stmt_insert_cpustack.bind(3, level);
    m_stmt_insert_cpustack.bind(4, addr);
    m_stmt_insert_cpustack.bind(5, sym);
    m_stmt_insert_cpustack.execute();
}

void database_sql::transaction_trace_fw(id_t obj, sysc_time_t st, protocol_kind proto, const char* json) {
    m_stmt_insert_transaction.bind(1, st);
    m_stmt_insert_transaction.bind(2, obj);
    m_stmt_insert_transaction.bind(3, 0ull);
    m_stmt_insert_transaction.bind(4, proto);
    m_stmt_insert_transaction.bind(5, json);
    m_stmt_insert_transaction.execute();
}

void database_sql::transaction_trace_bw(id_t obj, sysc_time_t st, protocol_kind proto, const char* json) {
    m_stmt_insert_transaction.bind(1, st);
    m_stmt_insert_transaction.bind(2, obj);
    m_stmt_insert_transaction.bind(3, 1ull);
    m_stmt_insert_transaction.bind(4, proto);
    m_stmt_insert_transaction.bind(5, json);
    m_stmt_insert_transaction.execute();
}

void database_sql::log_message(sysc_time_t st, int loglevel, const char* sender, const char* message) {
    m_stmt_insert_logmsg.bind(1, st);
    m_stmt_insert_logmsg.bind(2, loglevel);
    m_stmt_insert_logmsg.bind(3, sender);
    m_stmt_insert_logmsg.bind(4, message);
    m_stmt_insert_logmsg.execute();
}

database_sql::database_sql(const std::string& options):
    database(options),
    m_db(),
    m_stmt_tx_begin(m_db, "BEGIN IMMEDIATE TRANSACTION;"),
    m_stmt_tx_end(m_db, "END TRANSACTION;"),
    m_stmt_insert_module(m_db, "INSERT INTO modules (id, name, kind) VALUES (?1, ?2, ?3)"),
    m_stmt_insert_process(m_db, "INSERT INTO processes (id, name, kind) VALUES (?1, ?2, ?3)"),
    m_stmt_insert_port(m_db, "INSERT INTO ports (id, name) VALUES (?1, ?2)"),
    m_stmt_insert_event(m_db, "INSERT INTO events (id, name) VALUES (?1, ?2)"),
    m_stmt_insert_channel(m_db, "INSERT INTO channels (id, name, kind) VALUES (?1, ?2, ?3)"),
    m_stmt_insert_elab(m_db, "INSERT INTO elab (rt, module, phase, status) VALUES (?1, ?2, ?3, ?4)"),
    m_stmt_insert_schedule(m_db, "INSERT INTO sched (rt, proc, status, st) VALUES (?1, ?2, ?3, ?4)"),
    m_stmt_insert_notify(m_db, "INSERT INTO notify (rt, event, kind, st, delay) VALUES (?1, ?2, ?3, ?4, ?5)"),
    m_stmt_insert_update(m_db, "INSERT INTO updates (rt, channel, status, st) VALUES (?1, ?2, ?3, ?4)"),
    m_stmt_insert_binding(m_db, "INSERT INTO bindings (from_port, to_port, kind, proto) VALUES (?1, ?2, ?3, ?4)"),
    m_stmt_insert_cpuidle(m_db, "INSERT INTO cpuidle (st, cpu, idle) VALUES (?1, ?2, ?3)"),
    m_stmt_insert_cpustack(m_db, "INSERT INTO cpustack (st, cpu, level, addr, sym) VALUES (?1, ?2, ?3, ?4, ?5)"),
    m_stmt_insert_transaction(m_db, "INSERT INTO transactions (st, port, dir, proto, json) VALUES (?1, ?2, ?3, ?4, ?5)"),
    m_stmt_insert_logmsg(m_db, "INSERT INTO logmsg (st, loglvl, sender, msg) VALUES (?1, ?2, ?3, ?4)") {
}

database_sql::~database_sql() {
    stop();
}

} // namespace inscight
