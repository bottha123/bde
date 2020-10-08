// bdlb_pcgrandomgenerator.h                                                         -*-C++-*-
#ifndef INCLUDED_BDLB_PCGRANDOMGENERATOR
#define INCLUDED_BDLB_PCGRANDOMGENERATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class to generate random numbers using the PCG algorithm.
//
//@CLASSES:
//  bdlb::PCGRandomGenerator: random number generator
//
//@SEE_ALSO: bdlb_random
//
//@DESCRIPTION: This component provides a single class,
// 'bdlb::PCGRandomGenerator', that is used to generate random numbers
// employing the PCG algorithm, a high-performance, high-quality RNG.  The PCG
// technique employs the concepts of permutation functions on tuples and a base
// linear congruential generator.
//
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Generating random 32-bit numbers
///- - - - - - - - - - - - - - - - - - - - - -
// The PCGRandomGenerator constructor takes two 64-bit constants (the initial
// state, and the rng sequence selector.  Instances of PCGRandomGenerator with
// different sequence selectors will never have random sequences that coincide.
// In order to obtain nondeterministic output for each instance, on a Unix
// system random bytes can be obtained from /dev/urandom.  The component 
// bdlb_randomdevice provides such functionality.  Also illustrated below is a 
// a backup mechanism for obtaining random bytes.
//
//  uint64_t state;
//  if (0 != RandomDevice::getRandomBytes((unsigned char *)&state,
//                                                         sizeof(state))) {
//          state = time(NULL) ^ (intptr_t)&bsl::printf;  // fallback state
//  }
//  uint64_t streamSelector;
//  if (0 != RandomDevice::getRandomBytes((unsigned char *)&streamSelector,
//                                              sizeof(streamSelector))) {
//           streamSelector = time(NULL) ^
//                   (intptr_t)&bsl::printf;  // fallback streamSelector
//  }
//  bdlb::PCGRandomGenerator  rng(state, streamSelector);
//  bsl::uint32_t randomInt = rng.generate();
//..

#include <bsl_cstdint.h>

namespace BloombergLP {
namespace bdlb {

                                 // =========
                                 // class PCGRandomGenerator
                                 // =========
class PCGRandomGenerator {
    // This mechanism class implements a random number generator (RNG) based on
    // the PCG algorithm.  The PCG stands for "permuted congruential
    // generator." The state is 64 bits. It uses a so-called stream selector,
    // also 64 bits.  'initState' is the starting state for the RNG.  Any
    // 64-bit value may be passed. 'streamSelector' selects the output sequence
    // for the RNG.  Any 64-bit value may be passed, although only the low 63
    // bits are significant.  There are 2^63 different RNGs available, and
    // 'streamSelector' selects from among them.  Invoking different instances
    // with the identical 'initState' and 'streamSelector' will result in the
    // same sequence of random numbers from subsequent invocations of
    // getRandom().  For details of the algorithm,see
    // http://www.pcg-random.org.

  private:
    // DATA
    bsl::uint64_t d_state;           // the RNG state

    bsl::uint64_t d_streamSelector;  // selected sequence

    // FRIENDS
    friend bool operator==(const PCGRandomGenerator& lhs, const PCGRandomGenerator& rhs);

  public:
    // CREATORS
    PCGRandomGenerator(bsl::uint64_t initState = 0, bsl::uint64_t streamSelector = 1);
        // Create a 'PCGRandomGenerator' object and seed it with the optionally specified
        // 'initState' and 'streamSelector.'  If 'initState' is not specified,
        // 0 is used as the initial state.  If 'streamSelector' is not
        // specified, 1 is used as the initial state.

    //! PCGRandomGenerator(const PCGRandomGenerator& original) = default;
        // Create a 'PCGRandomGenerator' object having the same state as the specified
        // 'original' object. Note that this newly created object will generate
        // the same sequence of numbers as the 'original' object.

    // MANIPULATORS
    //! PCGRandomGenerator& operator=(const PCGRandomGenerator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object. Note that this newly
        // created object will generate the same sequence of numbers as the
        // 'original' object.

    void seed(bsl::uint64_t initState, bsl::uint64_t streamSelector);
        // Re-seed the RNG with the specified new 'initState' and
        // 'streamSelector'.

    bsl::uint32_t generate();
        // Return the next random number in the sequence.
};

// FREE OPERATORS
bool operator==(const PCGRandomGenerator& lhs, const PCGRandomGenerator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'PCGRandomGenerator' objects have the same value if
    // both of the corresponding values of their 'd_state' and
    // 'd_streamSelector' attributes are the same.

bool operator!=(const PCGRandomGenerator& lhs, const PCGRandomGenerator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'PCGRandomGenerator' objects do not have the
    // same value if either of the corresponding values of their 'd_state' or
    // 'd_streamSelector' attributes are not the same.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                                 // ---------
                                 // class PCGRandomGenerator
                                 // ---------

// CREATORS
inline
PCGRandomGenerator::PCGRandomGenerator(bsl::uint64_t initState,
                                       bsl::uint64_t streamSelector)
{
    seed(initState, streamSelector);
}

// MANIPULATORS
inline
void PCGRandomGenerator::seed(bsl::uint64_t initState,
                              bsl::uint64_t streamSelector)
{
    d_state          = 0U;
    d_streamSelector = (streamSelector << 1u) | 1u;
    generate();
    d_state += initState;
    generate();
}

inline
bsl::uint32_t PCGRandomGenerator::generate()
{
    bsl::uint64_t oldstate = d_state;
    d_state = oldstate * 6364136223846793005ULL + d_streamSelector;
    bsl::uint32_t xorshifted =
        static_cast<bsl::uint32_t>(((oldstate >> 18u) ^ oldstate) >> 27u);
    bsl::uint32_t rot = static_cast<bsl::uint32_t>(oldstate >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

}  // namespace bdlb

// FREE OPERATORS
inline
bool bdlb::operator==(const PCGRandomGenerator& lhs,
                      const PCGRandomGenerator& rhs)
{
    return lhs.d_state == rhs.d_state &&
           lhs.d_streamSelector == rhs.d_streamSelector;
}

inline
bool bdlb::operator!=(const PCGRandomGenerator& lhs,
                      const PCGRandomGenerator& rhs)
{
    return !(lhs == rhs);
}

}  // namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------