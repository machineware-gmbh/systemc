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

#include "inscight/database.h"
#include "sysc/kernel/sc_ver.h"

#if defined(__linux__)
#include <unistd.h>
static std::string progpath() {
    char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);

    if (len == -1)
        return "unknown";

    path[len] = '\0';
    return path;
}

static std::string username() {
    char uname[256] = {};
    if (getlogin_r(uname, sizeof(uname) - 1) == 0)
        return uname;

    const char* envuser = getenv("USER");
    if (envuser)
        return envuser;

    return "unknown";
}
#else
static std::string progpath() {
    return "unknown";
}

static std::string username() {
    return "unknown";
}

static int getpid() {
    return -1;
}
#endif

namespace inscight {

void database::work() {
    //pthread_setname_np(m_worker.native_handle(), "inscight_worker");
    init();

    meta_info info;
    info.path = progpath();
    info.user = username();
    info.version = sc_core::sc_version();
    info.timestamp = std::time(nullptr);
    info.pid = getpid();
    gen_meta(info);

    while (true) {
        m_mtx.lock();
        m_cv.wait(m_mtx, [&]() -> bool {
            return !m_running || !m_entries.empty();
        });

        std::vector<entry> copy;
        m_entries.swap(copy);
        m_mtx.unlock();

        if (!copy.empty()) {
            begin(copy.size());

            for (const entry& e : copy)
                process(e);

            end(copy.size());
        }

        if (!m_running)
            return;
    }
}

void database::process(const entry& e) {
    switch (e.kind) {
    case MODULE_CREATED:
        module_created(e.id, (const char*)e.arg0, (const char*)e.arg1);
        free((void*)e.arg0);
        free((void*)e.arg1);
        break;

    case PROCESS_CREATED:
        process_created(e.id, (const char*)e.arg0, (proc_kind)e.arg1);
        free((void*)e.arg0);
        break;

    case PORT_CREATED:
        port_created(e.id, (const char*)e.arg0);
        free((void*)e.arg0);
        break;

    case EVENT_CREATED:
        event_created(e.id, (const char*)e.arg0);
        free((void*)e.arg0);
        break;

    case CHANNEL_CREATED:
        channel_created(e.id, (const char*)e.arg0, (const char*)e.arg1);
        free((void*)e.arg0);
        free((void*)e.arg1);
        break;

    case MODULE_PHASE_STARTED:
        module_phase_started(e.id, (module_phase)e.arg0, (real_time_t)e.arg1);
        break;

    case MODULE_PHASE_FINISHED:
        module_phase_finished(e.id, (module_phase)e.arg0, (real_time_t)e.arg1);
        break;

    case PROCESS_START:
        process_start(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    case PROCESS_YIELD:
        process_yield(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    case EVENT_NOTIFY_IMMEDIATE:
        event_notify_immediate(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    case EVENT_NOTIFY_DELTA:
        event_notify_delta(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    case EVENT_NOTIFY_TIMED:
        event_notify_timed(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1, (sysc_time_t)e.arg2);
        break;

    case EVENT_CANCEL:
        event_cancel(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    case CHANNEL_UPDATE_START:
        channel_update_start(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    case CHANNEL_UPDATE_COMPLETE:
        channel_update_complete(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    default:
        fprintf(stderr, "ignoring unknown database entry kind %u\n", e.kind);
        break;
    }
}

void database::start() {
    m_running = true;
    m_worker = std::thread(&database::work, this);
}

void database::stop() {
    if (m_worker.joinable()) {
        m_running = false;
        m_cv.notify_all();
        m_worker.join();
    }
}

database::database(const std::string& options):
    m_running(),
    m_mtx(),
    m_cv(),
    m_entries(),
    m_worker() {
}

database::~database() {
    stop();
}

} // namespace inscight
