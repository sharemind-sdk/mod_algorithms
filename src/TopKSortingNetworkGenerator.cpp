/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include "TopKSortingNetworkGenerator.h"

#include <algorithm>
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
Network * constructPartialSwissSortingNetwork(const uint64_t elements,
                                              const uint64_t k)
{
    Network * const stages = new Network();
    const uint64_t n = logBase2(elements);
    try {
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
    } catch (...) {
        delete stages;
        throw;
    }

    return stages;
}

} /* namespace anonymous { */


Network * TopKSortingNetworkGenerator::getCachedOrGenerateAndCacheNetwork(
        const uint64_t elements,
        const uint64_t k)
{
    const std::pair<uint64_t, uint64_t> key(elements, k);

    std::lock_guard<std::mutex> lock(m_cacheMutex);
    const Cache::const_iterator it = m_cache.find(key);
    if (it != m_cache.end())
        return it->second;

    // Generate the sorting network
    Network * const sn = constructPartialSwissSortingNetwork(elements, k);
    try {
        // Cache the network and initialize the usage count
        m_cache.insert(Cache::value_type(key, sn));
    } catch (...) {
        delete sn;
        throw;
    }
    return sn;
}
