// #include bsls_precheck.cpp                                         -*-C++-*-
#include <bsls_precheck.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_bsltestutil.h>        // for testing only

namespace BloombergLP {
namespace bsls {

const char *PreCheck::d_file     = "";
int         PreCheck::d_line     = -1;
bool        PreCheck::d_checking = false;
BloombergLP::bsls::AtomicBool PreCheck::d_ab_checking = false;


}  // close package namespace
}  // close enterprise namespace


// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
