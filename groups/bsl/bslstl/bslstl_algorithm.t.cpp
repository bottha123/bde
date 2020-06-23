// bslstl_algorithm.t.cpp                                             -*-C++-*-
#include <bslstl_algorithm.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_destructorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <functional>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>      // atoi
#include <string.h>      // strlen

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// The component under test provides implementations for algorithms not
// provided by the underlying standard library implementation.
// ----------------------------------------------------------------------------
//
// [ 1] bool all_of (InputIter first, InputIter last, PREDICATE pred);
// [ 1] bool any_of (InputIter first, InputIter last, PREDICATE pred);
// [ 1] bool none_of(InputIter first, InputIter last, PREDICATE pred);
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

// ============================================================================
//                       GLOBAL TEST ALIASES
// ----------------------------------------------------------------------------

using namespace BloombergLP;

// ============================================================================
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

struct IsOdd {
    // A standard compliant C++03 unary predicate functor that returns 'true'
    // if an 'int' value is odd.

    // PUBLIC TYPES
    typedef int  argument_type;
    typedef bool result_type;

    // ACCESSORS
    result_type operator()(argument_type value) const
        // Return 'true' if 'value' is odd, and 'false' otherwise.
    {
        return (value % 2) != 0;
    }
};

bool isEven(int value)
    // A function that (as opposed to a functor) that
    // returns 'true' if 'value' is even, and 'false' otherwise.
{
    return (value % 2) == 0;
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

                verbose = argc > 2;
            veryVerbose = argc > 3;
        veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // FUNCTIONALITY TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 
        //
        // Plan:
        //: 1 Run each method with an empty input range and verify that the
        //:   behavior is as expected.
        //: 2 Run each method with a single-element input range and verify that
        //:   the behavior is as expected.
        //: 3 Run each method with multiple-element input range and verify that
        //:   the behavior is as expected.
        //
        // Testing:
        //   FUNCTIONALITY TEST
        // [ 1] bool all_of (InputIter first, InputIter last, PREDICATE pred);
        // [ 1] bool any_of (InputIter first, InputIter last, PREDICATE pred);
        // [ 1] bool none_of(InputIter first, InputIter last, PREDICATE pred);
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTIONALITY TEST"
                            "\n==============\n");

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        const int a[] = {4};
        const int b[] = {0, 2, 4, 6, 8, 1, 3, 5, 7, 9};
        const int c[] = {1, 3, 5, 7, 9, 0, 2, 4, 6, 8};

        ASSERT( (bsl::all_of (a, a,   IsOdd()) ));
        ASSERT(!(bsl::any_of (a, a,   IsOdd()) ));
        ASSERT( (bsl::none_of(a, a,   IsOdd()) ));

        ASSERT(!(bsl::all_of (a, a+1, IsOdd()) ));
        ASSERT( (bsl::all_of (a, a+1, isEven)  ));
        ASSERT(!(bsl::any_of (a, a+1, IsOdd()) ));
        ASSERT( (bsl::any_of (a, a+1, isEven)  ));
        ASSERT( (bsl::none_of(a, a+1, IsOdd()) ));
        ASSERT(!(bsl::none_of(a, a+1, isEven)  ));

        ASSERT( (bsl::all_of (b, b+5, isEven)  ));
        ASSERT(!(bsl::all_of (b, b+6, isEven)  ));
        ASSERT(!(bsl::all_of (c, c+5, isEven)  ));
        ASSERT(!(bsl::all_of (c, c+6, isEven)  ));

        ASSERT( (bsl::any_of (b, b+5, isEven)  ));
        ASSERT( (bsl::any_of (b, b+6, isEven)  ));
        ASSERT(!(bsl::any_of (c, c+5, isEven)  ));
        ASSERT( (bsl::any_of (c, c+6, isEven)  ));

        ASSERT(!(bsl::none_of(b, b+5, isEven)  ));
        ASSERT(!(bsl::none_of(b, b+6, isEven)  ));
        ASSERT( (bsl::none_of(c, c+5, isEven)  ));
        ASSERT(!(bsl::none_of(c, c+6, isEven)  ));
#endif
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

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

