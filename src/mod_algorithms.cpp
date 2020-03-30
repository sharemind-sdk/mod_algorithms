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

#include <cassert>
#include <sharemind/module-apis/api_0x1.h>
#include "Erf.h"
#include "Exp.h"
#include "BlockSortPermutation.h"
#include "CatchModuleApiErrors.h"
#include "Misc.h"
#include "ModuleData.h"
#include "Log.h"
#include "Sine.h"
#include "SortingNetwork.h"
#include "SquareRoot.h"
#include "TopKSortingNetwork.h"
#include "ToString.h"


extern "C" {

SHAREMIND_MODULE_API_MODULE_INFO("algorithms",
                                 0x00010000,   /* Version 0.1.0.0 */
                                 0x1);         /* Support API version 1. */

SHAREMIND_MODULE_API_0x1_INITIALIZER(c) __attribute__ ((visibility("default")));
SHAREMIND_MODULE_API_0x1_INITIALIZER(c) {
    assert(c);
    try {
        c->moduleHandle = new ModuleData();
        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors();
    }
}

SHAREMIND_MODULE_API_0x1_DEINITIALIZER(c)
        __attribute__ ((visibility("default")));
SHAREMIND_MODULE_API_0x1_DEINITIALIZER(c) {
    assert(c);
    assert(c->moduleHandle);
    delete static_cast<ModuleData *>(c->moduleHandle);
    #ifndef NDEBUG
    c->moduleHandle = nullptr; // Not needed, but may help debugging.
    #endif
}

#define SAMENAME(f) { #f, &(f) }

SHAREMIND_MODULE_API_0x1_SYSCALL_DEFINITIONS(

    // String syscalls:
    SAMENAME(int8_toString),
    SAMENAME(int16_toString),
    SAMENAME(int32_toString),
    SAMENAME(int64_toString),
    SAMENAME(uint8_toString),
    SAMENAME(uint16_toString),
    SAMENAME(uint32_toString),
    SAMENAME(uint64_toString),
    SAMENAME(float32_toString),
    SAMENAME(float64_toString),

    // Mathematics syscalls:
    SAMENAME(float32_erf),
    SAMENAME(float64_erf),
    SAMENAME(float32_exp),
    SAMENAME(float64_exp),
    SAMENAME(float32_log),
    SAMENAME(float64_log),
    SAMENAME(float32_sin),
    SAMENAME(float64_sin),
    SAMENAME(float32_sqrt),
    SAMENAME(float64_sqrt),

    // Sorting network syscalls:
    SAMENAME(SortingNetwork_serializedSize),
    SAMENAME(SortingNetwork_serialize),
    SAMENAME(MergingNetwork_serializedSize),
    SAMENAME(MergingNetwork_serialize),
    SAMENAME(TopKSortingNetwork_serializedSize),
    SAMENAME(TopKSortingNetwork_serialize),

    // Misc. syscalls:
    SAMENAME(sleepMilliseconds),

    // Templated syscalls:
    // BlockSortPermutation syscalls:
    { "public_uint16_sort_permutation", &blockSortPermutation<uint16_t, 1u> },
    { "public_uint32_sort_permutation", &blockSortPermutation<uint32_t, 1u> },
    { "public_uint32_x4_block_sort_permutation", &blockSortPermutation<uint32_t, 4u> }

);

} // extern "C" {
