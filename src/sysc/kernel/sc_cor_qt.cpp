/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

/*****************************************************************************

  sc_cor_qt.cpp -- Coroutine implementation with QuickThreads.

  Original Author: Martin Janssen, Synopsys, Inc., 2001-12-18

 CHANGE LOG APPEARS AT THE END OF THE FILE
 *****************************************************************************/

#if !defined(_WIN32) && !defined(WIN32) && !defined(SC_USE_PTHREADS)

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <sstream>

#include "sysc/kernel/sc_cor_qt.h"
#include "sysc/kernel/sc_simcontext.h"
#include "sysc/utils/sc_report.h"

namespace sc_core {

// ----------------------------------------------------------------------------
//  Sanitizer helpers
//
//  sc_cor_qt_start_switch   runs right before leaving old_cor, on its stack
//  sc_cor_qt_finish_switch  runs right after entering new_cor, on its stack
//  sc_cor_qt_cleanup        runs when a coroutine (not main) is destroyed
// ----------------------------------------------------------------------------

} // namespace sc_core

#if defined(SC_COR_QT_ASAN)
#include <sanitizer/common_interface_defs.h>
#elif defined(SC_COR_QT_TSAN)
#include <sanitizer/tsan_interface.h>
#endif

namespace sc_core {

#if defined(SC_COR_QT_ASAN)

// Asan must know which stack is in use and keeps a fake stack per coroutine

static inline void sc_cor_qt_start_switch( sc_cor_qt* old_cor, sc_cor_qt* new_cor )
{
    __sanitizer_start_switch_fiber(&old_cor->m_fake_stack, new_cor->m_stack, new_cor->m_stack_size);
}

static inline void sc_cor_qt_finish_switch( sc_cor_qt* old_cor, sc_cor_qt* new_cor )
{
    // also reports the stack of old_cor, which is how we learn the stack of
    // the main coroutine
    auto** stack_bottom = const_cast<const void**>(&old_cor->m_stack);
    __sanitizer_finish_switch_fiber(new_cor->m_fake_stack, stack_bottom, &old_cor->m_stack_size);
}

static inline void sc_cor_qt_cleanup( sc_cor_qt* cor )
{
    if (cor->m_fake_stack == nullptr) {
        return;
    }

    // briefly switch to the fake stack of the coroutine to have it released
    void* save_fake_stack;
    const void* stack_bottom;
    size_t stack_size;

    __sanitizer_start_switch_fiber(&save_fake_stack, nullptr, 0U);
    __sanitizer_finish_switch_fiber(cor->m_fake_stack, &stack_bottom, &stack_size);

    __sanitizer_start_switch_fiber(nullptr, stack_bottom, stack_size);
    __sanitizer_finish_switch_fiber(save_fake_stack, nullptr, nullptr);
}

#elif defined(SC_COR_QT_TSAN)

// Tsan keeps a call stack per fiber; without a fiber per coroutine, the
// frames of all coroutines pile up on the call stack of the thread

static inline void sc_cor_qt_start_switch( sc_cor_qt* old_cor, sc_cor_qt* new_cor )
{
    // the main coroutine runs on the fiber of the thread that started it,
    // all others get their fiber when they are first switched to
    if (old_cor->m_fiber == nullptr) {
        old_cor->m_fiber = __tsan_get_current_fiber();
    }

    if (new_cor->m_fiber == nullptr) {
        new_cor->m_fiber = __tsan_create_fiber(0U);
        new_cor->m_own_fiber = true;
    }

    // flags = 0 synchronizes both fibers: coroutines never run concurrently
    __tsan_switch_to_fiber(new_cor->m_fiber, 0U);
}

static inline void sc_cor_qt_finish_switch( sc_cor_qt*, sc_cor_qt* )
{
}

static inline void sc_cor_qt_cleanup( sc_cor_qt* cor )
{
    if (cor->m_own_fiber) {
        __tsan_destroy_fiber(cor->m_fiber);
    }
}

#else // no sanitizer

static inline void sc_cor_qt_start_switch( sc_cor_qt*, sc_cor_qt* )
{
}

static inline void sc_cor_qt_finish_switch( sc_cor_qt*, sc_cor_qt* )
{
}

static inline void sc_cor_qt_cleanup( sc_cor_qt* )
{
}

#endif

// ----------------------------------------------------------------------------

static std::size_t sc_pagesize()
{
    static std::size_t pagesize = sysconf( _SC_PAGESIZE );
    sc_assert( pagesize != 0 );
    return pagesize;
}

// ----------------------------------------------------------------------------
//  CLASS : sc_cor_qt
//
//  Coroutine class implemented with QuickThreads.
// ----------------------------------------------------------------------------

sc_cor_qt::~sc_cor_qt()
{
    if (SC_UNLIKELY_(this == m_pkg->get_main())) {
        return; // don't delete main stack
    }

#ifdef HAVE_VALGRIND_H
        VALGRIND_STACK_DEREGISTER(m_vgid);
#endif

    if ( m_stack ) {
        ::munmap( m_stack, m_stack_size );
    }

    sc_cor_qt_cleanup( this );
}

// switch stack protection on/off

void
sc_cor_qt::stack_protect( bool enable )
{
    // Code needs to be tested on HP-UX and disabled if it doesn't work there
    // Code still needs to be ported to WIN32

    const std::size_t pagesize = sc_pagesize();
    sc_assert( m_stack_size > ( 2 * pagesize ) );

    std::size_t sp_addr = reinterpret_cast<std::size_t>(m_stack);

#ifdef QUICKTHREADS_GROW_DOWN
    // Stacks grow from high address down to low address
    caddr_t redzone = caddr_t( sp_addr );
#else
    // Stacks grow from low address up to high address
    caddr_t redzone = caddr_t( sp_addr + m_stack_size - pagesize );
#endif

    int ret;

    // Enable the red zone at the end of the stack so that references within
    // it will cause an interrupt.

    if( enable ) {
        ret = mprotect( redzone, pagesize - 1, PROT_NONE );
    }

    // Revert the red zone to normal memory usage.

    else {
        ret = mprotect( redzone, pagesize - 1, PROT_READ | PROT_WRITE );
    }

    if( ret != 0 ) // ignore mprotect error with warning
    {
        static bool mprotect_fail_warned_once = false;
        if( mprotect_fail_warned_once == false )
        {
            mprotect_fail_warned_once = true;

            int mprotect_errno = errno;
            std::stringstream sstr;
            sstr << "unsuccessful stack protection ignored: "
                 << std::strerror(mprotect_errno)
                 << ", address=0x" << std::hex << redzone
                 << ", enable=" << std::boolalpha << enable;

            SC_REPORT_WARNING( SC_ID_COROUTINE_ERROR_
                             , sstr.str().c_str() );
        }
    }
}

// ----------------------------------------------------------------------------
//  CLASS : sc_cor_pkg_qt
//
//  Coroutine package class implemented with QuickThreads.
// ----------------------------------------------------------------------------


// support functions

// allocate aligned stack memory
static inline void*
stack_alloc( void** buf, std::size_t* stack_size )
{
    const std::size_t alignment     = sc_pagesize();
    const std::size_t round_up_mask = alignment - 1;
    sc_assert( 0 == ( alignment & round_up_mask ) ); // power of 2
    sc_assert( buf );

    // round up to multiple of alignment
    *stack_size = (*stack_size + round_up_mask) & ~round_up_mask;
    sc_assert( *stack_size > (alignment * 2) );

    *buf = ::mmap( NULL, *stack_size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANON, -1, 0 );
    if ( *buf == MAP_FAILED ) {
        *buf = NULL;
    }
    return *buf;
}

// constructor

sc_cor_pkg_qt::sc_cor_pkg_qt( sc_simcontext* simc )
  : sc_cor_pkg( simc )
  , m_main_cor()
  , m_curr_cor()
{
    m_main_cor.m_pkg = this;
    m_curr_cor = &m_main_cor;
}


// set current coroutine

sc_cor_qt*
sc_cor_pkg_qt::set_current(sc_cor_qt* cor)
{
    sc_cor_qt* old_cor = m_curr_cor;
    m_curr_cor = cor;
    return old_cor;
}


// destructor

sc_cor_pkg_qt::~sc_cor_pkg_qt()
{
}


// create a new coroutine

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#pragma GCC diagnostic ignored "-Warray-bounds"
extern "C"
void
sc_cor_qt_wrapper( void* arg, void* cor, qt_userf_t* fn )
{
    sc_cor_qt* new_cor = static_cast<sc_cor_qt*>( cor );
    new_cor->m_pkg->set_current( new_cor );
    // invoke the user function
    (*(sc_cor_fn*) fn)( arg );
    // not reached
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
sc_cor*
sc_cor_pkg_qt::create( std::size_t stack_size, sc_cor_fn* fn, void* arg )
{
    sc_cor_qt* cor = new sc_cor_qt();
    cor->m_pkg = this;
    cor->m_stack_size = stack_size;

    void* aligned_sp = stack_alloc( &cor->m_stack, &cor->m_stack_size );
    if( aligned_sp == NULL )
    {
        SC_REPORT_ERROR( SC_ID_COROUTINE_ERROR_
                       , "failed to allocate stack memory" );
        sc_abort();
    }

#ifdef HAVE_VALGRIND_H
    cor->m_vgid = VALGRIND_STACK_REGISTER(cor->m_stack, (char*)cor->m_stack + cor->m_stack_size - 1);
#endif
    cor->m_sp = QUICKTHREADS_SP( aligned_sp, cor->m_stack_size );
    cor->m_sp = QUICKTHREADS_ARGS( cor->m_sp, arg, cor, (qt_userf_t*) fn,
                                   sc_cor_qt_wrapper );
    return cor;
}
#pragma GCC diagnostic pop


// yield to the next coroutine

extern "C"
void*
sc_cor_qt_yieldhelp( qt_t* sp, void* old_cor_p, void* new_cor_p )
{
    auto* old_cor = static_cast<sc_cor_qt*>( old_cor_p );
    auto* new_cor = static_cast<sc_cor_qt*>( new_cor_p );

    old_cor->m_sp = sp;
    sc_cor_qt_finish_switch( old_cor, new_cor );
    return 0;
}

void
sc_cor_pkg_qt::yield( sc_cor* next_cor )
{
    auto* new_cor = static_cast<sc_cor_qt*>( next_cor );
    auto* old_cor = set_current( new_cor );

    sc_cor_qt_start_switch( old_cor, new_cor );
    QUICKTHREADS_BLOCK( sc_cor_qt_yieldhelp, old_cor, new_cor, new_cor->m_sp );
}


// abort the current coroutine (and resume the next coroutine)

extern "C"
void*
sc_cor_qt_aborthelp( qt_t*, void* old_cor_p, void* new_cor_p )
{
    auto* old_cor = static_cast<sc_cor_qt*>( old_cor_p );
    auto* new_cor = static_cast<sc_cor_qt*>( new_cor_p );

    sc_cor_qt_finish_switch( old_cor, new_cor );
    return 0;
}

void
sc_cor_pkg_qt::abort( sc_cor* next_cor )
{
    auto* new_cor = static_cast<sc_cor_qt*>( next_cor );
    auto* old_cor = set_current( new_cor );

    sc_cor_qt_start_switch( old_cor, new_cor );
    QUICKTHREADS_ABORT( sc_cor_qt_aborthelp, old_cor, new_cor, new_cor->m_sp );
}


// get the main coroutine

sc_cor*
sc_cor_pkg_qt::get_main()
{
    return &m_main_cor;
}

} // namespace sc_core

#endif

// $Log: sc_cor_qt.cpp,v $
// Revision 1.9  2011/08/29 18:04:32  acg
//  Philipp A. Hartmann: miscellaneous clean ups.
//
// Revision 1.8  2011/08/26 20:46:09  acg
//  Andy Goodrich: moved the modification log to the end of the file to
//  eliminate source line number skew when check-ins are done.
//
// Revision 1.7  2011/02/18 20:27:14  acg
//  Andy Goodrich: Updated Copyrights.
//
// Revision 1.6  2011/02/13 21:47:37  acg
//  Andy Goodrich: update copyright notice.
//
// Revision 1.5  2010/08/03 16:52:14  acg
//  Andy Goodrich: line formatting.
//
// Revision 1.4  2008/11/11 14:03:07  acg
//  Andy Goodrich: added execute access to the release of red zone storage
//  per Ulli's suggestion.
//
// Revision 1.3  2008/05/22 17:06:25  acg
//  Andy Goodrich: updated copyright notice to include 2008.
//
// Revision 1.2  2008/03/24 18:32:36  acg
//  Andy Goodrich: added include of sys/types.h to pick up the declaration
//  of caddr_t.
//
// Revision 1.1.1.1  2006/12/15 20:20:05  acg
// SystemC 2.3
//
// Revision 1.3  2006/01/13 18:44:29  acg
// Added $Log to record CVS changes into the source.

// Taf!
