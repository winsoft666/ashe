/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2020~2024 winsoft666 <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef ASHE_RAND_UTIL_H_
#define ASHE_RAND_UTIL_H_
#pragma once

#include "ashe/config.h"
#include <string>
#include <vector>
#include <cstdint>
#include <algorithm>

namespace ashe {
// Returns a random number in range [0, UINT64_MAX]. Thread-safe.
ASHE_API uint64_t RandUint64();

// Returns a random number between min and max (inclusive). Thread-safe.
ASHE_API int RandInt(int min, int max);

// Returns a random number in range [0, range).  Thread-safe.
ASHE_API uint64_t RandGenerator(uint64_t range);

// Returns a random double in range [0, 1). Thread-safe.
ASHE_API double RandDouble();

// Returns a random float in range [0, 1). Thread-safe.
ASHE_API float RandFloat();

// Given input |bits|, convert with maximum precision to a double in the range [0, 1). Thread-safe.
ASHE_API double BitsToOpenEndedUnitInterval(uint64_t bits);

// Given input `bits`, convert with maximum precision to a float in the range [0, 1). Thread-safe.
ASHE_API float BitsToOpenEndedUnitIntervalF(uint64_t bits);

ASHE_API void RandBytes(void* output, size_t output_length);

ASHE_API std::vector<uint8_t> RandBytesAsVector(size_t length);

ASHE_API std::string RandBytesAsString(size_t length);

// An STL UniformRandomBitGenerator backed by RandUint64.
class RandomBitGenerator {
   public:
    using result_type = uint64_t;
    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return UINT64_MAX; }
    result_type operator()() const { return RandUint64(); }

    RandomBitGenerator() = default;
    ~RandomBitGenerator() = default;
};


// Shuffles [first, last) randomly. Thread-safe.
template <typename Itr>
void RandomShuffle(Itr first, Itr last) {
    std::shuffle(first, last, RandomBitGenerator());
}
}  // namespace ashe

#endif
