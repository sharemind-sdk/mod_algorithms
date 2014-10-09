/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_MOD_ALGORITHMS_BLOCKSORTPERMUTATION_H
#define SHAREMIND_MOD_ALGORITHMS_BLOCKSORTPERMUTATION_H

#include <algorithm>
#include <array>

template <typename T, size_t N>
SHAREMIND_MODULE_API_0x1_SYSCALL(blockSortPermutation,
                                 args, num_args, refs, crefs,
                                 returnValue, c)
{
    (void) c;
    (void) args;

    if (num_args != 0u || returnValue || !crefs || !refs)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    assert(crefs[0u].pData);
    assert(refs[0u].pData);
    assert(crefs[0u].size > 0u);
    assert(refs[0u].size > 0u);

    typedef std::array<T, N> Block;
    static_assert(sizeof(Block) == sizeof(T) * N,
                  "Block type size differs from the assumed type size");

    if (crefs[0u].size != sizeof(T) &&
            (crefs[0u].size - 1u) % sizeof(Block) != 0)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    const size_t dataSize =
        crefs[0u].size == sizeof(T) ?
        sizeof(T) : crefs[0u].size - 1u;

    if (refs[0u].size != sizeof(uint64_t) &&
            (refs[0u].size - 1u) % sizeof(uint64_t) != 0)
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    const size_t indexSize =
        refs[0u].size == sizeof(uint64_t) ?
        sizeof(uint64_t) : refs[0u].size - 1u;

    if (dataSize / sizeof(Block) != indexSize / sizeof(uint64_t))
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;

    uint64_t * const index = static_cast<uint64_t *>(refs[0u].pData);
    if (std::any_of(index, index + indexSize / sizeof(uint64_t),
                [dataSize] (const uint64_t idx) {
                    return idx >= dataSize;
                }))
    {
        return SHAREMIND_MODULE_API_0x1_INVALID_CALL;
    }

    const Block * const data = static_cast<const Block *>(crefs[0u].pData);
    std::stable_sort(index, index + indexSize / sizeof(uint64_t),
            [&data] (const uint64_t a, const uint64_t b) {
                return data[a] < data[b];
            });

    return SHAREMIND_MODULE_API_0x1_OK;
}

#endif // SHAREMIND_MOD_ALGORITHMS_BLOCKSORTPERMUTATION_H
