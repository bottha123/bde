// bdlb_pcgrandomgenerator.h                                                         -*-C++-*-
#ifndef INCLUDED_BDLB_PCGRANDOMGENERATOR
#define INCLUDED_BDLB_PCGRANDOMGENERATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class to generate random numbers using the PCG algorithm.
//
//@CLASSES:
//  bdlb::PcgRandomGenerator: random number generator
//
//@SEE_ALSO: bdlb_random
//
//@DESCRIPTION: This component provides a single class,
// 'bdlb::PcgRandomGenerator', that is used to generate random numbers
// employing the PCG algorithm, a high-performance, high-quality RNG.  PCG
// stands for "permuted congruential generator" (see http://www.pcg-random.org
// for details).  The PCG technique employs the concepts of permutation
// functions on tuples and a base linear congruential generator.  The PCG
// algorithm is seeded with two values: its initial state and a "stream
// selector."  Stream selector is intended to address a potential hazard of
// multiple instances of a random number generator having unintended
// correlation between their outputs.  For example, if we allow them to have
// the same internal state (e.g.  mistakenly seeding them with the current time
// in seconds), they will output the exact same sequence of numbers.  Employing
// a 'streamSelector' enables the the same 'initState' to generate unique
// sequences.  Please refer to O’Neill (2014) at
// https://www.pcg-random.org/pdf/hmc-cs-2014-0905.pdf for details.
//
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Simulating a Pair of Dice
/// - - - - - - - - - - - - - - - - - -
// This example shows how one might use 'bdlb::PcgRandomGenerator' to create 
// and use a class to simulate the roll of a single die in a game, such as 
// craps, that uses dice.
//
// First, we define the 'Die' class itself:
//..
//                     // =========
//                     // class Die
//                     // =========
//
//  class Die {
//
//      // DATA
//      bdlb::PcgRandomGenerator d_pcg; // used to generate next role of this die
//
//    public:
//      // CREATORS
//      Die(bsl::uint64_t initialState, bsl::uint64_t streamSelector);
//          // Create an object used to simulate a single die, using the
//          // specified 'initialState' and 'streamSelector'.
//
//      // MANIPULATORS
//      int roll();
//          // Return the next pseudo-random value in the range '[1 .. 6]',
//          // based on the sequence of values established by the 'initialState'
//          // and 'streamSelector' values supplied at construction.
//  };
//
//                     // ---------
//                     // class Die
//                     // ---------
//
//  // CREATORS
//  inline
//  Die::Die(bsl::uint64_t initialState, bsl::uint64_t streamSelector)
//  : d_pcg(initialState, streamSelector)
//  {
//  }
//
// MANIPULATORS
// int Die::roll()
// {
//     int result;
//
//     do {
//         result = d_pcg.generate() & 7;
//     } while (result > 5);
//
//     return result + 1;
// }
//..
// Now, we can use our 'Dice' class to get the random numbers needed to
// simulate a game of craps.  Note that the game of craps requires two dice.
//
// We can instantiate a single 'Die' and role it twice,
//..
// void rollOneDieTwice()
// {
//     Die a(123, 456);
//
//     int d1 = a.roll();
//     int d2 = a.roll();
//
//     cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 5
// }
//..
// Alternatively, we could create two instances of 'Die', with separate initial
// states/sequences, and role each one once:
//..
// void rollTwoDice()
// {
//     Die a(123, 123);
//     Die b(456, 456);
//
//     int d1 = a.roll();
//     int d2 = b.roll();
//
//     cout << "d1 = " << d1 << ", d2 = " << d2 << endl;  // d1 = 3, d2 = 1
// }
//..
// Note that the specification of separate seeds is important to produce a
// proper distribution for our game.  If we had shared the seed value each die
// would always produce the same sequence of values as the other.
//..
// void shareStateAndSequence()
// {
//     Die a(123, 456);  // BAD IDEA
//     Die b(123, 456);  // BAD IDEA
//
//     int d1 = a.roll();
//     int d2 = b.roll();
//     assert(d2 == d1);
// }
//..
///Example 2: Using stream selector to guarantee uncorrelated sequences
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example illustrates how the stream selector can be used to guarantee
// that two distinct instances of 'PCGRandomGenerator' produce uncorrelated
// sequences.
//
// First, we use the 'RandomDevice' to choose the initial states for the
// generators using a source of true randomness:
//..
//  uint64_t state1;
//  int rc = bdlb::RandomDevice::getRandomBytes(
//      reinterpret_cast<unsigned char *>(&state1), sizeof(state1));
//  (void)rc;  // error handling omitted
//
//  uint64_t state2;
//  int rc = bdlb::RandomDevice::getRandomBytes(
//      reinterpret_cast<unsigned char *>(&state2), sizeof(state2));
//  (void)rc;  // error handling omitted
//..
// Then we select two distinct stream selectors for the generators, which, due
// to the PCG algorithmic properties will guarantee that the sequences will be
// uncorrelated even if the initial state is exactly the same:
//..
//  const uint64_t streamSelector1 = 1;
//  const uint64_t streamSelector2 = 2;
//..
// Finally, we initialize the generators with their respective initial states
// and stream selectors and check that they produce distinct sequences of
// random numbers.  The check is guaranteed to pass even in the rare, but
// possible case of 'state1 == state2'.
//..
//  bdlb::PcgRandomGenerator rng1(state1, streamSelector1);
//  bdlb::PcgRandomGenerator rng2(state2, streamSelector2);
//
//  const int NUM_VALUES = 1000;
//  bsl::vector<bsl::uint32_t> sequence1(NUM_VALUES);
//  bsl::vector<bsl::uint32_t> sequence2(NUM_VALUES);
//
//  for (int i = 0; i < NUM_VALUES; ++i) {
//      sequence1[i] = rng1.generate();
//      sequence2[i] = rng2.generate();
//  }
//
//  assert(sequence1 != sequence2);
//..

