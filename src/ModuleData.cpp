/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include "ModuleData.h"
#include "Intersection.h"

SharemindOperationType const intersectionOperation = {
    &equivalent,
    &execute,
    &commit,
    "AlgIntersectionOp"
};

ModuleData::ModuleData(SharemindConsensusFacility & cf)
    : consensusFacility(cf)
{
    consensusFacility.add_operation_type(&consensusFacility,
                                         &intersectionOperation);
}
