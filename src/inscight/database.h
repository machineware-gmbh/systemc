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

#ifndef INSCIGHT_DATABASE_H
#define INSCIGHT_DATABASE_H

#include <string>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "inscight/entry.h"

namespace inscight {

class database
{
protected:
    std::atomic<bool> m_enabled;
    std::atomic<bool> m_running;

    std::mutex m_mtx;
    std::condition_variable_any m_cv;
    std::vector<entry> m_entries;
    std::thread m_worker;

    void gen_meta();

    void work();

    void process(const entry& e);

    virtual void init() {}
    virtual void begin(size_t n) {}
    virtual void end(size_t n) {}

    struct meta_info {
        std::string path;
        std::string user;
        std::string version;
        std::time_t timestamp;
        int pid;
    };

    virtual void gen_meta(const meta_info& info) = 0;

    virtual void module_created(id_t obj, const char* name, const char* kind) = 0;
    virtual void process_created(id_t obj, const char* name, proc_kind kind) = 0;
    virtual void port_created(id_t obj, const char* name) = 0;
    virtual void event_created(id_t obj, const char* name) = 0;
    virtual void channel_created(id_t obj, const char* name, const char* kind) = 0;

    virtual void port_bound(id_t from, id_t to, binding_kind kind, protocol_kind proto) = 0;

    virtual void module_phase_started(id_t obj, module_phase phase, real_time_t t) = 0;
    virtual void module_phase_finished(id_t obj, module_phase phase, real_time_t t) = 0;

    virtual void process_start(id_t obj, real_time_t rt, sysc_time_t st) = 0;
    virtual void process_yield(id_t obj, real_time_t rt, sysc_time_t st) = 0;

    virtual void event_notify_immediate(id_t obj, real_time_t rt, sysc_time_t st) = 0;
    virtual void event_notify_delta(id_t obj, real_time_t rt, sysc_time_t st) = 0;
    virtual void event_notify_timed(id_t obj, real_time_t rt, sysc_time_t st, sysc_time_t delay) = 0;
    virtual void event_cancel(id_t obj, real_time_t rt, sysc_time_t st) = 0;

    virtual void channel_update_start(id_t obj, real_time_t rt, sysc_time_t st) = 0;
    virtual void channel_update_complete(id_t obj, real_time_t rt, sysc_time_t st) = 0;

    virtual void cpu_idle_enter(id_t obj, sysc_time_t st) = 0;
    virtual void cpu_idle_leave(id_t obj, sysc_time_t st) = 0;

    virtual void cpu_call_stack(id_t obj, sysc_time_t st, size_t level, unsigned long long addr, const char* sym) = 0;

    virtual void transaction_trace_fw(id_t obj, sysc_time_t st, protocol_kind proto, const char* json) = 0;
    virtual void transaction_trace_bw(id_t obj, sysc_time_t st, protocol_kind proto, const char* json) = 0;

    virtual void log_message(sysc_time_t st, int loglevel, const char* sender, const char* message) = 0;

public:
    template <typename... ARGS>
    void insert(ARGS&&... args) {
        m_mtx.lock();
        m_entries.emplace_back(std::forward<ARGS>(args)...);
        m_mtx.unlock();
        m_cv.notify_one();
    }

    void start();
    void stop();

    database(const std::string& options);
    virtual ~database();
};

} // namespace inscight

#endif
