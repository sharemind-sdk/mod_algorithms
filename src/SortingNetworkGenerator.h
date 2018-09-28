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

#ifndef SHAREMIND_MOD_ALGORITHMS_SORTINGNETWORKGENERATOR_H
#define SHAREMIND_MOD_ALGORITHMS_SORTINGNETWORKGENERATOR_H

#include <cassert>
#include <map>
#include <memory>
#include <mutex>
#include <sharemind/DebugOnly.h>
#include <sharemind/libsortnetwork/network.h>
#include <sharemind/MakeUnique.h>
#include <utility>


class __attribute__ ((visibility("internal"))) SortingNetworkGenerator {

private: /* Types: */

    class SortingNetwork {

    public: /* Methods: */

        /**
          \brief Generates the sorting network.
          \note This takes significant time for larger inputs.
          \bug There's no way to tell between invalid argument to
               sn_network_create_bitonic_mergesort and out of memory errors.
        */
        SortingNetwork(const size_t elements)
            : m_sortingNetwork(sn_network_create_bitonic_mergesort(elements)) {}

        ~SortingNetwork() noexcept {
            // Destroy the sorting network
            if (m_sortingNetwork)
                sn_network_destroy(m_sortingNetwork);
        }

        /**
          \bug There's no way to tell between invalid argument to
               sn_network_create_bitonic_mergesort and out of memory errors.
        */
        inline bool isValid() const noexcept
        { return m_sortingNetwork != nullptr; }

        inline size_t getNumberOfStages() const noexcept
        { return SN_NETWORK_STAGE_NUM(m_sortingNetwork); }

        inline size_t getNumberOfPairsInStage(const size_t stage) const noexcept
        { return SN_STAGE_COMP_NUM(getStage(stage)); }

        /*
         * For efficiency, it does not make sense to wrap the internal data
         * structures of the sorting network so we expose them here.
         */
        inline sn_stage_s * getStage(const size_t stage) const noexcept
        { return SN_NETWORK_STAGE_GET(m_sortingNetwork, stage); }

        inline size_t serializedSize() const noexcept {
            if (m_serializationSize != 0u)
                return m_serializationSize;
            // First, we would store the number of stages as a single number
            m_serializationSize = 1u;
            // Second, we store each stage separately
            for (size_t s = 0u; s < getNumberOfStages(); s++) {
                // For each stage, we store its size as a single number
                m_serializationSize++;
                // Then we store all the first indices in the pairs
                // and then the second ones
                m_serializationSize += 4u * getNumberOfPairsInStage(s);
            }
            return m_serializationSize;
        }

        inline void serialize(uint64_t * const ptr) const noexcept {
            size_t offset = 0u;

            // First, we store the number of stages
            static_assert(std::numeric_limits<size_t>::max()
                          == std::numeric_limits<uint64_t>::max(),
                          "Platform not supported!");
            ptr[offset++] = getNumberOfStages();
            // Second, we store each stage separately
            for (size_t s = 0u; s < getNumberOfStages(); s++) {
                // For each stage, we store its size as a single number
                const size_t pairsInStage = getNumberOfPairsInStage(s);
                ptr[offset++] = pairsInStage;

                // Extract the stage
                sn_stage_t * const stage = getStage(s);
                sn_comparator_t * comp = nullptr;

                // First we store the left sides of the pairs
                for (size_t p = 0u; p < pairsInStage; p++) {
                    comp = stage->comparators + p;
                    ptr[offset++] = SN_COMP_LEFT(comp);
                }

                // Then, we store the right sides of the pairs
                for (size_t p = 0u; p < pairsInStage; p++) {
                    comp = stage->comparators + p;
                    ptr[offset++] = SN_COMP_RIGHT(comp);
                }

                // We also store the target positions. Minima first.
                for (size_t p = 0u; p < pairsInStage; p++) {
                    comp = stage->comparators + p;
                    ptr[offset++] = SN_COMP_MIN(comp);
                }

                // Maxima second.
                for (size_t p = 0u; p < pairsInStage; p++) {
                    comp = stage->comparators + p;
                    ptr[offset++] = SN_COMP_MAX(comp);
                }
            }
        }

    private: /* Fields: */

        sn_network_t * const m_sortingNetwork;
        mutable size_t m_serializationSize = 0u;

    }; /* class SortingNetwork { */

public: /* Methods: */

    inline SortingNetwork * getCachedOrGenerateAndCacheNetwork(
                const size_t elements)
    {
        std::lock_guard<std::mutex> lock(m_sortingNetworkCacheMutex);
        auto it(m_sortingNetworkCache.find(elements));
        if (it != m_sortingNetworkCache.cend())
            return it->second.get();
        auto sn(sharemind::makeUnique<SortingNetwork>(elements));
        if (!sn->isValid())
            return nullptr;
        auto r(sn.get());
        SHAREMIND_DEBUG_ONLY(auto const rv =)
                m_sortingNetworkCache.emplace(elements, std::move(sn));
        assert(rv.second);
        return r;
    }

private: /* Fields: */

    mutable std::mutex m_sortingNetworkCacheMutex;
    std::map<size_t, std::unique_ptr<SortingNetwork> > m_sortingNetworkCache;

}; /* class SortingNetworkGenerator {*/

#endif /* SHAREMIND_MOD_ALGORITHMS_SORTINGNETWORKGENERATOR_H */
