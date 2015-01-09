/*
 * Copyright (C) 2015 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#include "Misc.h"

#include <cassert>
#include <chrono>
#include <thread>


SHAREMIND_EXTERN_C_BEGIN

/*
 * Mandatory argument: uint64 milliseconds to sleep
 * No return value.
 */
SHAREMIND_MODULE_API_0x1_SYSCALL(sleepMilliseconds,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    (void) c;
    if (num_args != 1u || crefs || refs || returnValue)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    /// \note http://gcc.gnu.org/bugzilla/show_bug.cgi?id=60421
    typedef std::chrono::duration<uint64_t, std::milli> UnsignedMilliseconds;
    std::this_thread::sleep_for(UnsignedMilliseconds(args[0u].uint64[0u]));
    return SHAREMIND_MODULE_API_0x1_OK;
}

SHAREMIND_EXTERN_C_END
