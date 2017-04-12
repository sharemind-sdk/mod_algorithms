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

#ifndef SHAREMIND_MOD_ALGORITHMS_TOPKSORTINGNETWORKGENERATOR_H
#define SHAREMIND_MOD_ALGORITHMS_TOPKSORTINGNETWORKGENERATOR_H

#include <cassert>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>


class __attribute__ ((visibility("internal"))) TopKSortingNetworkGenerator {

public: /* Types: */

    typedef std::vector<std::pair<size_t, size_t> > Stage;
    class Network: public std::vector<Stage> {

    public: /* Methods: */

        inline size_t serializedSize() const noexcept {
            if (m_serializationSize != 0u)
                return m_serializationSize;

            // First, we would store the number of stages as a single number
            m_serializationSize = 1u;
            // Second, we store each stage separately
            for (size_t s = 0u; s < size(); s++) {
                // For each stage, we store its size as a single number
                m_serializationSize++;
                // Then we store all the indices of the pairs
                m_serializationSize += 2u * (*this)[s].size();
            }
            return m_serializationSize;
        }

        void serialize(uint64_t * const ptr) const noexcept {
            assert(ptr);
            size_t offset = 0u;

            // First, we store the number of stages
            ptr[offset++] = static_cast<uint64_t>(size());
            // Second, we store each stage separately
            for (size_t s = 0u; s < size(); s++) {
                // For each stage, we store its size as a single number
                ptr[offset++] = static_cast<uint64_t>((*this)[s].size());

                // Store left and right indices
                for (size_t p = 0u; p < (*this)[s].size(); p++) {
                    ptr[offset++] = (*this)[s].at(p).first;
                    ptr[offset++] = (*this)[s].at(p).second;
                }
            }
        }

    private: /* Fields: */

        mutable size_t m_serializationSize = 0u;

    };

public: /* Methods: */

    Network * getCachedOrGenerateAndCacheNetwork(const uint64_t elements,
                                                 const uint64_t k);

private: /* Fields: */

    mutable std::mutex m_cacheMutex;
    std::map<std::pair<uint64_t, uint64_t>, std::unique_ptr<Network> > m_cache;

}; /* class TopKSortingNetworkGenerator { */

#endif /* SHAREMIND_MOD_ALGORITHMS_TOPKSORTINGNETWORKGENERATOR_H */
