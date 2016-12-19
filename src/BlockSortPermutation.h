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

    if (num_args != 1u || returnValue || !crefs || !refs)
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

    const bool ascending = static_cast<bool>(args[0].uint8[0]);

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
    uint64_t * const start = index;
    uint64_t * const end = index + indexSize / sizeof(uint64_t);
    if (ascending) {
        std::stable_sort(start, end,
                         [&data] (const uint64_t a, const uint64_t b) {
                             return data[a] < data[b];
                         });
    } else {
        std::stable_sort(start, end,
                         [&data] (const uint64_t a, const uint64_t b) {
                             return data[a] > data[b];
                         });
    }

    return SHAREMIND_MODULE_API_0x1_OK;
}

#endif // SHAREMIND_MOD_ALGORITHMS_BLOCKSORTPERMUTATION_H
