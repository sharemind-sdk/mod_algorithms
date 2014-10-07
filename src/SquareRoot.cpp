/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include "ToString.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <limits>
#include <sharemind/3rdparty/libsoftfloat/softfloat.h>
#include <sstream>


SHAREMIND_EXTERN_C_BEGIN

/*
 * Mandatory cref parameter: public float32 vector
 * Mandatory ref parameter: result vector
 */
SHAREMIND_MODULE_API_0x1_SYSCALL(float32_sqrt,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    (void) c;
    (void) args;

    if (num_args != 0u || returnValue || !crefs || !refs)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    assert(crefs[0u].pData);
    assert(refs[0u].pData);

    if (crefs[0u].size != refs[0u].size)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    const sf_float32 * const in =
            static_cast<const sf_float32 *>(crefs[0u].pData);
    sf_float32 * const out =
            static_cast<sf_float32 *>(refs[0u].pData);

    for (size_t i = 0u; i < crefs[0u].size / sizeof(sf_float32); i++)
        out[i] = sf_float32_sqrt(in[i], sf_fpu_state_default).result;

    return SHAREMIND_MODULE_API_0x1_OK;
}

/*
 * Mandatory cref parameter: public float64 vector
 * Mandatory ref parameter: result vector
 */
SHAREMIND_MODULE_API_0x1_SYSCALL(float64_sqrt,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    (void) c;
    (void) args;

    if (num_args != 0u || returnValue || !crefs || !refs)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    assert(crefs[0u].pData);
    assert(refs[0u].pData);

    if (crefs[0u].size != refs[0u].size)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    const sf_float64 * const in =
            static_cast<const sf_float64 *>(crefs[0u].pData);
    sf_float64 * const out =
            static_cast<sf_float64 *>(refs[0u].pData);

    for (size_t i = 0u; i < crefs[0u].size / sizeof(sf_float64); i++)
        out[i] = sf_float64_sqrt(in[i], sf_fpu_state_default).result;

    return SHAREMIND_MODULE_API_0x1_OK;
}

SHAREMIND_EXTERN_C_END
