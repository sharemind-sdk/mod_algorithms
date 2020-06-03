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

#include "SortingNetwork.h"

#include <cassert>
#include <limits>
#include "CatchModuleApiErrors.h"
#include "ModuleData.h"
#include "SortingNetworkGenerator.h"


SHAREMIND_EXTERN_C_BEGIN

/**
 * Mandatory argument: uint64 size of array to sort
 * Return value: the size of the sorting network.
 */
SHAREMIND_MODULE_API_0x1_SYSCALL(SortingNetwork_serializedSize,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    assert(c);
    assert(c->moduleHandle);

    static_assert(std::numeric_limits<size_t>::max()
                  == std::numeric_limits<uint64_t>::max(),
                  "std::numeric_limits<size_t>::max() "
                  "== std::numeric_limits<uint64_t>::max()");
    if (num_args != 1u || crefs || refs || !returnValue)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    const uint64_t elementCount = args[0u].uint64[0u];

    if (elementCount < 1)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    SortingNetworkGenerator<SortingNetwork> & generator =
            static_cast<ModuleData *>(c->moduleHandle)
                ->sortingNetworkGenerator;

    try {
        const auto r =
                generator.getCachedOrGenerateAndCacheNetwork(elementCount);
        if (!r) {
            /// \bug This might actually be OOM or invalid argument
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }
        returnValue->uint64[0u] = r->serializedSize();
    } catch (...) {
        return catchModuleApiErrors();
    }
    return SHAREMIND_MODULE_API_0x1_OK;
}

/**
 * Mandatory ref argument: uint64 size of array to sort
 * No return value.
 */
SHAREMIND_MODULE_API_0x1_SYSCALL(SortingNetwork_serialize,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    assert(c);
    assert(c->moduleHandle);

    if (num_args != 1u || crefs || !refs
        || (static_cast<void>(assert(refs[0u].pData)), refs[1u].pData)
        || returnValue)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;


    // Get the inputs
    static_assert(std::numeric_limits<size_t>::max()
                  >= std::numeric_limits<uint64_t>::max(),
                  "std::numeric_limits<size_t>::max() "
                  ">= std::numeric_limits<uint64_t>::max()");
    const size_t elementCount = args[0u].uint64[0u];
    uint64_t * const arrayStart = static_cast<uint64_t *>(refs[0u].pData);

    if (elementCount < 1)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    // Note that this strips the remainder 1 byte used by SecreC:
    const size_t availableStorageSize = refs[0u].size / sizeof(uint64_t);

    // Get the sorting network service
    SortingNetworkGenerator<SortingNetwork> & generator =
            static_cast<ModuleData *>(c->moduleHandle)
                ->sortingNetworkGenerator;

    try {
        // Do we have enough storage?
        const auto r =
                generator.getCachedOrGenerateAndCacheNetwork(elementCount);
        if (!r) {
            /// \bug This might actually be OOM or invalid argument
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }
        if (r->serializedSize() != availableStorageSize)
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
        r->serialize(arrayStart);
    } catch (...) {
        return catchModuleApiErrors();
    }
    return SHAREMIND_MODULE_API_0x1_OK;
}

/**
 * Mandatory argument: uint64 size of array to sort
 * Return value: the size of the merging network.
 */
SHAREMIND_MODULE_API_0x1_SYSCALL(MergingNetwork_serializedSize,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    assert(c);
    assert(c->moduleHandle);

    static_assert(std::numeric_limits<size_t>::max()
                  == std::numeric_limits<uint64_t>::max(),
                  "std::numeric_limits<size_t>::max() "
                  "== std::numeric_limits<uint64_t>::max()");
    if (num_args != 1u || crefs || refs || !returnValue)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    const uint64_t elementCount = args[0u].uint64[0u];

    if (elementCount < 1)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    SortingNetworkGenerator<MergingNetwork> & generator =
            static_cast<ModuleData *>(c->moduleHandle)
                ->mergingNetworkGenerator;

    try {
        const auto r =
                generator.getCachedOrGenerateAndCacheNetwork(elementCount);
        if (!r) {
            /// \bug This might actually be OOM or invalid argument
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }
        returnValue->uint64[0u] = r->serializedSize();
    } catch (...) {
        return catchModuleApiErrors();
    }
    return SHAREMIND_MODULE_API_0x1_OK;
}

/**
 * Mandatory ref argument: uint64 size of array to sort
 * No return value.
 *
 * NOTE that this returns a merging network not a merge sort
 * network. It expects an input where two halves have already been
 * sorted and it will merge the halves.
 */
SHAREMIND_MODULE_API_0x1_SYSCALL(MergingNetwork_serialize,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    assert(c);
    assert(c->moduleHandle);

    if (num_args != 1u || crefs || !refs
        || (assert(refs[0u].pData), refs[1u].pData) || returnValue)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;


    // Get the inputs
    static_assert(std::numeric_limits<size_t>::max()
                  >= std::numeric_limits<uint64_t>::max(),
                  "std::numeric_limits<size_t>::max() "
                  ">= std::numeric_limits<uint64_t>::max()");
    const size_t elementCount = args[0u].uint64[0u];
    uint64_t * const arrayStart = static_cast<uint64_t *>(refs[0u].pData);

    if (elementCount < 1)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    // Note that this strips the remainder 1 byte used by SecreC:
    const size_t availableStorageSize = refs[0u].size / sizeof(uint64_t);

    // Get the sorting network service
    SortingNetworkGenerator<MergingNetwork> & generator =
            static_cast<ModuleData *>(c->moduleHandle)
                ->mergingNetworkGenerator;

    try {
        // Do we have enough storage?
        const auto r =
                generator.getCachedOrGenerateAndCacheNetwork(elementCount);
        if (!r) {
            /// \bug This might actually be OOM or invalid argument
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        }
        if (r->serializedSize() != availableStorageSize)
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
        r->serialize(arrayStart);
    } catch (...) {
        return catchModuleApiErrors();
    }
    return SHAREMIND_MODULE_API_0x1_OK;
}

SHAREMIND_EXTERN_C_END
