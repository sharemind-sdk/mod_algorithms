/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include "Intersection.h"

#include <cassert>
#include <cstring>
#include <sharemind/miner/libconsensusservice.h>
#include <sharemind/miner/libprocessfacility.h>
#include "ModuleData.h"

struct TransactionData {

    TransactionData(const void * const localData_,
                    void * const localExists_,
                    uint64_t elementSize_,
                    size_t elementCount_)
        : localData(static_cast<const uint8_t *>(localData_))
        , localExists(static_cast<uint8_t *>(localExists_))
        , elementSize(elementSize_)
        , elementCount(elementCount_)
        , localResult(false)
        , globalResult(false)
    {}

    const uint8_t * const localData;
    uint8_t * const localExists;
    const uint64_t elementSize;
    const size_t elementCount;

    bool localResult;
    bool globalResult;
};

bool equivalent(const SharemindConsensusDatum * proposals, size_t count) {
    assert(proposals);
    assert(count > 0u);
    static constexpr auto const toId = [](const SharemindConsensusDatum * datum)
            { return *static_cast<SharemindProcessId const *>(datum->data); };
    SharemindProcessId const a = toId(&proposals[0u]);
    for (size_t i = 1u; i < count; i++)
        if (a != toId(&proposals[i]))
            return false;
    return true;
}

SharemindConsensusResultType execute(const SharemindConsensusDatum * proposals,
                                     size_t count,
                                     void * callbackPtr)
{
    assert(proposals);
    assert(count > 0u);
    assert(callbackPtr);

    TransactionData & transaction =
            *static_cast<TransactionData *>(callbackPtr);

    // local elements
    for (size_t i = 0u; i < transaction.elementCount; ++i) {
        bool existsInAll = true;

        // proposals
        for (size_t j = 0u; j < count; ++j) {
            if ((proposals[j].size - sizeof(SharemindProcessId)) % transaction.elementSize != 0u)
                return transaction.localResult = false;

            const size_t elements = (proposals[j].size - sizeof(SharemindProcessId)) /
                                    transaction.elementSize;
            const uint8_t * const data = static_cast<const uint8_t *>(proposals[j].data) +
                                         sizeof(SharemindProcessId);

            bool existsInCurrent = false;

            // proposal elements
            for (size_t k = 0u; k < elements; ++k) {
                if (memcmp(transaction.localData + i * transaction.elementSize,
                           data + k * transaction.elementSize,
                           transaction.elementSize) == 0)
                {
                    existsInCurrent = true;
                    break;
                }
            }

            if (!existsInCurrent) {
                existsInAll = false;
                break;
            }
        }

        transaction.localExists[i] = existsInAll;
    }

    return transaction.localResult = true;
}

void commit(const SharemindConsensusDatum * proposals,
            size_t count,
            const SharemindConsensusResultType * results,
            void * callbackPtr)
{
    assert(proposals);
    assert(count > 0u);
    assert(results);
    assert(callbackPtr);

    // Get the global result from all of the local results
    bool success = true;
    for (size_t i = 0u; i < count; ++i) {
        if (!static_cast<bool>(results[i])) {
            success = false;
            break;
        }
    }

    TransactionData & transaction =
            *static_cast<TransactionData *>(callbackPtr);
    transaction.globalResult = success;
}


/*
 * Mandatory argument: uint64 the element size in V
 * Mandatory cref parameter: public vector V
 * Mandatory ref parameter: result uint8 vector of same length as V
 * No return value.
 */
SHAREMIND_MODULE_API_0x1_SYSCALL(intersection,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    assert(c);
    assert(c->moduleHandle);
    assert(c->process_internal);

    if (num_args != 1u || !crefs || !refs || returnValue)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    assert(crefs[0u].pData);
    assert(refs[0u].pData);

    const size_t inDataSize = crefs[0u].size - 1u;
    const size_t outDataSize = refs[0u].size - 1u;
    const uint64_t elementSize = args[0u].uint64[0u];

    if (inDataSize % elementSize != 0u || inDataSize / elementSize != outDataSize)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    SharemindConsensusFacility & consensusFacility
            = static_cast<ModuleData *>(c->moduleHandle)->consensusFacility;

    typedef SharemindProcessFacility CPF;
    const CPF & processFacility = *static_cast<const CPF *>(c->process_internal);


    const size_t proposalSize = sizeof(SharemindProcessId) + inDataSize;
    uint8_t proposal[proposalSize];

    const SharemindProcessId pid = processFacility.get_process_id(&processFacility);
    memcpy(proposal, &pid, sizeof(SharemindProcessId));
    memcpy(proposal + sizeof(SharemindProcessId), crefs[0u].pData, inDataSize);

    TransactionData transaction(crefs[0u].pData,
                                refs[0u].pData,
                                elementSize,
                                outDataSize);
    SharemindConsensusFacilityError ret
            = consensusFacility.blocking_propose(&consensusFacility,
                                                 "AlgIntersectionOp",
                                                 proposalSize,
                                                 proposal,
                                                 &transaction);

    if (ret == SHAREMIND_CONSENSUS_FACILITY_OK && transaction.globalResult) {
        return SHAREMIND_MODULE_API_0x1_OK;
    } else if (ret == SHAREMIND_CONSENSUS_FACILITY_OUT_OF_MEMORY) {
        return SHAREMIND_MODULE_API_0x1_OUT_OF_MEMORY;
    } else {
        return SHAREMIND_MODULE_API_0x1_GENERAL_ERROR;
    }
}

