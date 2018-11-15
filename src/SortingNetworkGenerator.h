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
#include <sharemind/libsortnetwork/Network.h>
#include <utility>


class __attribute__ ((visibility("internal"))) SortingNetworkGenerator {

private: /* Types: */

    using Network = sharemind::SortingNetwork::Network;
    using Stage = sharemind::SortingNetwork::Stage;

    class SortingNetwork: private Network {

    public: /* Methods: */

        /**
          \brief Generates the sorting network.
          \note This takes significant time for larger inputs.
        */
        SortingNetwork(std::size_t numInputs)
            : Network(Network::makeBitonicMergeSort(numInputs))
            , m_serializationSize(
                [this]() {
                    // Start with the number of stages as a single number:
                    std::size_t r = 1u;
                    // Second, we store each stage separately:
                    for (auto const & stage : stages()) {
                        // For each stage, we store its size as a single number:
                        ++r;
                        /* Then we store all the first indices in the pairs and
                           then the second ones: */
                        r += 4u * stage.numComparators();
                    }
                    return r;
                }())
        {}

        std::size_t serializedSize() const noexcept
        { return m_serializationSize; }

        void serialize(std::uint64_t * ptr) const noexcept {
            // First, we store the number of stages
            static_assert(std::numeric_limits<std::size_t>::max()
                          == std::numeric_limits<std::uint64_t>::max(),
                          "Platform not supported!");
            (*ptr) = numStages();
            // Second, we store each stage separately
            for (auto const & stage : stages()) {
                // For each stage, we store its size as a single number
                (*++ptr) = stage.numComparators();

                // First we store the left sides of the pairs
                for (auto const & comp : stage.comparators())
                    (*++ptr) = comp.left();

                // Then, we store the right sides of the pairs
                for (auto const & comp : stage.comparators())
                    (*++ptr) = comp.right();

                // We also store the target positions. Minima first.
                for (auto const & comp : stage.comparators())
                    (*++ptr) = comp.min();

                // Maxima second.
                for (auto const & comp : stage.comparators())
                    (*++ptr) = comp.max();
            }
        }

    private: /* Fields: */

        std::size_t const m_serializationSize = 0u;

    }; /* class SortingNetwork { */

public: /* Methods: */

    SortingNetwork * getCachedOrGenerateAndCacheNetwork(
                std::size_t const numInputs)
    {
        std::lock_guard<std::mutex> lock(m_sortingNetworkCacheMutex);
        auto it(m_sortingNetworkCache.find(numInputs));
        if (it != m_sortingNetworkCache.cend())
            return it->second.get();
        auto sn(std::make_unique<SortingNetwork>(numInputs));
        auto r(sn.get());
        SHAREMIND_DEBUG_ONLY(auto const rv =)
                m_sortingNetworkCache.emplace(numInputs, std::move(sn));
        assert(rv.second);
        return r;
    }

private: /* Fields: */

    mutable std::mutex m_sortingNetworkCacheMutex;
    std::map<std::size_t, std::unique_ptr<SortingNetwork> >
            m_sortingNetworkCache;

}; /* class SortingNetworkGenerator {*/

#endif /* SHAREMIND_MOD_ALGORITHMS_SORTINGNETWORKGENERATOR_H */
