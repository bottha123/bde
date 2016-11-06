// bslmf_resulttype.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_RESULTTYPE
#define INCLUDED_BSLMF_RESULTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide access to 'result_type' or 'ResultType' nested type
//
//@CLASSES:
//     bslmf::ResultType<FUNC, FALLBACK>
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: A number of functor classes provide a typedef alias for the
// returned when invoking the functor.  Unfortunately, standard types name
// this typedef 'result_type' whereas BDE types name this typedef
// 'ResultType'. This component facilitates writing code that depends on these
// typedefs by providing a uniform interface for extracting the correct
// alias. 'bslmf::ResultType<FUNC>::type' is identical to 'FUNC::result_type'
// if such a type exists; otherwise, it is identical to 'FUNC::ResultType' if
// that type exists; otherwise it undefined. A fallback type can be optionally
// specified such that 'bslmf::ResultType<FUNC, FALLBACK>::type' is identical
// to 'FALLBACK' if neither 'FUNC::result_type' nor 'FUNC::ResultType' are
// defined.
//
///Usage
///-----
//
///Usage Example 1
///- - - - - - - -
// In this example, we want to wite a C++03-compatible function template,
// 'wrapInvoke<FUNC>(A1 a1, A2 a2)' that constructs an instance of
// functor 'FUNC', invokes it with argumens 'a1' and 'a2', and
// translates any thrown exception to a generic exception type.  First, we
// declare the generic exception type:
//..
//  struct InvocationException { };
//..
// Now, we declare 'wrapInvoke'. The return type of 'wrapInvoke' should be the
// same as the return type of invoking an object of type 'FUNC'. There is no
// non-intrusive way to deduce the return type of 'FUNC' in C++03. We
// therefore require that 'FUNC' provide either a 'result_type' nested type
// (the idiom used in standard library functors) or a 'ResultType' nested type
// (the idiom used in BDE library functors). We use 'bslmf::ResultType' to
// automatically select the correct idiom:
//..
//  template <class FUNC, class A1, class A2>
//  typename bslmf::ResultType<FUNC>::type
//  wrapInvoke(A1 a1, A2 a2) {
//      FUNC f;
//      try {
//          return f(a1, a2);
//      }
//      catch (...) {
//          throw InvocationException();
//      }
//  }
//..
// Next, we declare a functor class that compares its arguments, returns the
// string "less" if 'a1 < a2', returns "greater" if 'a2 > a1', and throws an
// exception of neither is true. The return type of this functor is declared
// using the BDE-style 'ResultType' nested type:
//..
//  struct LessGreater {
//      typedef const char* ResultType;
//      struct BadArgs { }; // Exception class
//
//      const char* operator()(long a1, long a2);
//  };
//
//  const char* LessGreater::operator()(long a1, long a2) {
//      if (a1 < a2) {
//          return "less";
//      }
//      else if (a2 < a1) {
//          return "greater";
//      }
//      else {
//          throw BadArgs();
//      }
//  }
//..
// For comparison, let's also define a 'plus' functor that conforms to the
// C++11 standard definition of 'std::plus':
//..
//  template <class T>
//  struct plus {
//      typedef T first_argument_type;
//      typedef T second_argument_type;
//      typedef T result_type;
//      T operator()(const T& x, const T& y) const { return x + y; }
//  };
//..
// Now, we can use 'wrapInvoke' with our 'LessGreater' functor:
//..
//  int main() {
//      const char* s = wrapInvoke<LessGreater>(5, -2);
//      assert(0 == std::strcmp(s, "greater"));
//..
// Finally we confirm that we can also use 'wrapInvoke' with the
// functor 'plus<int>':
//..
//      int sum = wrapInvoke<plus<int> >(5, -2);
//      assert(3 == sum);
//
//      return 0;
//  }
//..
///Usage Example 2
///- - - - - - - -
// This example extends the previous one by considering a functor that
// does not declare either 'result_type' or 'ResultType'.  The 'PostIncrement'
// functor performs the operation '*a += b' and returns the old value of '*a':
//..
//  struct PostIncrement {
//      int operator()(int* a, int b) {
//          unsigned tmp = *a;
//          *a += b;
//          return tmp;
//      }
//  };
//..
// Unfortunately, the following innocent-looking code is ill-formed; even
// though the return value is being discarded, the return type of 'wrapInvoke'
// is undefined because 'bslmf::ResultType<PostIncrement>::type' is undefined:
//..
//  // int x = 10;
//  // wrapInvoke<PostIncrement>(x, 2); // Ill formed
//..
// To make 'wrapInvoke' usable in these situations, we define a new version,
// 'wrapInvoke2', that will fall back to a 'void' return type if neither
// 'FUNC::result_type' nor 'FUNC::ResultType' is true:
//..
//  template <class FUNC, class A1, class A2>
//  typename bslmf::ResultType<FUNC, void>::type
//  wrapInvoke2(A1 a1, A2 a2) {
//      typedef typename bslmf::ResultType<FUNC, void>::type RetType;
//      FUNC f;
//      try {
//          // C-style cast needed for some compilers
//          return ((RetType) f(a1, a2));
//      }
//      catch (...) {
//          throw InvocationException();
//      }
//  }
//..
// This use of the fallback parameter allows us to use 'bslmf::ResultType' in
// a context where the return type of a function might be ignored:
//..
//  int main() {
//      int x = 10;
//      wrapInvoke2<PostIncrement>(&x, 2);
//      assert(12 == x);
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

