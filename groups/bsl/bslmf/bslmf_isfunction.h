// bslmf_isfunction.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ISFUNCTION
#define INCLUDED_BSLMF_ISFUNCTION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for determining function types.
//
//@CLASSES:
//  bsl::is_function: standard meta-function for determining function types
//
//@SEE_ALSO: bslmf_integralconstant
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_function',
// that may be used to query whether a template parameter type is a function
// type.
//
// 'bsl::is_function' meets the requirements of the 'is_function' template
// defined in the C++11 standard [meta.unary.cat].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Function Types
/// - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are function types.
//
// Now, we instantiate the 'bsl::is_function' template for both a non-function
// type and a function type, and assert the 'value' static data member of each
// instantiation:
//..
//  assert(false == bsl::is_function<int>::value);
//  assert(true  == bsl::is_function<int (int)>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONST
#include <bslmf_isconst.h>
#endif

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#if defined(BSLS_PLATFORM_CMP_IBM)
// The IBM xlC compiler produces a hard (non-SFINAEable) error when trying to
// apply cv-qualifiers to a template parameter (or typedef) that is a function
// type.  Hence, a more oblique approach is needed to detect all function types
// on this platform.  This implementation relies on the fact that you cannot
// form an array of function types.

namespace BloombergLP {
namespace bslmf {

struct IsFunction_Imp {
    struct FalseType {
        // This 'struct' provides a type larger than 'char' to be used in
        // unevaluated contexts, allowing 'sizeof(selected overload)' to
        // distinguish which overload was called when all other overloads
        // return a 'char'.

        char d_dummy[17];  // Member to guarantee 'sizeof(FalseType) > 1'
    };

    template <class TYPE>
    static FalseType test(int TYPE::*, void *);
        // This function will match any class type, including abstract types.

    template <class TYPE>
    static FalseType test(TYPE (*)[2], ...);
        // This function will match all types other than those that cannot be
        // used to form an array.  This includes function types, reference
        // types, void types, and abstract types.  Further overloads and
        // specializations will filter the reference, array, and abstract
        // types.

    template <class TYPE>
    static char test(...);
        // This function, when called with '0' in a non-evaluated context, will
        // match anything that the previous overloads fail to match, which will
        // include all function types, reference types, void types, and arrays
        // of unknown bound.
};

} // close package namespace
} // close enterprise namespace

namespace bsl {

template <class TYPE>
struct is_function : integral_constant<bool,
           sizeof(BloombergLP::bslmf::IsFunction_Imp::test<TYPE>(0, 0)) == 1> {
    // This 'struct' template implements the 'is_function' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 'TYPE' is a function type.  This 'struct' derives
    // from 'bsl::true_type' if the 'TYPE' is a function type, and
    // from 'bsl::false_type' otherwise.
};

template <class TYPE>
struct is_function<TYPE []> : false_type {
    // Array types are, self-evidently, never function types.  Arrays of
    // unknown bound will be misdiagnosed by the 'IsFunction_Imp' detector, so
    // this template is partially specialized to resolve such cases.
};

template <class TYPE>
struct is_function<TYPE &> : false_type {
    // Reference types are, self-evidently, never function types.  This
    // template is partially specialized to resolve such cases, as the
    // detection idiom embodied in 'IsFunction_Imp' would yield the wrong
    // result otherwise.
};

template <>
struct is_function<void> : false_type {
    // 'void' types are not functions.  It is easier to provide 4 cv-qualified
    // explicit specializations than introduce a further dependency and chain
    // template instantiations through 'remove_cv'.
};

template <>
struct is_function<const void> : false_type {
    // 'void' types are not functions.  It is easier to provide 4 cv-qualified
    // explicit specializations than introduce a further dependency and chain
    // template instantiations through 'remove_cv'.
};

template <>
struct is_function<volatile void> : false_type {
    // 'void' types are not functions.  It is easier to provide 4 cv-qualified
    // explicit specializations than introduce a further dependency and chain
    // template instantiations through 'remove_cv'.
};

template <>
struct is_function<const volatile void> : false_type {
    // 'void' types are not functions.  It is easier to provide 4 cv-qualified
    // explicit specializations than introduce a further dependency and chain
    // template instantiations through 'remove_cv'.
};

} // close namespace bsl

#else  // This is the simplest implementation, for conforming compilers
namespace bsl {

template <class TYPE>
struct is_function
    :  bsl::integral_constant<bool, !is_const<const TYPE>::value> {
    // This 'struct' template implements the 'is_function' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 'TYPE' is a function type.  This 'struct' derives
    // from 'bsl::true_type' if the 'TYPE' is a function type, and from
    // 'bsl::false_type' otherwise.  This implementation relies on the fact
    // that neither function types nor reference types can be cv-qualified so
    // that 'is_const<const TYPE>' will actually yield 'false'.
};

template <class TYPE>
struct is_function<TYPE &> : false_type {
    // Reference types are, self-evidently, never function types.  The idiom
    // for detecting function types in this component is that a function is a
    // type that is the same as the const-qualified version of that same type.
    // As references also have this property, we must filter out references
    // with this partial specialization.
};

# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class TYPE>
struct is_function<TYPE &&> : false_type {
    // Reference types are, self-evidently, never function types.  The idiom
    // for detecting function types in this component is that a function is a
    // type that is the same as the const-qualified version of that same type.
    // As references also have this property, we must filter out references
    // with this partial specialization.
};
# endif

}  // close namespace bsl
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
