/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include <cassert>
#include <sharemind/libmodapi/api_0x1.h>
#include "CatchModuleApiErrors.h"
#include "Misc.h"
#include "ModuleData.h"
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
    SAMENAME(float32_sqrt),
    SAMENAME(float64_sqrt),

    // Sorting network syscalls:
    SAMENAME(SortingNetwork_serializedSize),
    SAMENAME(SortingNetwork_serialize),
    SAMENAME(TopKSortingNetwork_serializedSize),
    SAMENAME(TopKSortingNetwork_serialize),

    // Misc. syscalls:
    SAMENAME(sleepMilliseconds)

);

} // extern "C" {