#ifndef INCLUDED_BSLMF_VOIDTYPE
#include <bslmf_voidtype.h>
#endif

namespace BloombergLP {

namespace bslmf {

template <class FALLBACK>
struct ResultType_Fallback;
    // Forward declaration

                        // ================
                        // class ResultType
                        // ================

template <class FUNC, class FALLBACK = bslmf::Nil,
          class V1 = void, class V2 = void>
struct ResultType : ResultType_Fallback<FALLBACK> {
    // Metafunction to return the result type of the specified functor type
    // 'FUNC'. The nested 'type' is identical to 'FUNC::result_type' if such a
    // type exists; otherwise, it is identical to 'FUNC::ResultType' if that
    // type exists; otherwise it is identical to the 'FALLBACK' template
    // paramter if it was specified; otherwise it is undefined.
};

template <class FUNC, class FALLBACK, class V2>
struct ResultType<FUNC, FALLBACK,
                  typename VoidType<typename FUNC::result_type>::type, V2> {
    // Specialization of 'ResultType' for when 'FUNC::result_type' exists.

    typedef typename FUNC::result_type type;
};

template <class FUNC, class FALLBACK, class V1>
struct ResultType<FUNC, FALLBACK,
                  V1, typename VoidType<typename FUNC::ResultType>::type> {
    // Specialization of 'ResultType' for when 'FUNC::ResultType' exists.

    typedef typename FUNC::ResultType type;
};

template <class FUNC, class FALLBACK>
struct ResultType<FUNC, FALLBACK,
                  typename VoidType<typename FUNC::result_type>::type,
                  typename VoidType<typename FUNC::ResultType>::type> {
    // Specialization of 'ResultType' for when both 'FUNC::result_type' and
    // 'FUNC::ResultType' exist. 'FUNC::result_type' takes precidence.
    // Note: It is not possible to use a single instantiation of 'VoidType'
    // with two arguments because that is not considered to be more
    // specialized than 'VoidType' with only one argument. (This may be a
    // limitation of the implementation of 'VoidType' in bsl.)

    typedef typename FUNC::result_type type;
};

// ===========================================================================
//                TEMPLATE AND INLINE FUNCTION IMPLEMENTATIONS
// ===========================================================================

template <class FALLBACK>
struct ResultType_Fallback {
    // Metafunction to that defines 'type' to be the specified 'FALLBACK' type
    // unless 'FALLBACK' is 'bslmf::Nil'.

    typedef FALLBACK type;
};

template <>
struct ResultType_Fallback<bslmf::Nil> {
    // Specialization of 'ResultType_Fallback' which does not define 'type' if
    // 'bslmf::Nil' is specified as the fallback parameter.
};

}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_RESULTTYPE)

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
