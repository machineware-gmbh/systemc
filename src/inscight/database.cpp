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
#include <signal.h>

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

#elif defined(__APPLE__)
#include <unistd.h>
#include <mach-o/dyld.h>

static std::string progpath() {
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size))
        return "unknown";

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

#elif defined(_MSC_VER)
#include <Windows.h>

static std::string progpath() {
    TCHAR path[MAX_PATH] = {};
    if (GetModuleFileName(NULL, path, MAX_PATH) == 0)
        return "unknown";
    return std::string(path);
}

static std::string username() {
    TCHAR name[MAX_PATH] = {};
    DWORD namelen = sizeof(name);
    if (GetUserName(name, &namelen))
        return std::string(name);
    return "unkown";
}

#else

static std::string progpath() {
    return "unknown";
}

static std::string username() {
    return "unknown";
}

#endif

namespace inscight {

static bool rt_trace_enabled = true;

#ifdef __linux__
static void handle_sigusr(int sig) {
    rt_trace_enabled = !rt_trace_enabled;

    const char* msg;
    if (rt_trace_enabled)
        msg = "INSCIGHT: rt-tracing enabled\n";
    else
        msg = "INSCIGHT: rt-tracing disabled\n";

    ssize_t r = write(STDERR_FILENO, msg, strlen(msg));
    (void)r;
}
#endif

void database::work() {
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

    case PORT_BOUND:
        port_bound(e.id, (id_t)e.arg0, (binding_kind)e.arg1, (protocol_kind)e.arg2);
        break;

    case MODULE_PHASE_STARTED:
        module_phase_started(e.id, (module_phase)e.arg0, (real_time_t)e.arg1);
        break;

    case MODULE_PHASE_FINISHED:
        module_phase_finished(e.id, (module_phase)e.arg0, (real_time_t)e.arg1);
        break;

    case PROCESS_START:
        m_enabled = rt_trace_enabled;
        if (m_enabled)
            process_start(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    case PROCESS_YIELD:
        if (m_enabled)
            process_yield(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        m_enabled = rt_trace_enabled;
        break;

    case EVENT_NOTIFY_IMMEDIATE:
        if (m_enabled)
            event_notify_immediate(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    case EVENT_NOTIFY_DELTA:
        if (m_enabled)
            event_notify_delta(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    case EVENT_NOTIFY_TIMED:
        if (m_enabled)
            event_notify_timed(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1, (sysc_time_t)e.arg2);
        break;

    case EVENT_CANCEL:
        if (m_enabled)
            event_cancel(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    case CHANNEL_UPDATE_START:
        if (m_enabled)
            channel_update_start(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    case CHANNEL_UPDATE_COMPLETE:
        if (m_enabled)
            channel_update_complete(e.id, (real_time_t)e.arg0, (sysc_time_t)e.arg1);
        break;

    case CPU_IDLE_ENTER:
        if (m_enabled)
            cpu_idle_enter(e.id, (sysc_time_t)e.arg0);
        break;

    case CPU_IDLE_LEAVE:
        if (m_enabled)
            cpu_idle_leave(e.id, (sysc_time_t)e.arg0);
        break;

    case CPU_CALL_STACK:
        if (m_enabled)
            cpu_call_stack(e.id, (sysc_time_t)e.arg0, (size_t)e.arg1, (unsigned long long)e.arg2, (const char*)e.arg3);
        free((void*)e.arg3);
        break;

    case TRANSACTION_TRACE_FW:
        if (m_enabled)
            transaction_trace_fw(e.id, (sysc_time_t)e.arg0, (protocol_kind)e.arg1, (const char*)e.arg2);
        free((void*)e.arg2);
        break;

    case TRANSACTION_TRACE_BW:
        if (m_enabled)
            transaction_trace_bw(e.id, (sysc_time_t)e.arg0, (protocol_kind)e.arg1, (const char*)e.arg2);
        free((void*)e.arg2);
        break;

    case LOG_MESSAGE:
        if (m_enabled)
            log_message((sysc_time_t)e.arg0, (int)e.arg1, (const char*)e.arg2, (const char*)e.arg3);
        free((void*)e.arg2);
        free((void*)e.arg3);
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
    m_enabled(true),
    m_running(),
    m_mtx(),
    m_cv(),
    m_entries(),
    m_worker() {
#ifdef __linux__
    signal(SIGUSR2, handle_sigusr);
#endif
}

database::~database() {
    stop();
}

} // namespace inscight
