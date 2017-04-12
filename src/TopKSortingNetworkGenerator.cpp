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

#include "TopKSortingNetworkGenerator.h"

#include <algorithm>
#include <sharemind/DebugOnly.h>
#include <sharemind/MakeUnique.h>
#include <utility>


namespace /* anonymous */ {

typedef TopKSortingNetworkGenerator::Network Network;
typedef TopKSortingNetworkGenerator::Stage Stage;

inline uint64_t logBase2(uint64_t n) {
    uint64_t out = 0u;
    while (n >>= 1u)
        ++out;
    return out;
}

class PascalStageIterator
        : public std::iterator<std::input_iterator_tag, uint64_t>
{

private: /* Methods: */

    inline explicit PascalStageIterator(const uint64_t n)
        : m_c(1u)
        , m_n(n)
        , m_k(1u) {}

    inline PascalStageIterator(const uint64_t n, bool)
        : m_c(1u)
        , m_n(n)
        , m_k(n + 2u) {}

public: /* Methods: */

    inline static PascalStageIterator begin(const uint64_t n)
        { return PascalStageIterator(n); }

    inline static PascalStageIterator end(const uint64_t n)
        { return PascalStageIterator(n, true); }

    inline uint64_t operator*() const { return m_c; }
    inline uint64_t operator->() const { return m_c; }

    inline bool operator==(const PascalStageIterator & i) const {
        return m_k == i.m_k;
    }

    inline bool operator!=(const PascalStageIterator & i) const {
        return m_k != i.m_k;
    }

    inline PascalStageIterator & operator++() {
        m_c *= m_n - m_k + 1u;
        m_c /= m_k;
        m_k ++;
        return *this;
    }

    inline PascalStageIterator operator++(int) {
        PascalStageIterator out = *this;
        this->operator++();
        return out;
    }

private: /* Fields: */

    uint64_t m_c;
    const uint64_t m_n;
    uint64_t m_k;

}; /* class PascalStageIterator */

inline PascalStageIterator beginRow(const uint64_t n) {
    return PascalStageIterator::begin(n);
}

inline PascalStageIterator endRow(const uint64_t n) {
    return PascalStageIterator::end(n);
}

// Find the best (for a loose value of best) k from array of length 2^n.
std::unique_ptr<Network> constructPartialSwissSortingNetwork(
        uint64_t const elements,
        uint64_t const k)
{
    auto stages(sharemind::makeUnique<Network>());
    const uint64_t n = logBase2(elements);
    Stage stage;
    std::vector<bool> needToCompute(1 << n);
    std::vector<bool> needToComputeNext(1 << n);

    for (size_t i = 0; i < k; ++ i)
        needToCompute.at(i) = true;

    size_t shift = 0;
    for (size_t r = 1; r <= n; ++ r) {
        stage.clear();
        size_t offset = 0;
        for (PascalStageIterator i = beginRow(n-r), e = endRow(n-r);
             i != e;
             ++ i)
        {
            const size_t half = *i << shift;
            for (size_t j = offset; j < half + offset; ++ j) {
                if (needToCompute[j]) {
                    stage.push_back(std::make_pair(j, j + half));
                    needToComputeNext[j] = true;
                    needToComputeNext[j + half] = true;
                }
            }

            offset += half*2;
        }

        ++shift;
        swap(needToCompute, needToComputeNext);

        stages->push_back(stage);
    }

    reverse(stages->begin(), stages->end());
    return stages;
}

} /* namespace anonymous { */


Network * TopKSortingNetworkGenerator::getCachedOrGenerateAndCacheNetwork(
        const uint64_t elements,
        const uint64_t k)
{
    std::pair<uint64_t, uint64_t> key(elements, k);

    std::lock_guard<std::mutex> lock(m_cacheMutex);
    auto it(m_cache.find(key));
    if (it != m_cache.cend())
        return it->second.get();
    auto sn(constructPartialSwissSortingNetwork(elements, k));
    auto r(sn.get());
    SHAREMIND_DEBUG_ONLY(auto const rv =)
            m_cache.emplace(std::move(key), std::move(sn));
    assert(rv.second);
    return r;
}
