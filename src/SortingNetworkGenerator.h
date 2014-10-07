/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_MOD_ALGORITHMS_SORTINGNETWORKGENERATOR_H
#define SHAREMIND_MOD_ALGORITHMS_SORTINGNETWORKGENERATOR_H

#include <mutex>
#include <sharemind/ScopedObjectMap.h>
extern "C" {
#include <sn_network.h> // libsortnetwork
}
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

    typedef sharemind::ScopedObjectMap<size_t, SortingNetwork> Cache;

public: /* Methods: */

    inline SortingNetwork * getCachedOrGenerateAndCacheNetwork(
                const size_t elements)
    {
        std::lock_guard<std::mutex> lock(m_sortingNetworkCacheMutex);
        if (m_sortingNetworkCache.find(elements) != m_sortingNetworkCache.end())
            return m_sortingNetworkCache.find(elements)->second;

        // Generate the sorting network
        SortingNetwork * const sn = new SortingNetwork(elements);

        try {
            // Validate it
            if (!sn->isValid()) {
                delete sn;
                return nullptr;
            }

            // Cache the network and initialize the usage count
            m_sortingNetworkCache.insert(Cache::value_type(elements, sn));
        } catch (...) {
            delete sn;
            throw;
        }
        return sn;
    }

private: /* Fields: */

    mutable std::mutex m_sortingNetworkCacheMutex;
    Cache m_sortingNetworkCache;

}; /* class SortingNetworkGenerator {*/

#endif /* SHAREMIND_MOD_ALGORITHMS_SORTINGNETWORKGENERATOR_H */
