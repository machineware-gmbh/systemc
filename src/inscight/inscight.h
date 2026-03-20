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

#ifndef INSCIGHT_H
#define INSCIGHT_H

#ifdef HAVE_INSCIGHT
#include "inscight/id.h"
#include "inscight/context.h"
#include "inscight/entry.h"
#include "inscight/database.h"
#define INSCIGHT_TRACE(...)                      \
    do {                                         \
        if (::inscight::ctx != nullptr)          \
            ::inscight::ctx->trace(__VA_ARGS__); \
    } while (0)
#else
#define INSCIGHT_TRACE(...) do {} while (0)
#endif

#define INSCIGHT_MODULE_CREATED(obj, name, kind) \
    INSCIGHT_TRACE(::inscight::MODULE_CREATED, obj, strdup(name), strdup(kind))
#define INSCIGHT_PROCESS_CREATED(obj, name, kind) \
    INSCIGHT_TRACE(::inscight::PROCESS_CREATED, obj, strdup(name), kind)
#define INSCIGHT_EVENT_CREATED(obj, name) \
    INSCIGHT_TRACE(::inscight::EVENT_CREATED, obj, strdup(name))
#define INSCIGHT_PORT_CREATED(obj, name) \
    INSCIGHT_TRACE(::inscight::PORT_CREATED, obj, strdup(name))
#define INSCIGHT_CHANNEL_CREATED(obj, name, kind)                  \
    INSCIGHT_TRACE(::inscight::CHANNEL_CREATED, obj, strdup(name), \
                   strdup(kind))

#define INSCIGHT_PORT_BOUND(from, to, kind, type)                      \
    INSCIGHT_TRACE(::inscight::PORT_BOUND, from, to, ::inscight::kind, \
                   ::inscight::protocol_from_str(type))

#define INSCIGHT_MODULE_PHASE_STARTED(obj, phase)                            \
    INSCIGHT_TRACE(::inscight::MODULE_PHASE_STARTED, obj, ::inscight::phase, \
                   ::inscight::real_time_stamp())
#define INSCIGHT_MODULE_PHASE_FINISHED(obj, phase)                            \
    INSCIGHT_TRACE(::inscight::MODULE_PHASE_FINISHED, obj, ::inscight::phase, \
                   ::inscight::real_time_stamp())

#define INSCIGHT_PROCESS_START(obj)                \
    INSCIGHT_TRACE(::inscight::PROCESS_START, obj, \
                   ::inscight::real_time_stamp(),  \
                   ::inscight::sysc_time_stamp())
#define INSCIGHT_PROCESS_YIELD(obj)                \
    INSCIGHT_TRACE(::inscight::PROCESS_YIELD, obj, \
                   ::inscight::real_time_stamp(),  \
                   ::inscight::sysc_time_stamp())

#define INSCIGHT_EVENT_NOTIFY_IMMEDIATE(obj)                \
    INSCIGHT_TRACE(::inscight::EVENT_NOTIFY_IMMEDIATE, obj, \
                   ::inscight::real_time_stamp(),           \
                   ::inscight::sysc_time_stamp())
#define INSCIGHT_EVENT_NOTIFY_DELTA(obj)                \
    INSCIGHT_TRACE(::inscight::EVENT_NOTIFY_DELTA, obj, \
                   ::inscight::real_time_stamp(),       \
                   ::inscight::sysc_time_stamp())
#define INSCIGHT_EVENT_NOTIFY_TIMED(obj, delay)         \
    INSCIGHT_TRACE(::inscight::EVENT_NOTIFY_TIMED, obj, \
                   ::inscight::real_time_stamp(),       \
                   ::inscight::sysc_time_stamp(),       \
                   delay / sc_core::sc_time(1.0, sc_core::SC_PS))
#define INSCIGHT_EVENT_CANCEL(obj)                \
    INSCIGHT_TRACE(::inscight::EVENT_CANCEL, obj, \
                   ::inscight::real_time_stamp(), \
                   ::inscight::sysc_time_stamp())

#define INSCIGHT_CHANNEL_UPDATE_START(obj)                \
    INSCIGHT_TRACE(::inscight::CHANNEL_UPDATE_START, obj, \
                   ::inscight::real_time_stamp(),         \
                   ::inscight::sysc_time_stamp())
#define INSCIGHT_CHANNEL_UPDATE_COMPLETE(obj)                \
    INSCIGHT_TRACE(::inscight::CHANNEL_UPDATE_COMPLETE, obj, \
                   ::inscight::real_time_stamp(),            \
                   ::inscight::sysc_time_stamp())

#define INSCIGHT_CPU_IDLE_ENTER(obj)                       \
    INSCIGHT_TRACE(::inscight::CPU_IDLE_ENTER, (obj).id(), \
                   ::inscight::sysc_time_stamp())

