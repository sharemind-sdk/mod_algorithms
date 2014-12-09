/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_MOD_ALGORITHMS_INTERSECTION_H
#define SHAREMIND_MOD_ALGORITHMS_INTERSECTION_H

#include "DeclareSyscall.h"
#include <sharemind/miner/libconsensusservice.h>

SHAREMIND_MOD_ALGORITHMS_DECLARE_SYSCALL(intersection)

bool equivalent(const SharemindConsensusDatum * proposals, size_t count)
    __attribute__((visibility("hidden")));

SharemindConsensusResultType execute(const SharemindConsensusDatum * proposals,
                                     size_t count,
                                     void * callbackPtr)
    __attribute__((visibility("hidden")));

void commit(const SharemindConsensusDatum * proposals,
            size_t count,
            const SharemindConsensusResultType * results,
            void * callbackPtr)
    __attribute__((visibility("hidden")));

#endif /* SHAREMIND_MOD_ALGORITHMS_INTERSECTION_H */