#include <bsl_cstdint.h>

namespace BloombergLP {
namespace bdlb {

                                 // ========================
                                 // class PcgRandomGenerator
                                 // ========================
class PcgRandomGenerator {
    // This class implements a random number generator (RNG) based on the PCG
    // algorithm.  

  private:
    // DATA
    bsl::uint64_t d_state;           // the RNG state

    bsl::uint64_t d_streamSelector;  // selected sequence (stream)

    // FRIENDS
    friend bool operator==(const PcgRandomGenerator& lhs,
                           const PcgRandomGenerator& rhs);

  public:
    // CREATORS
    PcgRandomGenerator(bsl::uint64_t initState      = 0,
                       bsl::uint64_t streamSelector = 1);
        // Create a 'PcgRandomGenerator' object and seed it with the optionally
        // specified 'initState' and 'streamSelector'.  If 'initState' is not
        // specified, 0 is used as the initial state.  If 'streamSelector' is
        // not specified, 1 is used as the stream selector. Only the low 63
        // bits of the 'streamSelector' are significant.  Note that invoking
        // different instances with the identical 'initState' and
        // 'streamSelector' will result in the same sequence of random numbers
        // from subsequent invocations of 'generate()'.

    //! PcgRandomGenerator(const PcgRandomGenerator& original) = default;
        // Create a 'PcgRandomGenerator' object having the same state as the
        // specified 'original' object.  Note that this newly created object
        // will generate the same sequence of numbers as the 'original' object.

    // MANIPULATORS
    //! PcgRandomGenerator& operator=(const PcgRandomGenerator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.  Note that this newly
        // created object will generate the same sequence of numbers as the
        // 'original' object.

    void seed(bsl::uint64_t initState, bsl::uint64_t streamSelector);
        // Re-seed the RNG with the specified new 'initState' and
        // 'streamSelector'.

    bsl::uint32_t generate();
        // Return the next random number in the sequence.
};

// FREE OPERATORS
bool operator==(const PcgRandomGenerator& lhs, const PcgRandomGenerator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'PcgRandomGenerator' objects have the
    // same value if both of the corresponding values of their 'd_state' and
    // 'd_streamSelector' attributes are the same.

bool operator!=(const PcgRandomGenerator& lhs, const PcgRandomGenerator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'PcgRandomGenerator' objects do
    // not have the same value if either of the corresponding values of their
    // 'd_state' or 'd_streamSelector' attributes are not the same.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // ------------------------
                          // class PcgRandomGenerator
                          // ------------------------

// CREATORS
inline
PcgRandomGenerator::PcgRandomGenerator(bsl::uint64_t initState,
                                       bsl::uint64_t streamSelector)
{
    seed(initState, streamSelector);
}

// MANIPULATORS
inline
void PcgRandomGenerator::seed(bsl::uint64_t initState,
                              bsl::uint64_t streamSelector)
{
    d_state          = 0U;
    d_streamSelector = (streamSelector << 1u) | 1u;
    generate();
    d_state += initState;
    generate();
}

inline
bsl::uint32_t PcgRandomGenerator::generate()
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
bool bdlb::operator==(const PcgRandomGenerator& lhs,
                      const PcgRandomGenerator& rhs)
{
    return lhs.d_state == rhs.d_state &&
           lhs.d_streamSelector == rhs.d_streamSelector;
}

inline
bool bdlb::operator!=(const PcgRandomGenerator& lhs,
                      const PcgRandomGenerator& rhs)
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