#define INSCIGHT_CPU_IDLE_LEAVE(obj)                       \
    INSCIGHT_TRACE(::inscight::CPU_IDLE_LEAVE, (obj).id(), \
                   ::inscight::sysc_time_stamp())

#define INSCIGHT_CPU_CALL_STACK(obj, t, lvl, addr, sym)    \
    INSCIGHT_TRACE(::inscight::CPU_CALL_STACK, (obj).id(), \
                   t, lvl, addr, strdup(sym ? sym : ""))

#define INSCIGHT_TRANSACTION_TRACE_FW(obj, t, proto, txjson)     \
    INSCIGHT_TRACE(::inscight::TRANSACTION_TRACE_FW, (obj).id(), \
                   t, proto, strdup(txjson))
#define INSCIGHT_TRANSACTION_TRACE_BW(obj, t, proto, txjson)     \
    INSCIGHT_TRACE(::inscight::TRANSACTION_TRACE_BW, (obj).id(), \
                   t, proto, strdup(txjson))

#define INSCIGHT_BTRANSPORT_FW(port, payload)                    \
    INSCIGHT_TRACE(::inscight::BTRANSPORT_FW, port,              \
                   reinterpret_cast<::inscight::id_t>(&payload), \
                   ::inscight::real_time_stamp(),                \
                   ::inscight::sysc_time_stamp());
#define INSCIGHT_BTRANSPORT_BW(port, payload)                    \
    INSCIGHT_TRACE(::inscight::BTRANSPORT_BW, port,              \
                   reinterpret_cast<::inscight::id_t>(&payload), \
                   ::inscight::real_time_stamp(),                \
                   ::inscight::sysc_time_stamp());

#define INSCIGHT_LOG_MESSAGE(lvl, sender, msg)         \
    INSCIGHT_TRACE(::inscight::LOG_MESSAGE, 0,         \
                   ::inscight::sysc_time_stamp(), lvl, \
                   strdup(sender ? sender : ""),       \
                   strdup(msg))

#define INSCIGHT_QUANTUM_UPDATE(oldq, newq)       \
    INSCIGHT_TRACE(::inscight::QUANTUM_UPDATE, 0, \
                   ::inscight::sysc_time_stamp(), \
                   ::inscight::to_picos(oldq),    \
                   ::inscight::to_picos(newq))

#define INSCIGHT_KTHREAD_STARTED()                                              \
    INSCIGHT_TRACE(::inscight::KTHREAD_EVENT, 0, ::inscight::real_time_stamp(), \
                   ::inscight::KTHREAD_STARTED)
#define INSCIGHT_KTHREAD_SUSPENDED()                                            \
    INSCIGHT_TRACE(::inscight::KTHREAD_EVENT, 0, ::inscight::real_time_stamp(), \
                   ::inscight::KTHREAD_SUSPENDED)
#define INSCIGHT_KTHREAD_RESUMED()                                              \
    INSCIGHT_TRACE(::inscight::KTHREAD_EVENT, 0, ::inscight::real_time_stamp(), \
                   ::inscight::KTHREAD_RESUMED)
#define INSCIGHT_KTHREAD_THROTTLED()                                            \
    INSCIGHT_TRACE(::inscight::KTHREAD_EVENT, 0, ::inscight::real_time_stamp(), \
                   ::inscight::KTHREAD_THROTTLED)
#define INSCIGHT_KTHREAD_UNTHROTTLED()                                          \
    INSCIGHT_TRACE(::inscight::KTHREAD_EVENT, 0, ::inscight::real_time_stamp(), \
                   ::inscight::KTHREAD_UNTHROTTLED)
#define INSCIGHT_KTHREAD_EXITED()                                               \
    INSCIGHT_TRACE(::inscight::KTHREAD_EVENT, 0, ::inscight::real_time_stamp(), \
                   ::inscight::KTHREAD_EXITED)

#define INSCIGHT_IRQ_LEVEL(obj, irqid, state)                                   \
    INSCIGHT_TRACE(::inscight::IRQ_EVENT, obj, inscight::real_time_stamp(),     \
                   ::inscight::sysc_time_stamp(), irqid,                        \
                   state ? ::inscight::IRQ_LEVEL_HI : ::inscight::IRQ_LEVEL_LO)
#define INSCIGHT_IRQ_EDGE(obj, irqid, state)                                    \
    INSCIGHT_TRACE(::inscight::IRQ_EVENT, obj, inscight::real_time_stamp(),     \
                   ::inscight::sysc_time_stamp(), irqid,                        \
                   state ? ::inscight::IRQ_EDGE_RISE : ::inscight::IRQ_EDGE_FALL)
#define INSCIGHT_IRQ_MSI(obj, irqid)                                            \
    INSCIGHT_TRACE(::inscight::IRQ_EVENT, obj, inscight::real_time_stamp(),     \
                   ::inscight::sysc_time_stamp(), irqid, ::inscight::IRQ_MSI)

#endif
