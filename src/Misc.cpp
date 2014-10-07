/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
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
