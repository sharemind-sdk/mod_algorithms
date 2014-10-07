/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include "TopKSortingNetwork.h"

#include <cassert>
#include "CatchModuleApiErrors.h"
#include "ModuleData.h"
#include "TopKSortingNetworkGenerator.h"


extern "C" {

/**
 * Mandatory arguments: uint32 size of array and uint32 number of
 * elements that need to be sorted.
 * Return value: the size of the sorting network.
 */
SHAREMIND_MODULE_API_0x1_SYSCALL(TopKSortingNetwork_serializedSize,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    assert(c);
    assert(c->moduleHandle);

    if (num_args != 2u || crefs || refs || !returnValue)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    const uint64_t elements = args[0u].uint64[0u];
    const uint64_t k = args[1u].uint64[0u];

    TopKSortingNetworkGenerator & generator =
            static_cast<ModuleData *>(c->moduleHandle)
                ->topKSortingNetworkGenerator;
    try {
        static_assert(sizeof(size_t) <= sizeof(returnValue->uint64[0u]),
                      "sizeof(size_t) <= sizeof(returnValue->uint64[0u])");
        auto const network =
                generator.getCachedOrGenerateAndCacheNetwork(elements, k);
        if (!network)
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        returnValue->uint64[0u] = network->serializedSize();
    } catch (...) {
        return catchModuleApiErrors();
    }

    return SHAREMIND_MODULE_API_0x1_OK;
}

/**
 * Mandatory arguments: uint64 size of array, uint64 number of
 * elements to sort, uint64 ref to the array where the sorting network
 * is stored.
 * No return value.
 */
SHAREMIND_MODULE_API_0x1_SYSCALL(TopKSortingNetwork_serialize,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    assert(c);
    assert(c->moduleHandle);

    if (num_args != 2u || crefs || !refs
        || (assert(refs[0u].pData), refs[1u].pData)
        || returnValue)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    // Get the inputs
    const uint64_t elements = args[0u].uint64[0u];
    const uint64_t k = args[1u].uint64[0u];
    uint64_t * const arrayStart = static_cast<uint64_t *>(refs[0u].pData);

    // Note that this strips the remainder 1 byte used by SecreC:
    const size_t availableStorageSize = refs[0u].size / sizeof(uint64_t);

    TopKSortingNetworkGenerator & generator =
            static_cast<ModuleData *>(c->moduleHandle)
                ->topKSortingNetworkGenerator;

    try {
        auto const network =
                generator.getCachedOrGenerateAndCacheNetwork(elements, k);
        if (!network)
            return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
        if (availableStorageSize != network->serializedSize())
            return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
        network->serialize(arrayStart);
        return SHAREMIND_MODULE_API_0x1_OK;
    } catch (...) {
        return catchModuleApiErrors();
    }
}

} // extern "C" {
