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
#include <sharemind/libsoftfloat/softfloat.h>


template<typename T, size_t N>
struct __attribute__ ((visibility("internal"))) BlockCompare {
    using Block = std::array<T, N>;

    int operator()(const Block& a, const Block& b, bool ascending) {
        if (ascending) {
            return a < b;
        } else {
            return a > b;
        }
    }
};

bool sf_float_gt(const sf_float32& a, const sf_float32& b) {
    sf_fpu_state state = 0;
    return sf_float32_gt(a, b, state).result;
}

bool sf_float_gt(const sf_float64& a, const sf_float64& b) {
    sf_fpu_state state = 0;
    return sf_float64_gt(a, b, state).result;
}

bool sf_float_lt(const sf_float32& a, const sf_float32& b) {
    sf_fpu_state state = 0;
    return sf_float32_lt(a, b, state).result;
}

bool sf_float_lt(const sf_float64& a, const sf_float64& b) {
    sf_fpu_state state = 0;
    return sf_float64_lt(a, b, state).result;
}

template<typename T, size_t N>
struct __attribute__ ((visibility("internal"))) FloatBlockCompare {
    using Block = std::array<T, N>;

    int operator()(const Block& a, const Block& b, bool ascending) {
        for (size_t i = 0; i < N; ++i) {
            if ((ascending && sf_float_lt(a[i], b[i])) ||
                (!ascending && sf_float_gt(a[i], b[i])))
            {
                return true;
            }
        }

        return false;
    }
};

template <typename T, size_t N, class Compare = BlockCompare<T, N> >
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
    Compare compare;
    uint64_t * const start = index;
    uint64_t * const end = index + indexSize / sizeof(uint64_t);

    std::stable_sort(start, end,
                     [&data, &compare, ascending] (const uint64_t a, const uint64_t b) {
                         return compare(data[a], data[b], ascending);
                     });

    return SHAREMIND_MODULE_API_0x1_OK;
}

#endif // SHAREMIND_MOD_ALGORITHMS_BLOCKSORTPERMUTATION_H
