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

#ifndef INSCIGHT_H
#define INSCIGHT_H

#ifdef HAVE_INSCIGHT
#include "inscight/id.h"
#include "inscight/context.h"
#include "inscight/entry.h"
#include "inscight/database.h"
#define INSCIGHT_TRACE(...) do {             \
    if (::inscight::ctx != nullptr)          \
        ::inscight::ctx->trace(__VA_ARGS__); \
} while (0)
#else
#define INSCIGHT_TRACE(...)
#endif

#define INSCIGHT_MODULE_CREATED(obj, name, kind) \
    INSCIGHT_TRACE(::inscight::MODULE_CREATED, obj, strdup(name), strdup(kind))
#define INSCIGHT_PROCESS_CREATED(obj, name, kind) \
    INSCIGHT_TRACE(::inscight::PROCESS_CREATED, obj, strdup(name), kind)
#define INSCIGHT_EVENT_CREATED(obj, name) \
    INSCIGHT_TRACE(::inscight::EVENT_CREATED, obj, strdup(name))
#define INSCIGHT_PORT_CREATED(obj, name) \
    INSCIGHT_TRACE(::inscight::PORT_CREATED, obj, strdup(name))
#define INSCIGHT_CHANNEL_CREATED(obj, name, kind) \
    INSCIGHT_TRACE(::inscight::CHANNEL_CREATED, obj, strdup(name), strdup(kind))

#define INSCIGHT_MODULE_PHASE_STARTED(obj, phase)                             \
    INSCIGHT_TRACE(::inscight::MODULE_PHASE_STARTED, obj, ::inscight::phase,  \
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
#define INSCIGHT_EVENT_NOTIFY_DELTA(obj) \
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

#endif
