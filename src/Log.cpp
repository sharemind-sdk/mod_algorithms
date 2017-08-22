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

#include "ToString.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <limits>
#include <sharemind/libsoftfloat/softfloat.h>
#include <sharemind/libsoftfloat_math/sf_log.h>
#include <sstream>

SHAREMIND_EXTERN_C_BEGIN

/*
 * Mandatory cref parameter: public float32 vector
 * Mandatory ref parameter: result vector
 */
SHAREMIND_MODULE_API_0x1_SYSCALL(float32_log,
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
        out[i] = sf_float32_log(in[i], sf_fpu_state_default).result;

    return SHAREMIND_MODULE_API_0x1_OK;
}

/*
 * Mandatory cref parameter: public float64 vector
 * Mandatory ref parameter: result vector
 */
SHAREMIND_MODULE_API_0x1_SYSCALL(float64_log,
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
        out[i] = sf_float64_log(in[i], sf_fpu_state_default).result;

    return SHAREMIND_MODULE_API_0x1_OK;
}

SHAREMIND_EXTERN_C_END
