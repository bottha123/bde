// baltzo_zoneinfobinaryreader.t.cpp                                  -*-C++-*-
#include <baltzo_zoneinfobinaryreader.h>

#include <baltzo_localtimedescriptor.h>
#include <baltzo_zoneinfo.h>
#include <baltzo_zoneinfobinaryheader.h>

#include <bdlb_bigendian.h>

#include <bdlt_epochutil.h>

#include <bdlsb_fixedmeminstreambuf.h>

#include <bslmf_assert.h>

#include <bsls_log.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_fstream.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

#include <bsls_byteorder.h>
#include <bsls_types.h>

using namespace BloombergLP;

using bsl::atoi;
using bsl::cout;
using bsl::endl;
using bsl::exit;
using bsl::flush;
using bsl::memcpy;
using bsl::memset;
using bsl::memcmp;
using bsl::strncmp;
using bsl::strncpy;

// ============================================================================
//                                  TEST PLAN
// ----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The 'baltzo::ZoneinfoBinaryReader' component contains a 'read' function to
// read the Zoneinfo binary data format from a byte stream.  A test apparatus
// is created for testing this component.
//
// The test apparatus contains functions to create an arbitrary Zoneinfo binary
// data.  A byte stream can be created from this data and the 'read' function
// can be called on the byte stream to produce a 'baltzo::Zoneinfo' object.
// The test apparatus contains the 'verifyTimeZone' function to verify the
// resulting 'baltzo::Zoneinfo' object matches the data in the byte stream.
// Both version '\0' and version '2' of the Zoneinfo binary data are tested.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 8] read(baltzo::Zoneinfo *, bsl::istream&, bA=0);
// [ 7] read(baltzo::Zoneinfo *, FileDescription *, bsl::istream&, bA=0);
//
// ----------------------------------------------------------------------------
// TEST APPARATUS
// [ 2] CONCERN: Test apparatus functions as documented.
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
// [ 3] CONCERN: 'read' properly process the abbreviation strings
// [ 4] CONCERN: 'read' properly process local time types data
// [ 5] CONCERN: 'read' properly process transition data
// [ 6] CONCERN: 'read' retrieve data with multiple transitions
// [ 9] CONCERN: 'read' properly process the time zone string
// [10] CONCERN: 'read' retrieve a real-life Zoneinfo data
// [11] CONCERN: 'read' fails when header information is invalid
// ----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\t" \
                         << #L << ": " << L << "\n";\
               aSsErT(1, #X, __LINE__); } }
// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"
#define T_ cout << '\t' << flush;

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

static const bsls::Types::Int64 FIRST_TRANSITION =
              bdlt::EpochUtil::convertToTimeT64(bdlt::Datetime(1, 1, 1));
// DATA
const char unsigned NEW_YORK_DATA[] = {
    // Data from America/New_York

    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xeb,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x10, 0x9e, 0xa6, 0x1e, 0x70,
    0x9f, 0xba, 0xeb, 0x60, 0xa0, 0x86, 0x00, 0x70, 0xa1, 0x9a, 0xcd, 0x60,
    0xa2, 0x65, 0xe2, 0x70, 0xa3, 0x83, 0xe9, 0xe0, 0xa4, 0x6a, 0xae, 0x70,
    0xa5, 0x35, 0xa7, 0x60, 0xa6, 0x53, 0xca, 0xf0, 0xa7, 0x15, 0x89, 0x60,
    0xa8, 0x33, 0xac, 0xf0, 0xa8, 0xfe, 0xa5, 0xe0, 0xaa, 0x13, 0x8e, 0xf0,
    0xaa, 0xde, 0x87, 0xe0, 0xab, 0xf3, 0x70, 0xf0, 0xac, 0xbe, 0x69, 0xe0,
    0xad, 0xd3, 0x52, 0xf0, 0xae, 0x9e, 0x4b, 0xe0, 0xaf, 0xb3, 0x34, 0xf0,
    0xb0, 0x7e, 0x2d, 0xe0, 0xb1, 0x9c, 0x51, 0x70, 0xb2, 0x67, 0x4a, 0x60,
    0xb3, 0x7c, 0x33, 0x70, 0xb4, 0x47, 0x2c, 0x60, 0xb5, 0x5c, 0x15, 0x70,
    0xb6, 0x27, 0x0e, 0x60, 0xb7, 0x3b, 0xf7, 0x70, 0xb8, 0x06, 0xf0, 0x60,
    0xb9, 0x1b, 0xd9, 0x70, 0xb9, 0xe6, 0xd2, 0x60, 0xbb, 0x04, 0xf5, 0xf0,
    0xbb, 0xc6, 0xb4, 0x60, 0xbc, 0xe4, 0xd7, 0xf0, 0xbd, 0xaf, 0xd0, 0xe0,
    0xbe, 0xc4, 0xb9, 0xf0, 0xbf, 0x8f, 0xb2, 0xe0, 0xc0, 0xa4, 0x9b, 0xf0,
    0xc1, 0x6f, 0x94, 0xe0, 0xc2, 0x84, 0x7d, 0xf0, 0xc3, 0x4f, 0x76, 0xe0,
    0xc4, 0x64, 0x5f, 0xf0, 0xc5, 0x2f, 0x58, 0xe0, 0xc6, 0x4d, 0x7c, 0x70,
    0xc7, 0x0f, 0x3a, 0xe0, 0xc8, 0x2d, 0x5e, 0x70, 0xc8, 0xf8, 0x57, 0x60,
    0xca, 0x0d, 0x40, 0x70, 0xca, 0xd8, 0x39, 0x60, 0xcb, 0x88, 0xf0, 0x70,
    0xd2, 0x23, 0xf4, 0x70, 0xd2, 0x60, 0xfb, 0xe0, 0xd3, 0x75, 0xe4, 0xf0,
    0xd4, 0x40, 0xdd, 0xe0, 0xd5, 0x55, 0xc6, 0xf0, 0xd6, 0x20, 0xbf, 0xe0,
    0xd7, 0x35, 0xa8, 0xf0, 0xd8, 0x00, 0xa1, 0xe0, 0xd9, 0x15, 0x8a, 0xf0,
    0xd9, 0xe0, 0x83, 0xe0, 0xda, 0xfe, 0xa7, 0x70, 0xdb, 0xc0, 0x65, 0xe0,
    0xdc, 0xde, 0x89, 0x70, 0xdd, 0xa9, 0x82, 0x60, 0xde, 0xbe, 0x6b, 0x70,
    0xdf, 0x89, 0x64, 0x60, 0xe0, 0x9e, 0x4d, 0x70, 0xe1, 0x69, 0x46, 0x60,
    0xe2, 0x7e, 0x2f, 0x70, 0xe3, 0x49, 0x28, 0x60, 0xe4, 0x5e, 0x11, 0x70,
    0xe5, 0x57, 0x2e, 0xe0, 0xe6, 0x47, 0x2d, 0xf0, 0xe7, 0x37, 0x10, 0xe0,
    0xe8, 0x27, 0x0f, 0xf0, 0xe9, 0x16, 0xf2, 0xe0, 0xea, 0x06, 0xf1, 0xf0,
    0xea, 0xf6, 0xd4, 0xe0, 0xeb, 0xe6, 0xd3, 0xf0, 0xec, 0xd6, 0xb6, 0xe0,
    0xed, 0xc6, 0xb5, 0xf0, 0xee, 0xbf, 0xd3, 0x60, 0xef, 0xaf, 0xd2, 0x70,
    0xf0, 0x9f, 0xb5, 0x60, 0xf1, 0x8f, 0xb4, 0x70, 0xf2, 0x7f, 0x97, 0x60,
    0xf3, 0x6f, 0x96, 0x70, 0xf4, 0x5f, 0x79, 0x60, 0xf5, 0x4f, 0x78, 0x70,
    0xf6, 0x3f, 0x5b, 0x60, 0xf7, 0x2f, 0x5a, 0x70, 0xf8, 0x28, 0x77, 0xe0,
    0xf9, 0x0f, 0x3c, 0x70, 0xfa, 0x08, 0x59, 0xe0, 0xfa, 0xf8, 0x58, 0xf0,
    0xfb, 0xe8, 0x3b, 0xe0, 0xfc, 0xd8, 0x3a, 0xf0, 0xfd, 0xc8, 0x1d, 0xe0,
    0xfe, 0xb8, 0x1c, 0xf0, 0xff, 0xa7, 0xff, 0xe0, 0x00, 0x97, 0xfe, 0xf0,
    0x01, 0x87, 0xe1, 0xe0, 0x02, 0x77, 0xe0, 0xf0, 0x03, 0x70, 0xfe, 0x60,
    0x04, 0x60, 0xfd, 0x70, 0x05, 0x50, 0xe0, 0x60, 0x06, 0x40, 0xdf, 0x70,
    0x07, 0x30, 0xc2, 0x60, 0x07, 0x8d, 0x19, 0x70, 0x09, 0x10, 0xa4, 0x60,
    0x09, 0xad, 0x94, 0xf0, 0x0a, 0xf0, 0x86, 0x60, 0x0b, 0xe0, 0x85, 0x70,
    0x0c, 0xd9, 0xa2, 0xe0, 0x0d, 0xc0, 0x67, 0x70, 0x0e, 0xb9, 0x84, 0xe0,
    0x0f, 0xa9, 0x83, 0xf0, 0x10, 0x99, 0x66, 0xe0, 0x11, 0x89, 0x65, 0xf0,
    0x12, 0x79, 0x48, 0xe0, 0x13, 0x69, 0x47, 0xf0, 0x14, 0x59, 0x2a, 0xe0,
    0x15, 0x49, 0x29, 0xf0, 0x16, 0x39, 0x0c, 0xe0, 0x17, 0x29, 0x0b, 0xf0,
    0x18, 0x22, 0x29, 0x60, 0x19, 0x08, 0xed, 0xf0, 0x1a, 0x02, 0x0b, 0x60,
    0x1a, 0xf2, 0x0a, 0x70, 0x1b, 0xe1, 0xed, 0x60, 0x1c, 0xd1, 0xec, 0x70,
    0x1d, 0xc1, 0xcf, 0x60, 0x1e, 0xb1, 0xce, 0x70, 0x1f, 0xa1, 0xb1, 0x60,
    0x20, 0x76, 0x00, 0xf0, 0x21, 0x81, 0x93, 0x60, 0x22, 0x55, 0xe2, 0xf0,
    0x23, 0x6a, 0xaf, 0xe0, 0x24, 0x35, 0xc4, 0xf0, 0x25, 0x4a, 0x91, 0xe0,
    0x26, 0x15, 0xa6, 0xf0, 0x27, 0x2a, 0x73, 0xe0, 0x27, 0xfe, 0xc3, 0x70,
    0x29, 0x0a, 0x55, 0xe0, 0x29, 0xde, 0xa5, 0x70, 0x2a, 0xea, 0x37, 0xe0,
    0x2b, 0xbe, 0x87, 0x70, 0x2c, 0xd3, 0x54, 0x60, 0x2d, 0x9e, 0x69, 0x70,
    0x2e, 0xb3, 0x36, 0x60, 0x2f, 0x7e, 0x4b, 0x70, 0x30, 0x93, 0x18, 0x60,
    0x31, 0x67, 0x67, 0xf0, 0x32, 0x72, 0xfa, 0x60, 0x33, 0x47, 0x49, 0xf0,
    0x34, 0x52, 0xdc, 0x60, 0x35, 0x27, 0x2b, 0xf0, 0x36, 0x32, 0xbe, 0x60,
    0x37, 0x07, 0x0d, 0xf0, 0x38, 0x1b, 0xda, 0xe0, 0x38, 0xe6, 0xef, 0xf0,
    0x39, 0xfb, 0xbc, 0xe0, 0x3a, 0xc6, 0xd1, 0xf0, 0x3b, 0xdb, 0x9e, 0xe0,
    0x3c, 0xaf, 0xee, 0x70, 0x3d, 0xbb, 0x80, 0xe0, 0x3e, 0x8f, 0xd0, 0x70,
    0x3f, 0x9b, 0x62, 0xe0, 0x40, 0x6f, 0xb2, 0x70, 0x41, 0x84, 0x7f, 0x60,
    0x42, 0x4f, 0x94, 0x70, 0x43, 0x64, 0x61, 0x60, 0x44, 0x2f, 0x76, 0x70,
    0x45, 0x44, 0x43, 0x60, 0x45, 0xf3, 0xa8, 0xf0, 0x47, 0x2d, 0x5f, 0xe0,
    0x47, 0xd3, 0x8a, 0xf0, 0x49, 0x0d, 0x41, 0xe0, 0x49, 0xb3, 0x6c, 0xf0,
    0x4a, 0xed, 0x23, 0xe0, 0x4b, 0x9c, 0x89, 0x70, 0x4c, 0xd6, 0x40, 0x60,
    0x4d, 0x7c, 0x6b, 0x70, 0x4e, 0xb6, 0x22, 0x60, 0x4f, 0x5c, 0x4d, 0x70,
    0x50, 0x96, 0x04, 0x60, 0x51, 0x3c, 0x2f, 0x70, 0x52, 0x75, 0xe6, 0x60,
    0x53, 0x1c, 0x11, 0x70, 0x54, 0x55, 0xc8, 0x60, 0x54, 0xfb, 0xf3, 0x70,
    0x56, 0x35, 0xaa, 0x60, 0x56, 0xe5, 0x0f, 0xf0, 0x58, 0x1e, 0xc6, 0xe0,
    0x58, 0xc4, 0xf1, 0xf0, 0x59, 0xfe, 0xa8, 0xe0, 0x5a, 0xa4, 0xd3, 0xf0,
    0x5b, 0xde, 0x8a, 0xe0, 0x5c, 0x84, 0xb5, 0xf0, 0x5d, 0xbe, 0x6c, 0xe0,
    0x5e, 0x64, 0x97, 0xf0, 0x5f, 0x9e, 0x4e, 0xe0, 0x60, 0x4d, 0xb4, 0x70,
    0x61, 0x87, 0x6b, 0x60, 0x62, 0x2d, 0x96, 0x70, 0x63, 0x67, 0x4d, 0x60,
    0x64, 0x0d, 0x78, 0x70, 0x65, 0x47, 0x2f, 0x60, 0x65, 0xed, 0x5a, 0x70,
    0x67, 0x27, 0x11, 0x60, 0x67, 0xcd, 0x3c, 0x70, 0x69, 0x06, 0xf3, 0x60,
    0x69, 0xad, 0x1e, 0x70, 0x6a, 0xe6, 0xd5, 0x60, 0x6b, 0x96, 0x3a, 0xf0,
    0x6c, 0xcf, 0xf1, 0xe0, 0x6d, 0x76, 0x1c, 0xf0, 0x6e, 0xaf, 0xd3, 0xe0,
    0x6f, 0x55, 0xfe, 0xf0, 0x70, 0x8f, 0xb5, 0xe0, 0x71, 0x35, 0xe0, 0xf0,
    0x72, 0x6f, 0x97, 0xe0, 0x73, 0x15, 0xc2, 0xf0, 0x74, 0x4f, 0x79, 0xe0,
    0x74, 0xfe, 0xdf, 0x70, 0x76, 0x38, 0x96, 0x60, 0x76, 0xde, 0xc1, 0x70,
    0x78, 0x18, 0x78, 0x60, 0x78, 0xbe, 0xa3, 0x70, 0x79, 0xf8, 0x5a, 0x60,
    0x7a, 0x9e, 0x85, 0x70, 0x7b, 0xd8, 0x3c, 0x60, 0x7c, 0x7e, 0x67, 0x70,
    0x7d, 0xb8, 0x1e, 0x60, 0x7e, 0x5e, 0x49, 0x70, 0x7f, 0x98, 0x00, 0x60,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x02, 0x03, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0xff, 0xff, 0xc7, 0xc0, 0x01,
    0x00, 0xff, 0xff, 0xb9, 0xb0, 0x00, 0x04, 0xff, 0xff, 0xc7, 0xc0, 0x01,
    0x08, 0xff, 0xff, 0xc7, 0xc0, 0x01, 0x0c, 0x45, 0x44, 0x54, 0x00, 0x45,
    0x53, 0x54, 0x00, 0x45, 0x57, 0x54, 0x00, 0x45, 0x50, 0x54, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x54, 0x5a, 0x69, 0x66, 0x32,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xec, 0x00, 0x00, 0x00, 0x05, 0x00,
    0x00, 0x00, 0x14, 0xff, 0xff, 0xff, 0xff, 0x5e, 0x03, 0xf0, 0x90, 0xff,
    0xff, 0xff, 0xff, 0x9e, 0xa6, 0x1e, 0x70, 0xff, 0xff, 0xff, 0xff, 0x9f,
    0xba, 0xeb, 0x60, 0xff, 0xff, 0xff, 0xff, 0xa0, 0x86, 0x00, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xa1, 0x9a, 0xcd, 0x60, 0xff, 0xff, 0xff, 0xff, 0xa2,
    0x65, 0xe2, 0x70, 0xff, 0xff, 0xff, 0xff, 0xa3, 0x83, 0xe9, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xa4, 0x6a, 0xae, 0x70, 0xff, 0xff, 0xff, 0xff, 0xa5,
    0x35, 0xa7, 0x60, 0xff, 0xff, 0xff, 0xff, 0xa6, 0x53, 0xca, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xa7, 0x15, 0x89, 0x60, 0xff, 0xff, 0xff, 0xff, 0xa8,
    0x33, 0xac, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xa8, 0xfe, 0xa5, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xaa, 0x13, 0x8e, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xaa,
    0xde, 0x87, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xab, 0xf3, 0x70, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xac, 0xbe, 0x69, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xad,
    0xd3, 0x52, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xae, 0x9e, 0x4b, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xaf, 0xb3, 0x34, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xb0,
    0x7e, 0x2d, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xb1, 0x9c, 0x51, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xb2, 0x67, 0x4a, 0x60, 0xff, 0xff, 0xff, 0xff, 0xb3,
    0x7c, 0x33, 0x70, 0xff, 0xff, 0xff, 0xff, 0xb4, 0x47, 0x2c, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xb5, 0x5c, 0x15, 0x70, 0xff, 0xff, 0xff, 0xff, 0xb6,
    0x27, 0x0e, 0x60, 0xff, 0xff, 0xff, 0xff, 0xb7, 0x3b, 0xf7, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xb8, 0x06, 0xf0, 0x60, 0xff, 0xff, 0xff, 0xff, 0xb9,
    0x1b, 0xd9, 0x70, 0xff, 0xff, 0xff, 0xff, 0xb9, 0xe6, 0xd2, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xbb, 0x04, 0xf5, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xbb,
    0xc6, 0xb4, 0x60, 0xff, 0xff, 0xff, 0xff, 0xbc, 0xe4, 0xd7, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xbd, 0xaf, 0xd0, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xbe,
    0xc4, 0xb9, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xbf, 0x8f, 0xb2, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xc0, 0xa4, 0x9b, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xc1,
    0x6f, 0x94, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xc2, 0x84, 0x7d, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xc3, 0x4f, 0x76, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xc4,
    0x64, 0x5f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xc5, 0x2f, 0x58, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xc6, 0x4d, 0x7c, 0x70, 0xff, 0xff, 0xff, 0xff, 0xc7,
    0x0f, 0x3a, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xc8, 0x2d, 0x5e, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xc8, 0xf8, 0x57, 0x60, 0xff, 0xff, 0xff, 0xff, 0xca,
    0x0d, 0x40, 0x70, 0xff, 0xff, 0xff, 0xff, 0xca, 0xd8, 0x39, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xcb, 0x88, 0xf0, 0x70, 0xff, 0xff, 0xff, 0xff, 0xd2,
    0x23, 0xf4, 0x70, 0xff, 0xff, 0xff, 0xff, 0xd2, 0x60, 0xfb, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xd3, 0x75, 0xe4, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xd4,
    0x40, 0xdd, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xd5, 0x55, 0xc6, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xd6, 0x20, 0xbf, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xd7,
    0x35, 0xa8, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xd8, 0x00, 0xa1, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xd9, 0x15, 0x8a, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xd9,
    0xe0, 0x83, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xda, 0xfe, 0xa7, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xdb, 0xc0, 0x65, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xdc,
    0xde, 0x89, 0x70, 0xff, 0xff, 0xff, 0xff, 0xdd, 0xa9, 0x82, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xde, 0xbe, 0x6b, 0x70, 0xff, 0xff, 0xff, 0xff, 0xdf,
    0x89, 0x64, 0x60, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x9e, 0x4d, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xe1, 0x69, 0x46, 0x60, 0xff, 0xff, 0xff, 0xff, 0xe2,
    0x7e, 0x2f, 0x70, 0xff, 0xff, 0xff, 0xff, 0xe3, 0x49, 0x28, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xe4, 0x5e, 0x11, 0x70, 0xff, 0xff, 0xff, 0xff, 0xe5,
    0x57, 0x2e, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe6, 0x47, 0x2d, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xe7, 0x37, 0x10, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe8,
    0x27, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xe9, 0x16, 0xf2, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xea, 0x06, 0xf1, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xea,
    0xf6, 0xd4, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xeb, 0xe6, 0xd3, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xec, 0xd6, 0xb6, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xed,
    0xc6, 0xb5, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xee, 0xbf, 0xd3, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xef, 0xaf, 0xd2, 0x70, 0xff, 0xff, 0xff, 0xff, 0xf0,
    0x9f, 0xb5, 0x60, 0xff, 0xff, 0xff, 0xff, 0xf1, 0x8f, 0xb4, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xf2, 0x7f, 0x97, 0x60, 0xff, 0xff, 0xff, 0xff, 0xf3,
    0x6f, 0x96, 0x70, 0xff, 0xff, 0xff, 0xff, 0xf4, 0x5f, 0x79, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xf5, 0x4f, 0x78, 0x70, 0xff, 0xff, 0xff, 0xff, 0xf6,
    0x3f, 0x5b, 0x60, 0xff, 0xff, 0xff, 0xff, 0xf7, 0x2f, 0x5a, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xf8, 0x28, 0x77, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xf9,
    0x0f, 0x3c, 0x70, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x08, 0x59, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xfa, 0xf8, 0x58, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xfb,
    0xe8, 0x3b, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xd8, 0x3a, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xfd, 0xc8, 0x1d, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xfe,
    0xb8, 0x1c, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xa7, 0xff, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x97, 0xfe, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x87, 0xe1, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x77, 0xe0, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x03, 0x70, 0xfe, 0x60, 0x00, 0x00, 0x00, 0x00, 0x04,
    0x60, 0xfd, 0x70, 0x00, 0x00, 0x00, 0x00, 0x05, 0x50, 0xe0, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x06, 0x40, 0xdf, 0x70, 0x00, 0x00, 0x00, 0x00, 0x07,
    0x30, 0xc2, 0x60, 0x00, 0x00, 0x00, 0x00, 0x07, 0x8d, 0x19, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x09, 0x10, 0xa4, 0x60, 0x00, 0x00, 0x00, 0x00, 0x09,
    0xad, 0x94, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xf0, 0x86, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x0b, 0xe0, 0x85, 0x70, 0x00, 0x00, 0x00, 0x00, 0x0c,
    0xd9, 0xa2, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0d, 0xc0, 0x67, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x0e, 0xb9, 0x84, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0f,
    0xa9, 0x83, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x10, 0x99, 0x66, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x11, 0x89, 0x65, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x12,
    0x79, 0x48, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x13, 0x69, 0x47, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x14, 0x59, 0x2a, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x15,
    0x49, 0x29, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x16, 0x39, 0x0c, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x17, 0x29, 0x0b, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x18,
    0x22, 0x29, 0x60, 0x00, 0x00, 0x00, 0x00, 0x19, 0x08, 0xed, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x1a, 0x02, 0x0b, 0x60, 0x00, 0x00, 0x00, 0x00, 0x1a,
    0xf2, 0x0a, 0x70, 0x00, 0x00, 0x00, 0x00, 0x1b, 0xe1, 0xed, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x1c, 0xd1, 0xec, 0x70, 0x00, 0x00, 0x00, 0x00, 0x1d,
    0xc1, 0xcf, 0x60, 0x00, 0x00, 0x00, 0x00, 0x1e, 0xb1, 0xce, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x1f, 0xa1, 0xb1, 0x60, 0x00, 0x00, 0x00, 0x00, 0x20,
    0x76, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x21, 0x81, 0x93, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x22, 0x55, 0xe2, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x23,
    0x6a, 0xaf, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x24, 0x35, 0xc4, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x25, 0x4a, 0x91, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x26,
    0x15, 0xa6, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x27, 0x2a, 0x73, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x27, 0xfe, 0xc3, 0x70, 0x00, 0x00, 0x00, 0x00, 0x29,
    0x0a, 0x55, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x29, 0xde, 0xa5, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x2a, 0xea, 0x37, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x2b,
    0xbe, 0x87, 0x70, 0x00, 0x00, 0x00, 0x00, 0x2c, 0xd3, 0x54, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x2d, 0x9e, 0x69, 0x70, 0x00, 0x00, 0x00, 0x00, 0x2e,
    0xb3, 0x36, 0x60, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x7e, 0x4b, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x93, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00, 0x31,
    0x67, 0x67, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x32, 0x72, 0xfa, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x33, 0x47, 0x49, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x34,
    0x52, 0xdc, 0x60, 0x00, 0x00, 0x00, 0x00, 0x35, 0x27, 0x2b, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x36, 0x32, 0xbe, 0x60, 0x00, 0x00, 0x00, 0x00, 0x37,
    0x07, 0x0d, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x38, 0x1b, 0xda, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x38, 0xe6, 0xef, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x39,
    0xfb, 0xbc, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3a, 0xc6, 0xd1, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x3b, 0xdb, 0x9e, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3c,
    0xaf, 0xee, 0x70, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xbb, 0x80, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x3e, 0x8f, 0xd0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x3f,
    0x9b, 0x62, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x40, 0x6f, 0xb2, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x41, 0x84, 0x7f, 0x60, 0x00, 0x00, 0x00, 0x00, 0x42,
    0x4f, 0x94, 0x70, 0x00, 0x00, 0x00, 0x00, 0x43, 0x64, 0x61, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x44, 0x2f, 0x76, 0x70, 0x00, 0x00, 0x00, 0x00, 0x45,
    0x44, 0x43, 0x60, 0x00, 0x00, 0x00, 0x00, 0x45, 0xf3, 0xa8, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x47, 0x2d, 0x5f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x47,
    0xd3, 0x8a, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x49, 0x0d, 0x41, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x49, 0xb3, 0x6c, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x4a,
    0xed, 0x23, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x4b, 0x9c, 0x89, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x4c, 0xd6, 0x40, 0x60, 0x00, 0x00, 0x00, 0x00, 0x4d,
    0x7c, 0x6b, 0x70, 0x00, 0x00, 0x00, 0x00, 0x4e, 0xb6, 0x22, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x4f, 0x5c, 0x4d, 0x70, 0x00, 0x00, 0x00, 0x00, 0x50,
    0x96, 0x04, 0x60, 0x00, 0x00, 0x00, 0x00, 0x51, 0x3c, 0x2f, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x52, 0x75, 0xe6, 0x60, 0x00, 0x00, 0x00, 0x00, 0x53,
    0x1c, 0x11, 0x70, 0x00, 0x00, 0x00, 0x00, 0x54, 0x55, 0xc8, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x54, 0xfb, 0xf3, 0x70, 0x00, 0x00, 0x00, 0x00, 0x56,
    0x35, 0xaa, 0x60, 0x00, 0x00, 0x00, 0x00, 0x56, 0xe5, 0x0f, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x58, 0x1e, 0xc6, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x58,
    0xc4, 0xf1, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x59, 0xfe, 0xa8, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x5a, 0xa4, 0xd3, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x5b,
    0xde, 0x8a, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x5c, 0x84, 0xb5, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x5d, 0xbe, 0x6c, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x5e,
    0x64, 0x97, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x9e, 0x4e, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x60, 0x4d, 0xb4, 0x70, 0x00, 0x00, 0x00, 0x00, 0x61,
    0x87, 0x6b, 0x60, 0x00, 0x00, 0x00, 0x00, 0x62, 0x2d, 0x96, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x63, 0x67, 0x4d, 0x60, 0x00, 0x00, 0x00, 0x00, 0x64,
    0x0d, 0x78, 0x70, 0x00, 0x00, 0x00, 0x00, 0x65, 0x47, 0x2f, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x65, 0xed, 0x5a, 0x70, 0x00, 0x00, 0x00, 0x00, 0x67,
    0x27, 0x11, 0x60, 0x00, 0x00, 0x00, 0x00, 0x67, 0xcd, 0x3c, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x69, 0x06, 0xf3, 0x60, 0x00, 0x00, 0x00, 0x00, 0x69,
    0xad, 0x1e, 0x70, 0x00, 0x00, 0x00, 0x00, 0x6a, 0xe6, 0xd5, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x6b, 0x96, 0x3a, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x6c,
    0xcf, 0xf1, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x76, 0x1c, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x6e, 0xaf, 0xd3, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x6f,
    0x55, 0xfe, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x70, 0x8f, 0xb5, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x71, 0x35, 0xe0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x72,
    0x6f, 0x97, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x73, 0x15, 0xc2, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x74, 0x4f, 0x79, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x74,
    0xfe, 0xdf, 0x70, 0x00, 0x00, 0x00, 0x00, 0x76, 0x38, 0x96, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x76, 0xde, 0xc1, 0x70, 0x00, 0x00, 0x00, 0x00, 0x78,
    0x18, 0x78, 0x60, 0x00, 0x00, 0x00, 0x00, 0x78, 0xbe, 0xa3, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x79, 0xf8, 0x5a, 0x60, 0x00, 0x00, 0x00, 0x00, 0x7a,
    0x9e, 0x85, 0x70, 0x00, 0x00, 0x00, 0x00, 0x7b, 0xd8, 0x3c, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x7c, 0x7e, 0x67, 0x70, 0x00, 0x00, 0x00, 0x00, 0x7d,
    0xb8, 0x1e, 0x60, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x5e, 0x49, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x7f, 0x98, 0x00, 0x60, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x03, 0x04, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0xff, 0xff, 0xba, 0x9e, 0x00, 0x00, 0xff, 0xff, 0xc7,
    0xc0, 0x01, 0x04, 0xff, 0xff, 0xb9, 0xb0, 0x00, 0x08, 0xff, 0xff, 0xc7,
    0xc0, 0x01, 0x0c, 0xff, 0xff, 0xc7, 0xc0, 0x01, 0x10, 0x4c, 0x4d, 0x54,
    0x00, 0x45, 0x44, 0x54, 0x00, 0x45, 0x53, 0x54, 0x00, 0x45, 0x57, 0x54,
    0x00, 0x45, 0x50, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x0a, 0x45, 0x53, 0x54, 0x35, 0x45, 0x44, 0x54, 0x2c,
    0x4d, 0x33, 0x2e, 0x32, 0x2e, 0x30, 0x2c, 0x4d, 0x31, 0x31, 0x2e, 0x31,
    0x2e, 0x30, 0x0a
};

const unsigned char LONDON_DATA[] = {
    // Data from Europe/London

    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
    0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2,
    0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x0d, 0x9b, 0x26, 0xad, 0xa0,
    0x9b, 0xd6, 0x05, 0x20, 0x9c, 0xcf, 0x30, 0xa0, 0x9d, 0xa4, 0xc3, 0xa0,
    0x9e, 0x9c, 0x9d, 0xa0, 0x9f, 0x97, 0x1a, 0xa0, 0xa0, 0x85, 0xba, 0x20,
    0xa1, 0x76, 0xfc, 0xa0, 0xa2, 0x65, 0x9c, 0x20, 0xa3, 0x7b, 0xc8, 0xa0,
    0xa4, 0x4e, 0xb8, 0xa0, 0xa5, 0x3f, 0xfb, 0x20, 0xa6, 0x25, 0x60, 0x20,
    0xa7, 0x27, 0xc6, 0x20, 0xa8, 0x2a, 0x2c, 0x20, 0xa8, 0xeb, 0xf8, 0xa0,
    0xaa, 0x00, 0xd3, 0xa0, 0xaa, 0xd5, 0x15, 0x20, 0xab, 0xe9, 0xf0, 0x20,
    0xac, 0xc7, 0x6c, 0x20, 0xad, 0xc9, 0xd2, 0x20, 0xae, 0xa7, 0x4e, 0x20,
    0xaf, 0xa0, 0x79, 0xa0, 0xb0, 0x87, 0x30, 0x20, 0xb1, 0x92, 0xd0, 0xa0,
    0xb2, 0x70, 0x4c, 0xa0, 0xb3, 0x72, 0xb2, 0xa0, 0xb4, 0x50, 0x2e, 0xa0,
    0xb5, 0x49, 0x5a, 0x20, 0xb6, 0x30, 0x10, 0xa0, 0xb7, 0x32, 0x76, 0xa0,
    0xb8, 0x0f, 0xf2, 0xa0, 0xb9, 0x12, 0x58, 0xa0, 0xb9, 0xef, 0xd4, 0xa0,
    0xba, 0xe9, 0x00, 0x20, 0xbb, 0xd8, 0xf1, 0x20, 0xbc, 0xdb, 0x57, 0x20,
    0xbd, 0xb8, 0xd3, 0x20, 0xbe, 0xb1, 0xfe, 0xa0, 0xbf, 0x98, 0xb5, 0x20,
    0xc0, 0x9b, 0x1b, 0x20, 0xc1, 0x78, 0x97, 0x20, 0xc2, 0x7a, 0xfd, 0x20,
    0xc3, 0x58, 0x79, 0x20, 0xc4, 0x51, 0xa4, 0xa0, 0xc5, 0x38, 0x5b, 0x20,
    0xc6, 0x3a, 0xc1, 0x20, 0xc7, 0x58, 0xd6, 0xa0, 0xc7, 0xda, 0x09, 0xa0,
    0xca, 0x16, 0x26, 0x90, 0xca, 0x97, 0x59, 0x90, 0xcb, 0xd1, 0x1e, 0x90,
    0xcc, 0x77, 0x3b, 0x90, 0xcd, 0xb1, 0x00, 0x90, 0xce, 0x60, 0x58, 0x10,
    0xcf, 0x90, 0xe2, 0x90, 0xd0, 0x6e, 0x5e, 0x90, 0xd1, 0x72, 0x16, 0x10,
    0xd1, 0xfb, 0x32, 0x10, 0xd2, 0x69, 0xfe, 0x20, 0xd3, 0x63, 0x29, 0xa0,
    0xd4, 0x49, 0xe0, 0x20, 0xd5, 0x1e, 0x21, 0xa0, 0xd5, 0x42, 0xfd, 0x90,
    0xd5, 0xdf, 0xe0, 0x10, 0xd6, 0x4e, 0xac, 0x20, 0xd6, 0xfe, 0x03, 0xa0,
    0xd8, 0x2e, 0x8e, 0x20, 0xd8, 0xf9, 0x95, 0x20, 0xda, 0x0e, 0x70, 0x20,
    0xda, 0xeb, 0xec, 0x20, 0xdb, 0xe5, 0x17, 0xa0, 0xdc, 0xcb, 0xce, 0x20,
    0xdd, 0xc4, 0xf9, 0xa0, 0xde, 0xb4, 0xea, 0xa0, 0xdf, 0xae, 0x16, 0x20,
    0xe0, 0x94, 0xcc, 0xa0, 0xe1, 0x72, 0x48, 0xa0, 0xe2, 0x6b, 0x74, 0x20,
    0xe3, 0x52, 0x2a, 0xa0, 0xe4, 0x54, 0x90, 0xa0, 0xe5, 0x32, 0x0c, 0xa0,
    0xe6, 0x3d, 0xad, 0x20, 0xe7, 0x1b, 0x29, 0x20, 0xe8, 0x14, 0x54, 0xa0,
    0xe8, 0xfb, 0x0b, 0x20, 0xe9, 0xfd, 0x71, 0x20, 0xea, 0xda, 0xed, 0x20,
    0xeb, 0xdd, 0x53, 0x20, 0xec, 0xba, 0xcf, 0x20, 0xed, 0xb3, 0xfa, 0xa0,
    0xee, 0x9a, 0xb1, 0x20, 0xef, 0x81, 0x67, 0xa0, 0xf0, 0x9f, 0x7d, 0x20,
    0xf1, 0x61, 0x49, 0xa0, 0xf2, 0x7f, 0x5f, 0x20, 0xf3, 0x4a, 0x66, 0x20,
    0xf4, 0x5f, 0x41, 0x20, 0xf5, 0x21, 0x0d, 0xa0, 0xf6, 0x3f, 0x23, 0x20,
    0xf7, 0x00, 0xef, 0xa0, 0xf8, 0x1f, 0x05, 0x20, 0xf8, 0xe0, 0xd1, 0xa0,
    0xf9, 0xfe, 0xe7, 0x20, 0xfa, 0xc0, 0xb3, 0xa0, 0xfb, 0xe8, 0x03, 0xa0,
    0xfc, 0x7b, 0xab, 0xa0, 0xfd, 0xc7, 0xbb, 0x70, 0x03, 0x70, 0xc6, 0x20,
    0x04, 0x29, 0x58, 0x20, 0x05, 0x50, 0xa8, 0x20, 0x06, 0x09, 0x3a, 0x20,
    0x07, 0x30, 0x8a, 0x20, 0x07, 0xe9, 0x1c, 0x20, 0x09, 0x10, 0x6c, 0x20,
    0x09, 0xc8, 0xfe, 0x20, 0x0a, 0xf0, 0x4e, 0x20, 0x0b, 0xb2, 0x1a, 0xa0,
    0x0c, 0xd0, 0x30, 0x20, 0x0d, 0x91, 0xfc, 0xa0, 0x0e, 0xb0, 0x12, 0x20,
    0x0f, 0x71, 0xde, 0xa0, 0x10, 0x99, 0x2e, 0xa0, 0x11, 0x51, 0xc0, 0xa0,
    0x12, 0x79, 0x10, 0xa0, 0x13, 0x31, 0xa2, 0xa0, 0x14, 0x58, 0xf2, 0xa0,
    0x15, 0x23, 0xeb, 0x90, 0x16, 0x38, 0xc6, 0x90, 0x17, 0x03, 0xcd, 0x90,
    0x18, 0x18, 0xa8, 0x90, 0x18, 0xe3, 0xaf, 0x90, 0x19, 0xf8, 0x8a, 0x90,
    0x1a, 0xc3, 0x91, 0x90, 0x1b, 0xe1, 0xa7, 0x10, 0x1c, 0xac, 0xae, 0x10,
    0x1d, 0xc1, 0x89, 0x10, 0x1e, 0x8c, 0x90, 0x10, 0x1f, 0xa1, 0x6b, 0x10,
    0x20, 0x6c, 0x72, 0x10, 0x21, 0x81, 0x4d, 0x10, 0x22, 0x4c, 0x54, 0x10,
    0x23, 0x61, 0x2f, 0x10, 0x24, 0x2c, 0x36, 0x10, 0x25, 0x4a, 0x4b, 0x90,
    0x26, 0x0c, 0x18, 0x10, 0x27, 0x2a, 0x2d, 0x90, 0x27, 0xf5, 0x34, 0x90,
    0x29, 0x0a, 0x0f, 0x90, 0x29, 0xd5, 0x16, 0x90, 0x2a, 0xe9, 0xf1, 0x90,
    0x2b, 0xb4, 0xf8, 0x90, 0x2c, 0xc9, 0xd3, 0x90, 0x2d, 0x94, 0xda, 0x90,
    0x2e, 0xa9, 0xb5, 0x90, 0x2f, 0x74, 0xbc, 0x90, 0x30, 0x89, 0x97, 0x90,
    0x30, 0xe7, 0x24, 0x00, 0x31, 0x5d, 0xd9, 0x10, 0x32, 0x72, 0xb4, 0x10,
    0x33, 0x3d, 0xbb, 0x10, 0x34, 0x52, 0x96, 0x10, 0x35, 0x1d, 0x9d, 0x10,
    0x36, 0x32, 0x78, 0x10, 0x36, 0xfd, 0x7f, 0x10, 0x38, 0x1b, 0x94, 0x90,
    0x38, 0xdd, 0x61, 0x10, 0x39, 0xfb, 0x76, 0x90, 0x3a, 0xbd, 0x43, 0x10,
    0x3b, 0xdb, 0x58, 0x90, 0x3c, 0xa6, 0x5f, 0x90, 0x3d, 0xbb, 0x3a, 0x90,
    0x3e, 0x86, 0x41, 0x90, 0x3f, 0x9b, 0x1c, 0x90, 0x40, 0x66, 0x23, 0x90,
    0x41, 0x84, 0x39, 0x10, 0x42, 0x46, 0x05, 0x90, 0x43, 0x64, 0x1b, 0x10,
    0x44, 0x25, 0xe7, 0x90, 0x45, 0x43, 0xfd, 0x10, 0x46, 0x05, 0xc9, 0x90,
    0x47, 0x23, 0xdf, 0x10, 0x47, 0xee, 0xe6, 0x10, 0x49, 0x03, 0xc1, 0x10,
    0x49, 0xce, 0xc8, 0x10, 0x4a, 0xe3, 0xa3, 0x10, 0x4b, 0xae, 0xaa, 0x10,
    0x4c, 0xcc, 0xbf, 0x90, 0x4d, 0x8e, 0x8c, 0x10, 0x4e, 0xac, 0xa1, 0x90,
    0x4f, 0x6e, 0x6e, 0x10, 0x50, 0x8c, 0x83, 0x90, 0x51, 0x57, 0x8a, 0x90,
    0x52, 0x6c, 0x65, 0x90, 0x53, 0x37, 0x6c, 0x90, 0x54, 0x4c, 0x47, 0x90,
    0x55, 0x17, 0x4e, 0x90, 0x56, 0x2c, 0x29, 0x90, 0x56, 0xf7, 0x30, 0x90,
    0x58, 0x15, 0x46, 0x10, 0x58, 0xd7, 0x12, 0x90, 0x59, 0xf5, 0x28, 0x10,
    0x5a, 0xb6, 0xf4, 0x90, 0x5b, 0xd5, 0x0a, 0x10, 0x5c, 0xa0, 0x11, 0x10,
    0x5d, 0xb4, 0xec, 0x10, 0x5e, 0x7f, 0xf3, 0x10, 0x5f, 0x94, 0xce, 0x10,
    0x60, 0x5f, 0xd5, 0x10, 0x61, 0x7d, 0xea, 0x90, 0x62, 0x3f, 0xb7, 0x10,
    0x63, 0x5d, 0xcc, 0x90, 0x64, 0x1f, 0x99, 0x10, 0x65, 0x3d, 0xae, 0x90,
    0x66, 0x08, 0xb5, 0x90, 0x67, 0x1d, 0x90, 0x90, 0x67, 0xe8, 0x97, 0x90,
    0x68, 0xfd, 0x72, 0x90, 0x69, 0xc8, 0x79, 0x90, 0x6a, 0xdd, 0x54, 0x90,
    0x6b, 0xa8, 0x5b, 0x90, 0x6c, 0xc6, 0x71, 0x10, 0x6d, 0x88, 0x3d, 0x90,
    0x6e, 0xa6, 0x53, 0x10, 0x6f, 0x68, 0x1f, 0x90, 0x70, 0x86, 0x35, 0x10,
    0x71, 0x51, 0x3c, 0x10, 0x72, 0x66, 0x17, 0x10, 0x73, 0x31, 0x1e, 0x10,
    0x74, 0x45, 0xf9, 0x10, 0x75, 0x11, 0x00, 0x10, 0x76, 0x2f, 0x15, 0x90,
    0x76, 0xf0, 0xe2, 0x10, 0x78, 0x0e, 0xf7, 0x90, 0x78, 0xd0, 0xc4, 0x10,
    0x79, 0xee, 0xd9, 0x90, 0x7a, 0xb0, 0xa6, 0x10, 0x7b, 0xce, 0xbb, 0x90,
    0x7c, 0x99, 0xc2, 0x90, 0x7d, 0xae, 0x9d, 0x90, 0x7e, 0x79, 0xa4, 0x90,
    0x7f, 0x8e, 0x7f, 0x90, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02,
    0x00, 0x02, 0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x03, 0x05, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x04,
    0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04,
    0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04,
    0x05, 0x04, 0x05, 0x04, 0x05, 0x06, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x00, 0x00, 0x0e, 0x10, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x1c, 0x20, 0x01, 0x08,
    0x00, 0x00, 0x0e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x10, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
    0x42, 0x53, 0x54, 0x00, 0x47, 0x4d, 0x54, 0x00, 0x42, 0x44, 0x53, 0x54,
    0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x00, 0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xf3, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x11, 0xff,
    0xff, 0xff, 0xff, 0x1a, 0x5d, 0x09, 0xcb, 0xff, 0xff, 0xff, 0xff, 0x9b,
    0x26, 0xad, 0xa0, 0xff, 0xff, 0xff, 0xff, 0x9b, 0xd6, 0x05, 0x20, 0xff,
    0xff, 0xff, 0xff, 0x9c, 0xcf, 0x30, 0xa0, 0xff, 0xff, 0xff, 0xff, 0x9d,
    0xa4, 0xc3, 0xa0, 0xff, 0xff, 0xff, 0xff, 0x9e, 0x9c, 0x9d, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0x9f, 0x97, 0x1a, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xa0,
    0x85, 0xba, 0x20, 0xff, 0xff, 0xff, 0xff, 0xa1, 0x76, 0xfc, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0xa2, 0x65, 0x9c, 0x20, 0xff, 0xff, 0xff, 0xff, 0xa3,
    0x7b, 0xc8, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xa4, 0x4e, 0xb8, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0xa5, 0x3f, 0xfb, 0x20, 0xff, 0xff, 0xff, 0xff, 0xa6,
    0x25, 0x60, 0x20, 0xff, 0xff, 0xff, 0xff, 0xa7, 0x27, 0xc6, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xa8, 0x2a, 0x2c, 0x20, 0xff, 0xff, 0xff, 0xff, 0xa8,
    0xeb, 0xf8, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xaa, 0x00, 0xd3, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0xaa, 0xd5, 0x15, 0x20, 0xff, 0xff, 0xff, 0xff, 0xab,
    0xe9, 0xf0, 0x20, 0xff, 0xff, 0xff, 0xff, 0xac, 0xc7, 0x6c, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xad, 0xc9, 0xd2, 0x20, 0xff, 0xff, 0xff, 0xff, 0xae,
    0xa7, 0x4e, 0x20, 0xff, 0xff, 0xff, 0xff, 0xaf, 0xa0, 0x79, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0xb0, 0x87, 0x30, 0x20, 0xff, 0xff, 0xff, 0xff, 0xb1,
    0x92, 0xd0, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xb2, 0x70, 0x4c, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0xb3, 0x72, 0xb2, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xb4,
    0x50, 0x2e, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xb5, 0x49, 0x5a, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xb6, 0x30, 0x10, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xb7,
    0x32, 0x76, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xb8, 0x0f, 0xf2, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0xb9, 0x12, 0x58, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xb9,
    0xef, 0xd4, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xba, 0xe9, 0x00, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xbb, 0xd8, 0xf1, 0x20, 0xff, 0xff, 0xff, 0xff, 0xbc,
    0xdb, 0x57, 0x20, 0xff, 0xff, 0xff, 0xff, 0xbd, 0xb8, 0xd3, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xbe, 0xb1, 0xfe, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xbf,
    0x98, 0xb5, 0x20, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x9b, 0x1b, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xc1, 0x78, 0x97, 0x20, 0xff, 0xff, 0xff, 0xff, 0xc2,
    0x7a, 0xfd, 0x20, 0xff, 0xff, 0xff, 0xff, 0xc3, 0x58, 0x79, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xc4, 0x51, 0xa4, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xc5,
    0x38, 0x5b, 0x20, 0xff, 0xff, 0xff, 0xff, 0xc6, 0x3a, 0xc1, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xc7, 0x58, 0xd6, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xc7,
    0xda, 0x09, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xca, 0x16, 0x26, 0x90, 0xff,
    0xff, 0xff, 0xff, 0xca, 0x97, 0x59, 0x90, 0xff, 0xff, 0xff, 0xff, 0xcb,
    0xd1, 0x1e, 0x90, 0xff, 0xff, 0xff, 0xff, 0xcc, 0x77, 0x3b, 0x90, 0xff,
    0xff, 0xff, 0xff, 0xcd, 0xb1, 0x00, 0x90, 0xff, 0xff, 0xff, 0xff, 0xce,
    0x60, 0x58, 0x10, 0xff, 0xff, 0xff, 0xff, 0xcf, 0x90, 0xe2, 0x90, 0xff,
    0xff, 0xff, 0xff, 0xd0, 0x6e, 0x5e, 0x90, 0xff, 0xff, 0xff, 0xff, 0xd1,
    0x72, 0x16, 0x10, 0xff, 0xff, 0xff, 0xff, 0xd1, 0xfb, 0x32, 0x10, 0xff,
    0xff, 0xff, 0xff, 0xd2, 0x69, 0xfe, 0x20, 0xff, 0xff, 0xff, 0xff, 0xd3,
    0x63, 0x29, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xd4, 0x49, 0xe0, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xd5, 0x1e, 0x21, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xd5,
    0x42, 0xfd, 0x90, 0xff, 0xff, 0xff, 0xff, 0xd5, 0xdf, 0xe0, 0x10, 0xff,
    0xff, 0xff, 0xff, 0xd6, 0x4e, 0xac, 0x20, 0xff, 0xff, 0xff, 0xff, 0xd6,
    0xfe, 0x03, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xd8, 0x2e, 0x8e, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xd8, 0xf9, 0x95, 0x20, 0xff, 0xff, 0xff, 0xff, 0xda,
    0x0e, 0x70, 0x20, 0xff, 0xff, 0xff, 0xff, 0xda, 0xeb, 0xec, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xdb, 0xe5, 0x17, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xdc,
    0xcb, 0xce, 0x20, 0xff, 0xff, 0xff, 0xff, 0xdd, 0xc4, 0xf9, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0xde, 0xb4, 0xea, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xdf,
    0xae, 0x16, 0x20, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x94, 0xcc, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0xe1, 0x72, 0x48, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xe2,
    0x6b, 0x74, 0x20, 0xff, 0xff, 0xff, 0xff, 0xe3, 0x52, 0x2a, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0xe4, 0x54, 0x90, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xe5,
    0x32, 0x0c, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xe6, 0x3d, 0xad, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xe7, 0x1b, 0x29, 0x20, 0xff, 0xff, 0xff, 0xff, 0xe8,
    0x14, 0x54, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xe8, 0xfb, 0x0b, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xe9, 0xfd, 0x71, 0x20, 0xff, 0xff, 0xff, 0xff, 0xea,
    0xda, 0xed, 0x20, 0xff, 0xff, 0xff, 0xff, 0xeb, 0xdd, 0x53, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xec, 0xba, 0xcf, 0x20, 0xff, 0xff, 0xff, 0xff, 0xed,
    0xb3, 0xfa, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xee, 0x9a, 0xb1, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xef, 0x81, 0x67, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xf0,
    0x9f, 0x7d, 0x20, 0xff, 0xff, 0xff, 0xff, 0xf1, 0x61, 0x49, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0xf2, 0x7f, 0x5f, 0x20, 0xff, 0xff, 0xff, 0xff, 0xf3,
    0x4a, 0x66, 0x20, 0xff, 0xff, 0xff, 0xff, 0xf4, 0x5f, 0x41, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xf5, 0x21, 0x0d, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xf6,
    0x3f, 0x23, 0x20, 0xff, 0xff, 0xff, 0xff, 0xf7, 0x00, 0xef, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0xf8, 0x1f, 0x05, 0x20, 0xff, 0xff, 0xff, 0xff, 0xf8,
    0xe0, 0xd1, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xf9, 0xfe, 0xe7, 0x20, 0xff,
    0xff, 0xff, 0xff, 0xfa, 0xc0, 0xb3, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xfb,
    0xe8, 0x03, 0xa0, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x7b, 0xab, 0xa0, 0xff,
    0xff, 0xff, 0xff, 0xfd, 0xc7, 0xbb, 0x70, 0x00, 0x00, 0x00, 0x00, 0x03,
    0x70, 0xc6, 0x20, 0x00, 0x00, 0x00, 0x00, 0x04, 0x29, 0x58, 0x20, 0x00,
    0x00, 0x00, 0x00, 0x05, 0x50, 0xa8, 0x20, 0x00, 0x00, 0x00, 0x00, 0x06,
    0x09, 0x3a, 0x20, 0x00, 0x00, 0x00, 0x00, 0x07, 0x30, 0x8a, 0x20, 0x00,
    0x00, 0x00, 0x00, 0x07, 0xe9, 0x1c, 0x20, 0x00, 0x00, 0x00, 0x00, 0x09,
    0x10, 0x6c, 0x20, 0x00, 0x00, 0x00, 0x00, 0x09, 0xc8, 0xfe, 0x20, 0x00,
    0x00, 0x00, 0x00, 0x0a, 0xf0, 0x4e, 0x20, 0x00, 0x00, 0x00, 0x00, 0x0b,
    0xb2, 0x1a, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xd0, 0x30, 0x20, 0x00,
    0x00, 0x00, 0x00, 0x0d, 0x91, 0xfc, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x0e,
    0xb0, 0x12, 0x20, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x71, 0xde, 0xa0, 0x00,
    0x00, 0x00, 0x00, 0x10, 0x99, 0x2e, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x11,
    0x51, 0xc0, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x12, 0x79, 0x10, 0xa0, 0x00,
    0x00, 0x00, 0x00, 0x13, 0x31, 0xa2, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x14,
    0x58, 0xf2, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x15, 0x23, 0xeb, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x16, 0x38, 0xc6, 0x90, 0x00, 0x00, 0x00, 0x00, 0x17,
    0x03, 0xcd, 0x90, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0xa8, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x18, 0xe3, 0xaf, 0x90, 0x00, 0x00, 0x00, 0x00, 0x19,
    0xf8, 0x8a, 0x90, 0x00, 0x00, 0x00, 0x00, 0x1a, 0xc3, 0x91, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x1b, 0xe1, 0xa7, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1c,
    0xac, 0xae, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1d, 0xc1, 0x89, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x1e, 0x8c, 0x90, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1f,
    0xa1, 0x6b, 0x10, 0x00, 0x00, 0x00, 0x00, 0x20, 0x6c, 0x72, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x21, 0x81, 0x4d, 0x10, 0x00, 0x00, 0x00, 0x00, 0x22,
    0x4c, 0x54, 0x10, 0x00, 0x00, 0x00, 0x00, 0x23, 0x61, 0x2f, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x24, 0x2c, 0x36, 0x10, 0x00, 0x00, 0x00, 0x00, 0x25,
    0x4a, 0x4b, 0x90, 0x00, 0x00, 0x00, 0x00, 0x26, 0x0c, 0x18, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x27, 0x2a, 0x2d, 0x90, 0x00, 0x00, 0x00, 0x00, 0x27,
    0xf5, 0x34, 0x90, 0x00, 0x00, 0x00, 0x00, 0x29, 0x0a, 0x0f, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x29, 0xd5, 0x16, 0x90, 0x00, 0x00, 0x00, 0x00, 0x2a,
    0xe9, 0xf1, 0x90, 0x00, 0x00, 0x00, 0x00, 0x2b, 0xb4, 0xf8, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x2c, 0xc9, 0xd3, 0x90, 0x00, 0x00, 0x00, 0x00, 0x2d,
    0x94, 0xda, 0x90, 0x00, 0x00, 0x00, 0x00, 0x2e, 0xa9, 0xb5, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x2f, 0x74, 0xbc, 0x90, 0x00, 0x00, 0x00, 0x00, 0x30,
    0x89, 0x97, 0x90, 0x00, 0x00, 0x00, 0x00, 0x30, 0xe7, 0x24, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x31, 0x5d, 0xd9, 0x10, 0x00, 0x00, 0x00, 0x00, 0x32,
    0x72, 0xb4, 0x10, 0x00, 0x00, 0x00, 0x00, 0x33, 0x3d, 0xbb, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x34, 0x52, 0x96, 0x10, 0x00, 0x00, 0x00, 0x00, 0x35,
    0x1d, 0x9d, 0x10, 0x00, 0x00, 0x00, 0x00, 0x36, 0x32, 0x78, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x36, 0xfd, 0x7f, 0x10, 0x00, 0x00, 0x00, 0x00, 0x38,
    0x1b, 0x94, 0x90, 0x00, 0x00, 0x00, 0x00, 0x38, 0xdd, 0x61, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x39, 0xfb, 0x76, 0x90, 0x00, 0x00, 0x00, 0x00, 0x3a,
    0xbd, 0x43, 0x10, 0x00, 0x00, 0x00, 0x00, 0x3b, 0xdb, 0x58, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x3c, 0xa6, 0x5f, 0x90, 0x00, 0x00, 0x00, 0x00, 0x3d,
    0xbb, 0x3a, 0x90, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x86, 0x41, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x3f, 0x9b, 0x1c, 0x90, 0x00, 0x00, 0x00, 0x00, 0x40,
    0x66, 0x23, 0x90, 0x00, 0x00, 0x00, 0x00, 0x41, 0x84, 0x39, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x42, 0x46, 0x05, 0x90, 0x00, 0x00, 0x00, 0x00, 0x43,
    0x64, 0x1b, 0x10, 0x00, 0x00, 0x00, 0x00, 0x44, 0x25, 0xe7, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x45, 0x43, 0xfd, 0x10, 0x00, 0x00, 0x00, 0x00, 0x46,
    0x05, 0xc9, 0x90, 0x00, 0x00, 0x00, 0x00, 0x47, 0x23, 0xdf, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x47, 0xee, 0xe6, 0x10, 0x00, 0x00, 0x00, 0x00, 0x49,
    0x03, 0xc1, 0x10, 0x00, 0x00, 0x00, 0x00, 0x49, 0xce, 0xc8, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x4a, 0xe3, 0xa3, 0x10, 0x00, 0x00, 0x00, 0x00, 0x4b,
    0xae, 0xaa, 0x10, 0x00, 0x00, 0x00, 0x00, 0x4c, 0xcc, 0xbf, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x4d, 0x8e, 0x8c, 0x10, 0x00, 0x00, 0x00, 0x00, 0x4e,
    0xac, 0xa1, 0x90, 0x00, 0x00, 0x00, 0x00, 0x4f, 0x6e, 0x6e, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x50, 0x8c, 0x83, 0x90, 0x00, 0x00, 0x00, 0x00, 0x51,
    0x57, 0x8a, 0x90, 0x00, 0x00, 0x00, 0x00, 0x52, 0x6c, 0x65, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x53, 0x37, 0x6c, 0x90, 0x00, 0x00, 0x00, 0x00, 0x54,
    0x4c, 0x47, 0x90, 0x00, 0x00, 0x00, 0x00, 0x55, 0x17, 0x4e, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x56, 0x2c, 0x29, 0x90, 0x00, 0x00, 0x00, 0x00, 0x56,
    0xf7, 0x30, 0x90, 0x00, 0x00, 0x00, 0x00, 0x58, 0x15, 0x46, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x58, 0xd7, 0x12, 0x90, 0x00, 0x00, 0x00, 0x00, 0x59,
    0xf5, 0x28, 0x10, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xb6, 0xf4, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x5b, 0xd5, 0x0a, 0x10, 0x00, 0x00, 0x00, 0x00, 0x5c,
    0xa0, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x5d, 0xb4, 0xec, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x5e, 0x7f, 0xf3, 0x10, 0x00, 0x00, 0x00, 0x00, 0x5f,
    0x94, 0xce, 0x10, 0x00, 0x00, 0x00, 0x00, 0x60, 0x5f, 0xd5, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x61, 0x7d, 0xea, 0x90, 0x00, 0x00, 0x00, 0x00, 0x62,
    0x3f, 0xb7, 0x10, 0x00, 0x00, 0x00, 0x00, 0x63, 0x5d, 0xcc, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x64, 0x1f, 0x99, 0x10, 0x00, 0x00, 0x00, 0x00, 0x65,
    0x3d, 0xae, 0x90, 0x00, 0x00, 0x00, 0x00, 0x66, 0x08, 0xb5, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x67, 0x1d, 0x90, 0x90, 0x00, 0x00, 0x00, 0x00, 0x67,
    0xe8, 0x97, 0x90, 0x00, 0x00, 0x00, 0x00, 0x68, 0xfd, 0x72, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x69, 0xc8, 0x79, 0x90, 0x00, 0x00, 0x00, 0x00, 0x6a,
    0xdd, 0x54, 0x90, 0x00, 0x00, 0x00, 0x00, 0x6b, 0xa8, 0x5b, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x6c, 0xc6, 0x71, 0x10, 0x00, 0x00, 0x00, 0x00, 0x6d,
    0x88, 0x3d, 0x90, 0x00, 0x00, 0x00, 0x00, 0x6e, 0xa6, 0x53, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x6f, 0x68, 0x1f, 0x90, 0x00, 0x00, 0x00, 0x00, 0x70,
    0x86, 0x35, 0x10, 0x00, 0x00, 0x00, 0x00, 0x71, 0x51, 0x3c, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x72, 0x66, 0x17, 0x10, 0x00, 0x00, 0x00, 0x00, 0x73,
    0x31, 0x1e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x74, 0x45, 0xf9, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x75, 0x11, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x76,
    0x2f, 0x15, 0x90, 0x00, 0x00, 0x00, 0x00, 0x76, 0xf0, 0xe2, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x78, 0x0e, 0xf7, 0x90, 0x00, 0x00, 0x00, 0x00, 0x78,
    0xd0, 0xc4, 0x10, 0x00, 0x00, 0x00, 0x00, 0x79, 0xee, 0xd9, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x7a, 0xb0, 0xa6, 0x10, 0x00, 0x00, 0x00, 0x00, 0x7b,
    0xce, 0xbb, 0x90, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x99, 0xc2, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x7d, 0xae, 0x9d, 0x90, 0x00, 0x00, 0x00, 0x00, 0x7e,
    0x79, 0xa4, 0x90, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x8e, 0x7f, 0x90, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x03, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x03, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x04,
    0x06, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x05, 0x06, 0x05, 0x06, 0x05,
    0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05,
    0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05,
    0x06, 0x07, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06,
    0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06,
    0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06,
    0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06,
    0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06,
    0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06,
    0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06, 0x05, 0x06,
    0x05, 0x06, 0xff, 0xff, 0xff, 0xb5, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x10,
    0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x1c, 0x20,
    0x01, 0x0c, 0x00, 0x00, 0x0e, 0x10, 0x00, 0x04, 0x00, 0x00, 0x0e, 0x10,
    0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x08, 0x4c, 0x4d, 0x54, 0x00, 0x42, 0x53, 0x54, 0x00, 0x47, 0x4d,
    0x54, 0x00, 0x42, 0x44, 0x53, 0x54, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00,
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x0a,
    0x47, 0x4d, 0x54, 0x30, 0x42, 0x53, 0x54, 0x2c, 0x4d, 0x33, 0x2e, 0x35,
    0x2e, 0x30, 0x2f, 0x31, 0x2c, 0x4d, 0x31, 0x30, 0x2e, 0x35, 0x2e, 0x30,
    0x0a,
};

const unsigned char TOKYO_DATA[] = {
    // Data from Asia/Tokyo

    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
    0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09,
    0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0c, 0xc3, 0xce, 0x85, 0x70,
    0xd7, 0x3e, 0x1e, 0x90, 0xd7, 0xec, 0x16, 0x80, 0xd8, 0xf9, 0x16, 0x90,
    0xd9, 0xcb, 0xf8, 0x80, 0xdb, 0x07, 0x1d, 0x10, 0xdb, 0xab, 0xda, 0x80,
    0xdc, 0xe6, 0xff, 0x10, 0xdd, 0x8b, 0xbc, 0x80, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x00, 0x00, 0x7e, 0x90, 0x00, 0x00, 0x00,
    0x00, 0x8c, 0xa0, 0x01, 0x04, 0x00, 0x00, 0x7e, 0x90, 0x00, 0x08, 0x43,
    0x4a, 0x54, 0x00, 0x4a, 0x44, 0x54, 0x00, 0x4a, 0x53, 0x54, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
    0x10, 0xff, 0xff, 0xff, 0xff, 0x65, 0xc2, 0xa4, 0x70, 0xff, 0xff, 0xff,
    0xff, 0x74, 0xce, 0xe3, 0x70, 0xff, 0xff, 0xff, 0xff, 0xc3, 0xce, 0x85,
    0x70, 0xff, 0xff, 0xff, 0xff, 0xd7, 0x3e, 0x1e, 0x90, 0xff, 0xff, 0xff,
    0xff, 0xd7, 0xec, 0x16, 0x80, 0xff, 0xff, 0xff, 0xff, 0xd8, 0xf9, 0x16,
    0x90, 0xff, 0xff, 0xff, 0xff, 0xd9, 0xcb, 0xf8, 0x80, 0xff, 0xff, 0xff,
    0xff, 0xdb, 0x07, 0x1d, 0x10, 0xff, 0xff, 0xff, 0xff, 0xdb, 0xab, 0xda,
    0x80, 0xff, 0xff, 0xff, 0xff, 0xdc, 0xe6, 0xff, 0x10, 0xff, 0xff, 0xff,
    0xff, 0xdd, 0x8b, 0xbc, 0x80, 0x01, 0x02, 0x04, 0x03, 0x04, 0x03, 0x04,
    0x03, 0x04, 0x03, 0x04, 0x00, 0x00, 0x83, 0x03, 0x00, 0x00, 0x00, 0x00,
    0x7e, 0x90, 0x00, 0x04, 0x00, 0x00, 0x7e, 0x90, 0x00, 0x08, 0x00, 0x00,
    0x8c, 0xa0, 0x01, 0x0c, 0x00, 0x00, 0x7e, 0x90, 0x00, 0x04, 0x4c, 0x4d,
    0x54, 0x00, 0x4a, 0x53, 0x54, 0x00, 0x43, 0x4a, 0x54, 0x00, 0x4a, 0x44,
    0x54, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x0a, 0x4a, 0x53, 0x54, 0x2d, 0x39, 0x0a,
};

const unsigned char KIRITIMATI_DATA[] = {
    // Data from Pacific/Kiritimati

    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
    0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0x12, 0x55, 0xf2, 0x00,
    0x2f, 0x06, 0x7d, 0x20, 0x01, 0x02, 0xff, 0xff, 0x6a, 0x00, 0x00, 0x00,
    0xff, 0xff, 0x73, 0x60, 0x00, 0x00, 0x00, 0x00, 0xc4, 0xe0, 0x00, 0x00,
    0x4c, 0x49, 0x4e, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54,
    0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
    0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00,
    0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x09, 0xff, 0xff, 0xff, 0xff, 0x7e,
    0x37, 0x48, 0x80, 0x00, 0x00, 0x00, 0x00, 0x12, 0x55, 0xf2, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x2f, 0x06, 0x7d, 0x20, 0x01, 0x02, 0x03, 0xff, 0xff,
    0x6c, 0x80, 0x00, 0x00, 0xff, 0xff, 0x6a, 0x00, 0x00, 0x04, 0xff, 0xff,
    0x73, 0x60, 0x00, 0x04, 0x00, 0x00, 0xc4, 0xe0, 0x00, 0x04, 0x4c, 0x4d,
    0x54, 0x00, 0x4c, 0x49, 0x4e, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0a, 0x4c, 0x49, 0x4e, 0x54, 0x2d, 0x31, 0x34, 0x0a,
};

const unsigned char CHATHAM_DATA[] = {
    //Data from Pacific/Catham

    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x10, 0xe7, 0x8b, 0xc2, 0x04,
    0x09, 0x18, 0xfd, 0xe0, 0x09, 0xac, 0xa5, 0xe0, 0x0a, 0xef, 0xa5, 0x60,
    0x0b, 0x9e, 0xfc, 0xe0, 0x0c, 0xd8, 0xc1, 0xe0, 0x0d, 0x7e, 0xde, 0xe0,
    0x0e, 0xb8, 0xa3, 0xe0, 0x0f, 0x5e, 0xc0, 0xe0, 0x10, 0x98, 0x85, 0xe0,
    0x11, 0x3e, 0xa2, 0xe0, 0x12, 0x78, 0x67, 0xe0, 0x13, 0x1e, 0x84, 0xe0,
    0x14, 0x58, 0x49, 0xe0, 0x14, 0xfe, 0x66, 0xe0, 0x16, 0x38, 0x2b, 0xe0,
    0x16, 0xe7, 0x83, 0x60, 0x18, 0x21, 0x48, 0x60, 0x18, 0xc7, 0x65, 0x60,
    0x1a, 0x01, 0x2a, 0x60, 0x1a, 0xa7, 0x47, 0x60, 0x1b, 0xe1, 0x0c, 0x60,
    0x1c, 0x87, 0x29, 0x60, 0x1d, 0xc0, 0xee, 0x60, 0x1e, 0x67, 0x0b, 0x60,
    0x1f, 0xa0, 0xd0, 0x60, 0x20, 0x46, 0xed, 0x60, 0x21, 0x80, 0xb2, 0x60,
    0x22, 0x30, 0x09, 0xe0, 0x23, 0x69, 0xce, 0xe0, 0x24, 0x0f, 0xeb, 0xe0,
    0x25, 0x2e, 0x01, 0x60, 0x26, 0x02, 0x42, 0xe0, 0x27, 0x0d, 0xe3, 0x60,
    0x27, 0xe2, 0x24, 0xe0, 0x28, 0xed, 0xc5, 0x60, 0x29, 0xc2, 0x06, 0xe0,
    0x2a, 0xcd, 0xa7, 0x60, 0x2b, 0xab, 0x23, 0x60, 0x2c, 0xad, 0x89, 0x60,
    0x2d, 0x8b, 0x05, 0x60, 0x2e, 0x8d, 0x6b, 0x60, 0x2f, 0x6a, 0xe7, 0x60,
    0x30, 0x6d, 0x4d, 0x60, 0x31, 0x4a, 0xc9, 0x60, 0x32, 0x56, 0x69, 0xe0,
    0x33, 0x2a, 0xab, 0x60, 0x34, 0x36, 0x4b, 0xe0, 0x35, 0x0a, 0x8d, 0x60,
    0x36, 0x16, 0x2d, 0xe0, 0x36, 0xf3, 0xa9, 0xe0, 0x37, 0xf6, 0x0f, 0xe0,
    0x38, 0xd3, 0x8b, 0xe0, 0x39, 0xd5, 0xf1, 0xe0, 0x3a, 0xb3, 0x6d, 0xe0,
    0x3b, 0xbf, 0x0e, 0x60, 0x3c, 0x93, 0x4f, 0xe0, 0x3d, 0x9e, 0xf0, 0x60,
    0x3e, 0x73, 0x31, 0xe0, 0x3f, 0x7e, 0xd2, 0x60, 0x40, 0x5c, 0x4e, 0x60,
    0x41, 0x5e, 0xb4, 0x60, 0x42, 0x3c, 0x30, 0x60, 0x43, 0x3e, 0x96, 0x60,
    0x44, 0x1c, 0x12, 0x60, 0x45, 0x1e, 0x78, 0x60, 0x45, 0xfb, 0xf4, 0x60,
    0x46, 0xfe, 0x5a, 0x60, 0x47, 0xf7, 0x85, 0xe0, 0x48, 0xde, 0x3c, 0x60,
    0x49, 0xd7, 0x67, 0xe0, 0x4a, 0xbe, 0x1e, 0x60, 0x4b, 0xb7, 0x49, 0xe0,
    0x4c, 0x9e, 0x00, 0x60, 0x4d, 0x97, 0x2b, 0xe0, 0x4e, 0x7d, 0xe2, 0x60,
    0x4f, 0x77, 0x0d, 0xe0, 0x50, 0x66, 0xfe, 0xe0, 0x51, 0x60, 0x2a, 0x60,
    0x52, 0x46, 0xe0, 0xe0, 0x53, 0x40, 0x0c, 0x60, 0x54, 0x26, 0xc2, 0xe0,
    0x55, 0x1f, 0xee, 0x60, 0x56, 0x06, 0xa4, 0xe0, 0x56, 0xff, 0xd0, 0x60,
    0x57, 0xe6, 0x86, 0xe0, 0x58, 0xdf, 0xb2, 0x60, 0x59, 0xc6, 0x68, 0xe0,
    0x5a, 0xbf, 0x94, 0x60, 0x5b, 0xaf, 0x85, 0x60, 0x5c, 0xa8, 0xb0, 0xe0,
    0x5d, 0x8f, 0x67, 0x60, 0x5e, 0x88, 0x92, 0xe0, 0x5f, 0x6f, 0x49, 0x60,
    0x60, 0x68, 0x74, 0xe0, 0x61, 0x4f, 0x2b, 0x60, 0x62, 0x48, 0x56, 0xe0,
    0x63, 0x2f, 0x0d, 0x60, 0x64, 0x28, 0x38, 0xe0, 0x65, 0x0e, 0xef, 0x60,
    0x66, 0x11, 0x55, 0x60, 0x66, 0xf8, 0x0b, 0xe0, 0x67, 0xf1, 0x37, 0x60,
    0x68, 0xd7, 0xed, 0xe0, 0x69, 0xd1, 0x19, 0x60, 0x6a, 0xb7, 0xcf, 0xe0,
    0x6b, 0xb0, 0xfb, 0x60, 0x6c, 0x97, 0xb1, 0xe0, 0x6d, 0x90, 0xdd, 0x60,
    0x6e, 0x77, 0x93, 0xe0, 0x6f, 0x70, 0xbf, 0x60, 0x70, 0x60, 0xb0, 0x60,
    0x71, 0x59, 0xdb, 0xe0, 0x72, 0x40, 0x92, 0x60, 0x73, 0x39, 0xbd, 0xe0,
    0x74, 0x20, 0x74, 0x60, 0x75, 0x19, 0x9f, 0xe0, 0x76, 0x00, 0x56, 0x60,
    0x76, 0xf9, 0x81, 0xe0, 0x77, 0xe0, 0x38, 0x60, 0x78, 0xd9, 0x63, 0xe0,
    0x79, 0xc0, 0x1a, 0x60, 0x7a, 0xb9, 0x45, 0xe0, 0x7b, 0xa9, 0x36, 0xe0,
    0x7c, 0xa2, 0x62, 0x60, 0x7d, 0x89, 0x18, 0xe0, 0x7e, 0x82, 0x44, 0x60,
    0x7f, 0x68, 0xfa, 0xe0, 0x03, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x00, 0x00, 0xab, 0xfc, 0x00, 0x00, 0x00, 0x00, 0xc1, 0x5c, 0x01, 0x04,
    0x00, 0x00, 0xb3, 0x4c, 0x00, 0x0a, 0x00, 0x00, 0xb3, 0x4c, 0x00, 0x0a,
    0x4c, 0x4d, 0x54, 0x00, 0x43, 0x48, 0x41, 0x44, 0x54, 0x00, 0x43, 0x48,
    0x41, 0x53, 0x54, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x10, 0xff, 0xff, 0xff, 0xff,
    0xe7, 0x8b, 0xc2, 0x04, 0x00, 0x00, 0x00, 0x00, 0x09, 0x18, 0xfd, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x09, 0xac, 0xa5, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x0a, 0xef, 0xa5, 0x60, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x9e, 0xfc, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x0c, 0xd8, 0xc1, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x0d, 0x7e, 0xde, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0e, 0xb8, 0xa3, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x0f, 0x5e, 0xc0, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x98, 0x85, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x11, 0x3e, 0xa2, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x12, 0x78, 0x67, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x13, 0x1e, 0x84, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x14, 0x58, 0x49, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x14, 0xfe, 0x66, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x16, 0x38, 0x2b, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x16, 0xe7, 0x83, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x18, 0x21, 0x48, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x18, 0xc7, 0x65, 0x60, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x01, 0x2a, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x1a, 0xa7, 0x47, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x1b, 0xe1, 0x0c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x87, 0x29, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x1d, 0xc0, 0xee, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x1e, 0x67, 0x0b, 0x60, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xa0, 0xd0, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x20, 0x46, 0xed, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x21, 0x80, 0xb2, 0x60, 0x00, 0x00, 0x00, 0x00, 0x22, 0x30, 0x09, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x23, 0x69, 0xce, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x24, 0x0f, 0xeb, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x25, 0x2e, 0x01, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x26, 0x02, 0x42, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x27, 0x0d, 0xe3, 0x60, 0x00, 0x00, 0x00, 0x00, 0x27, 0xe2, 0x24, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x28, 0xed, 0xc5, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x29, 0xc2, 0x06, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x2a, 0xcd, 0xa7, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x2b, 0xab, 0x23, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x2c, 0xad, 0x89, 0x60, 0x00, 0x00, 0x00, 0x00, 0x2d, 0x8b, 0x05, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x2e, 0x8d, 0x6b, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x2f, 0x6a, 0xe7, 0x60, 0x00, 0x00, 0x00, 0x00, 0x30, 0x6d, 0x4d, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x31, 0x4a, 0xc9, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x32, 0x56, 0x69, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x33, 0x2a, 0xab, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x34, 0x36, 0x4b, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x35, 0x0a, 0x8d, 0x60, 0x00, 0x00, 0x00, 0x00, 0x36, 0x16, 0x2d, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x36, 0xf3, 0xa9, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x37, 0xf6, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x38, 0xd3, 0x8b, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x39, 0xd5, 0xf1, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x3a, 0xb3, 0x6d, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3b, 0xbf, 0x0e, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x3c, 0x93, 0x4f, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x3d, 0x9e, 0xf0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x73, 0x31, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x3f, 0x7e, 0xd2, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x5c, 0x4e, 0x60, 0x00, 0x00, 0x00, 0x00, 0x41, 0x5e, 0xb4, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x3c, 0x30, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x43, 0x3e, 0x96, 0x60, 0x00, 0x00, 0x00, 0x00, 0x44, 0x1c, 0x12, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x45, 0x1e, 0x78, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x45, 0xfb, 0xf4, 0x60, 0x00, 0x00, 0x00, 0x00, 0x46, 0xfe, 0x5a, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x47, 0xf7, 0x85, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x48, 0xde, 0x3c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x49, 0xd7, 0x67, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x4a, 0xbe, 0x1e, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x4b, 0xb7, 0x49, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x4c, 0x9e, 0x00, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x4d, 0x97, 0x2b, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x4e, 0x7d, 0xe2, 0x60, 0x00, 0x00, 0x00, 0x00, 0x4f, 0x77, 0x0d, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x50, 0x66, 0xfe, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x51, 0x60, 0x2a, 0x60, 0x00, 0x00, 0x00, 0x00, 0x52, 0x46, 0xe0, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x53, 0x40, 0x0c, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x54, 0x26, 0xc2, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x55, 0x1f, 0xee, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x56, 0x06, 0xa4, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x56, 0xff, 0xd0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x57, 0xe6, 0x86, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x58, 0xdf, 0xb2, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x59, 0xc6, 0x68, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xbf, 0x94, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x5b, 0xaf, 0x85, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x5c, 0xa8, 0xb0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x5d, 0x8f, 0x67, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x5e, 0x88, 0x92, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x5f, 0x6f, 0x49, 0x60, 0x00, 0x00, 0x00, 0x00, 0x60, 0x68, 0x74, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x61, 0x4f, 0x2b, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x62, 0x48, 0x56, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x63, 0x2f, 0x0d, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x64, 0x28, 0x38, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x65, 0x0e, 0xef, 0x60, 0x00, 0x00, 0x00, 0x00, 0x66, 0x11, 0x55, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x66, 0xf8, 0x0b, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x67, 0xf1, 0x37, 0x60, 0x00, 0x00, 0x00, 0x00, 0x68, 0xd7, 0xed, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x69, 0xd1, 0x19, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x6a, 0xb7, 0xcf, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x6b, 0xb0, 0xfb, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x6c, 0x97, 0xb1, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x6d, 0x90, 0xdd, 0x60, 0x00, 0x00, 0x00, 0x00, 0x6e, 0x77, 0x93, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x6f, 0x70, 0xbf, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x70, 0x60, 0xb0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x71, 0x59, 0xdb, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x72, 0x40, 0x92, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x73, 0x39, 0xbd, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x74, 0x20, 0x74, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x75, 0x19, 0x9f, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x76, 0x00, 0x56, 0x60, 0x00, 0x00, 0x00, 0x00, 0x76, 0xf9, 0x81, 0xe0,
    0x00, 0x00, 0x00, 0x00, 0x77, 0xe0, 0x38, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x78, 0xd9, 0x63, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x79, 0xc0, 0x1a, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x7a, 0xb9, 0x45, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x7b, 0xa9, 0x36, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x7c, 0xa2, 0x62, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x7d, 0x89, 0x18, 0xe0, 0x00, 0x00, 0x00, 0x00,
    0x7e, 0x82, 0x44, 0x60, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x68, 0xfa, 0xe0,
    0x03, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x00, 0x00, 0xab, 0xfc,
    0x00, 0x00, 0x00, 0x00, 0xc1, 0x5c, 0x01, 0x04, 0x00, 0x00, 0xb3, 0x4c,
    0x00, 0x0a, 0x00, 0x00, 0xb3, 0x4c, 0x00, 0x0a, 0x4c, 0x4d, 0x54, 0x00,
    0x43, 0x48, 0x41, 0x44, 0x54, 0x00, 0x43, 0x48, 0x41, 0x53, 0x54, 0x00,
    0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x43, 0x48, 0x41,
    0x53, 0x54, 0x2d, 0x31, 0x32, 0x3a, 0x34, 0x35, 0x43, 0x48, 0x41, 0x44,
    0x54, 0x2c, 0x4d, 0x39, 0x2e, 0x35, 0x2e, 0x30, 0x2f, 0x32, 0x3a, 0x34,
    0x35, 0x2c, 0x4d, 0x34, 0x2e, 0x31, 0x2e, 0x30, 0x2f, 0x33, 0x3a, 0x34,
    0x35, 0x0a,
};

const unsigned char ASIA_BANGKOK_DATA[] = {
    // Data from Asia/Bangkok.

    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0xa2, 0x6a, 0x67, 0xc4,
    0x01, 0x00, 0x00, 0x5e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x62, 0x70, 0x00,
    0x04, 0x42, 0x4d, 0x54, 0x00, 0x49, 0x43, 0x54, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0c, 0xff, 0xff, 0xff,
    0xff, 0x56, 0xb6, 0x85, 0xc4, 0xff, 0xff, 0xff, 0xff, 0xa2, 0x6a, 0x67,
    0xc4, 0x01, 0x02, 0x00, 0x00, 0x5e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x5e,
    0x3c, 0x00, 0x04, 0x00, 0x00, 0x62, 0x70, 0x00, 0x08, 0x4c, 0x4d, 0x54,
    0x00, 0x42, 0x4d, 0x54, 0x00, 0x49, 0x43, 0x54, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0a, 0x49, 0x43, 0x54, 0x2d, 0x37, 0x0a
};

const unsigned char TEST_DATA[] = {
    // Expected version '\0' data for test case 2.

    0x54, 0x5a, 0x69, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char TEST_DATA_VERSION2[] = {
    // Expected version '2' data for test case 2.

    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x0a
};

// TYPEDEFS
typedef baltzo::ZoneinfoBinaryReader Obj;
typedef baltzo::ZoneinfoBinaryHeader BinHeader;
typedef baltzo::LocalTimeDescriptor  Desc;

class RawHeader;
class RawLocalTimeTypes;
class ZoneinfoData;

static int verifyTimeZone(const ZoneinfoData&     data,
                          const baltzo::Zoneinfo& timeZone,
                          int                     line,
                          bool                    expectToFail = false);
    // Validate description of a time zone based on the specified 'timeZone' is
    // the same as the description based on the specified 'data', and output an
    // error message containing the line number indicated by the specified
    // 'line' when the validation failed.  Optionally, specified 'expectToFail'
    // indicating whether there is a difference between 'timeZone' and 'data'.
    // If 'expectToFail' is 'true', 'verifyTimeZone' will not 'ASSERT',
    // otherwise, 'verifyTimeZone' will 'ASSERT' at the location that an error
    // is detected.  The behavior is undefined unless 'data' contains valid
    // Zoneinfo version '\0' binary data in the buffer.  Return 0 if the
    // validation succeed, and a non-zero value otherwise.

static int verifyTimeZoneVersion2Format(
                                 const ZoneinfoData&     data,
                                 const baltzo::Zoneinfo& timeZone,
                                 int                     line,
                                 bool                    expectToFail = false);
    // Validate description of a time zone based on the specified 'timeZone' is
    // the same as the description based on the specified 'data', and output an
    // error message containing the line number indicated by the specified
    // 'line' when the validation failed.  Optionally, specified 'expectToFail'
    // indicating whether there is a difference between 'timeZone' and 'data'.
    // If 'expectToFail' is 'true', 'verifyTimeZone' will not 'ASSERT',
    // otherwise, 'verifyTimeZone' will 'ASSERT' at the location that an error
    // is detected.  The behavior is undefined unless 'data' contains valid
    // Zoneinfo version '2' binary data in the buffer.  Return 0 if the
    // validation succeed, and a non-zero value otherwise.

static void writeBigEndian(char *address, int value);
    // Write the specified 'value' into the memory at the specified 'address in
    // big-endian format.

static void writeBigEndian64(char *address, bsls::Types::Int64 value);
    // Write the specified 'value' into the memory at the specified 'address in
    // big-endian format.

static int readBigEndian(const char *address);
    // Read from the memory at the specified 'address' containing an integer
    // in big-endian format, and return the value of the integer.

static bsls::Types::Int64 readBigEndian64(const char *address);
    // Read from the memory at the specified 'address' containing an integer
    // in big-endian format, and return the value of the integer.

// ============================================================================
//              GLOBAL HELPER CLASSES AND FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------
static void writeBigEndian(char *address, int value)
{
    int hostValue = BSLS_BYTEORDER_HOST_U32_TO_BE(value);
    memcpy(address, &hostValue, sizeof hostValue);
}

// ----------------------------------------------------------------------------

static void writeBigEndian64(char *address, bsls::Types::Int64 value)
{
    bsls::Types::Int64 hostValue = BSLS_BYTEORDER_HOST_U64_TO_BE(value);
    memcpy(address, &hostValue, sizeof hostValue);
}

// ----------------------------------------------------------------------------

static int readBigEndian(const char *address)
{
    int value;
    memcpy(&value, address, sizeof value);
    return BSLS_BYTEORDER_BE_U32_TO_HOST(value);
}

// ----------------------------------------------------------------------------

static bsls::Types::Int64 readBigEndian64(const char *address)
{
    bsls::Types::Int64 value;
    memcpy(&value, address, sizeof value);
    return BSLS_BYTEORDER_BE_U64_TO_HOST(value);
}

                              // ===============
                              // class RawHeader
                              // ===============

class RawHeader {
    // The byte sequence of the header of a Zoneinfo binary data format.

    // DATA
    char d_headerId[4];           // must be 'EXPECTED_HEADER_ID'
    char d_version[1];            // must be '\0' or '2' (as of 2005)
    char d_reserved[15];          // unused
    char d_numIsGmt[4];           // number of encoded UTC/local indicators
    char d_numIsStd[4];           // number of encoded standard/wall indicators
    char d_numLeaps[4];           // number of leap info's encoded in the file
    char d_numTransitions[4];     // number of transition times
    char d_numLocalTimeTypes[4];  // number of transition types
    char d_abbrevDataSize[4];     // size of the sequence of abbreviation chars

  public:
    // CREATORS
    RawHeader();
        // Create a Zoneinfo binary header with the properties:
        //..
        // version           == '\0'
        // numIsGmt          == 0
        // numIsStd          == 0
        // numLeaps          == 0
        // numTransitions    == 0
        // numLocalTimeTypes == 1
        // abbrevDataSize    == 1
        //..

    // MANIPULATORS
    void setHeaderId(int index, char value);
        // Set the character at the specified 'index' of the header identifier
        // to the specified 'value'.

    void setVersion(char value);
        // Set the value of the version to the specified 'value'.

    void setNumIsGmt(int value);
        // Set the number of 'isGmt' flag to the specified 'value'.  Note that
        // the value is stored in big-endian format.

    void setNumIsStd(int value);
        // Set the number of 'isStd' flag to the specified 'value'.  Note that
        // the value is stored in big-endian format.

    void setNumLeaps(int value);
        // Set the number of leap corrections to the specified 'value'.  Note
        // that the value is stored in big-endian format.

    void setNumTransitions(int value);
        // Set the number of transitions to the specified 'value'.  Note that
        // the value is stored in big-endian format.

    void setNumLocalTimeTypes(int value);
        // Set the number of local time types to the specified 'value'.  Note
        // that the value is stored in big-endian format.

    void setAbbrevDataSize(int value);
        // Set the size of abbreviation data to the specified 'value'.  Note
        // that the value is stored in big-endian format.

    // ACCESSORS
    char version() const;
        // Return the value of 'version' of this object.

    int numIsGmt() const;
        // Return the value of 'numIsGmt' of this object.

    int numIsStd() const;
        // Return the value of 'numIsStd' of this object.

    int numLeaps() const;
        // Return the value of 'numLeaps' of this object.

    int numTransitions() const;
        // Return the value of 'numTransitions' of this object.

    int numLocalTimeTypes() const;
        // Return the value of 'numLocalTimeTypes' of this object.

    int abbrevDataSize() const;
        // Return the value of 'abbrevDataSize' of this object.

};

BSLMF_ASSERT(44 == sizeof(RawHeader));

                        // ---------------
                        // class RawHeader
                        // ---------------
// CREATORS
RawHeader::RawHeader()
{
    memset(this, 0, sizeof *this);
    d_headerId[0] = 'T';
    d_headerId[1] = 'Z';
    d_headerId[2] = 'i';
    d_headerId[3] = 'f';
    setNumLocalTimeTypes(1);
    setAbbrevDataSize(1);
}

// MANIPULATORS
void RawHeader::setHeaderId(int index, char value)
{
    d_headerId[index] = value;
}

void RawHeader::setVersion(char value)
{
    *d_version = value;
}

void RawHeader::setNumIsGmt(int value)
{
    writeBigEndian(d_numIsGmt, value);
}

void RawHeader::setNumIsStd(int value)
{
    writeBigEndian(d_numIsStd, value);
}

void RawHeader::setNumLeaps(int value)
{
    writeBigEndian(d_numLeaps, value);
}

void RawHeader::setNumTransitions(int value)
{
    writeBigEndian(d_numTransitions, value);
}

void RawHeader::setNumLocalTimeTypes(int value)
{
    writeBigEndian(d_numLocalTimeTypes, value);
}

void RawHeader::setAbbrevDataSize(int value)
{
    writeBigEndian(d_abbrevDataSize, value);
}

// ACCESSORS
char RawHeader::version() const
{
    return *d_version;
}

int RawHeader::numIsGmt() const
{
    return readBigEndian(d_numIsGmt);
}

int RawHeader::numIsStd() const
{
    return readBigEndian(d_numIsStd);
}

int RawHeader::numLeaps() const
{
    return readBigEndian(d_numLeaps);
}

int RawHeader::numTransitions() const
{
    return readBigEndian(d_numTransitions);
}

int RawHeader::numLocalTimeTypes() const
{
    return readBigEndian(d_numLocalTimeTypes);
}

int RawHeader::abbrevDataSize() const
{
    return readBigEndian(d_abbrevDataSize);
}

                          // =======================
                          // class RawLocalTimeTypes
                          // =======================

class RawLocalTimeTypes {
    // The byte sequence of a local-time type in a Zoneinfo binary data format.

    // DATA
    char          d_offset[4];          // UTC offset in number of seconds
    unsigned char d_isDst;              // indicates whether local time is DST
    unsigned char d_abbreviationIndex;  // index to abbreviation string

  public:
    // MANIPULATORS
    void setOffset(int value);
        // Set the value of 'offset' to the specified 'value'.  Note that the
        // value will be stored in big-endian format.

    void setIsDst(unsigned char value);
        // Set the value of 'isDst' flag to the specified 'value'.

    void setAbbreviationIndex(unsigned char value);
        // Set the value of 'abbreviationIndex' to the specified 'value'.

    // ACCESSORS
    int offset() const;
        // Return the value of 'offset' of this object.

    unsigned char isDst() const;
        // Return the value of 'isDst' of this object.

    unsigned char abbreviationIndex() const;
        // Return the value of 'abbreviationIndex' of this object.
};

BSLMF_ASSERT(6 == sizeof(RawLocalTimeTypes));

                          // -----------------------
                          // class RawLocalTimeTypes
                          // -----------------------

// MANIPULATORS
void RawLocalTimeTypes::setOffset(int value)
{
    writeBigEndian(d_offset, value);
}

void RawLocalTimeTypes::setIsDst(unsigned char value)
{
    d_isDst = value;
}

void RawLocalTimeTypes::setAbbreviationIndex(unsigned char value)
{
    d_abbreviationIndex = value;
}

// ACCESSORS
int RawLocalTimeTypes::offset() const
{
    return readBigEndian(d_offset);
}

unsigned char RawLocalTimeTypes::isDst() const
{
    return d_isDst;
}

unsigned char RawLocalTimeTypes::abbreviationIndex() const
{
    return d_abbreviationIndex;
}

                             // =================
                             // class RawLeapInfo
                             // =================

class RawLeapInfo {
    // The byte sequence of a leap correction in a Zoneinfo binary data format.

    // DATA
    char d_transition[4];  // POSIX time at which the leap second occur
    char d_correction[4];  // accumulated leap correction

  public:
    // MANIPULATORS
    void setTransition(int value);
        // Set the value of 'transition' to the specified 'value'.  Note that
        // the value will be stored in big-endian format.

    void setCorrection(int value);
        // Set the value of 'correction' to the specified 'value'.  Note that
        // the value will be stored in big-endian format.

    // ACCESSORS
    int transition() const;
        // Return the value of 'transition' of this object.

    int correction() const;
        // Return the value of 'correction' of this object.
};

                             // -----------------
                             // class RawLeapInfo
                             // -----------------

// MANIPULATORS
void RawLeapInfo::setTransition(int value)
{
    writeBigEndian(d_transition, value);
}

void RawLeapInfo::setCorrection(int value)
{
    writeBigEndian(d_correction, value);
}

// ACCESSORS
int RawLeapInfo::transition() const
{
    return readBigEndian(d_transition);
}

int RawLeapInfo::correction() const
{
    return readBigEndian(d_correction);
}

                            // ===================
                            // class RawLeapInfo64
                            // ===================

class RawLeapInfo64 {
    // The byte sequence of a leap correction in a Zoneinfo binary data format.

    // DATA
    char d_transition[8];  // POSIX time at which the leap second occur
    char d_correction[4];  // accumulated leap correction

  public:
    // MANIPULATORS
    void setTransition(bsls::Types::Int64 value);
        // Set the value of 'transition' to the specified 'value'.  Note that
        // the value will be stored in big-endian format.

    void setCorrection(int value);
        // Set the value of 'correction' to the specified 'value'.  Note that
        // the value will be stored in big-endian format.

    // ACCESSORS
    bsls::Types::Int64 transition() const;
        // Return the value of 'transition' of this object.

    int correction() const;
        // Return the value of 'correction' of this object.
};

                            // -------------------
                            // class RawLeapInfo64
                            // -------------------

// MANIPULATORS
void RawLeapInfo64::setTransition(bsls::Types::Int64 value)
{
    writeBigEndian64(d_transition, value);
}

void RawLeapInfo64::setCorrection(int value)
{
    writeBigEndian(d_correction, value);
}

// ACCESSORS
bsls::Types::Int64 RawLeapInfo64::transition() const
{
    return readBigEndian64(d_transition);
}

int RawLeapInfo64::correction() const
{
    return readBigEndian(d_correction);
}

                             // ==================
                             // class ZoneinfoData
                             // ==================

class ZoneinfoData {
    // This class provides methods to create and store a Zoneinfo binary data
    // format.

    // DATA
    char *d_buffer;  // buffer to store Zoneinfo binary data
    int   d_size;    // size of 'd_buffer'

    // PRIVATE MANIPULATORS
    void populateTransitionTimeBuf();
        // Populate the transition times of Zoneinfo binary data.  A valid
        // header is assumed to exist in 'd_buffer' and the number of
        // transitions can be found in the header.  The transition times starts
        // at 0 and increase by 1 for each subsequent transition.

    void populateTransitionTimeBuf64();
        // Populate the transition times of Zoneinfo binary data.  A valid
        // header is assumed to exist in 'd_buffer' and the number of
        // transitions can be found in the header.  The transition times starts
        // at 0 and increase by 1 for each subsequent transition.

    void populateTransitionIndexBuf();
        // Populate the index to local time types for each transition.  A valid
        // header is assumed to exist in 'd_buffer' and the number of indexes
        // can be found in the header.  The indexes starts at 0, increase by 1
        // for each subsequent transition, and restarts at 0 when the indexes
        // is equal to the number of local time types.

    void populateTransitionIndexBuf64();
        // Populate the index to local time types for each transition.  A valid
        // header is assumed to exist in 'd_buffer' and the number of indexes
        // can be found in the header.  The indexes starts at 0, increase by 1
        // for each subsequent transition, and restarts at 0 when the indexes
        // is equal to the number of local time types.

    void populateLeapCorrectionBuf();
        // Populate the leap correction information of Zoneinfo binary data.  A
        // valid header is assumed to exist in 'd_buffer' and the number of
        // leap corrections can be found in the header.  The leap-correction
        // times starts at 0, increase by 1 for each subsequent correction.
        // Each leap correction increases the accumulated leap correction by 1.

    void populateLeapCorrectionBuf64();
        // Populate the leap correction information of Zoneinfo binary data.  A
        // valid header is assumed to exist in 'd_buffer' and the number of
        // leap corrections can be found in the header.  The leap-correction
        // times starts at 0, increase by 1 for each subsequent correction.
        // Each leap correction increases the accumulated leap correction by 1.

    void populateLocalTimeTypeBuf();
        // Populate the local time type portion of the Zoneinfo binary data.  A
        // valid header is assumed to exist in 'd_buffer' and the number of
        // local-time types can be found in the header.  The first local time
        // type has an offset from UTC of 0 seconds, isDst of 'false' and
        // abbreviation data index of 0.  Subsequent local time type increments
        // the offset and abbreviation data index by 1, and alternate between
        // 'true' and 'false' for isDst settings.

    void populateLocalTimeTypeBuf64();
        // Populate the local time type portion of the Zoneinfo binary data.  A
        // valid header is assumed to exist in 'd_buffer' and the number of
        // local-time types can be found in the header.  The first local time
        // type has an offset from UTC of 0 seconds, isDst of 'false' and
        // abbreviation data index of 0.  Subsequent local time type increments
        // the offset and abbreviation data index by 1, and alternate between
        // 'true' and 'false' for isDst settings.

    void populateAbbreviationData();
        // Populate the abbreviation part of the Zoneinfo binary data.  A valid
        // header is assumed to exist in 'd_buffer' and the abbreviation data
        // size can be found in the header.  The abbreviation data will be
        // filled with '\0' character.

    void populateAbbreviationData64();
        // Populate the abbreviation part of the Zoneinfo binary data.  A valid
        // header is assumed to exist in 'd_buffer' and the abbreviation data
        // size can be found in the header.  The abbreviation data will be
        // filled with '\0' character.

    void populateTimeZoneString();
        // Populate the time zone string part of the Zoneinfo binary data.  It
        // will be filled with two new line characters.

    void populateBuffer(const RawHeader& header);
        // Populate 'd_buffer' with synthetic Zoneinfo binary data that matches
        // the file description of the specified 'header.  This function will
        // call all the other 'populate*' functions to fill the buffer with
        // data.

  public:
    // CREATORS
    ZoneinfoData();
        // Create a 'ZoneinfoData' object with the smallest possible valid
        // Zoneinfo binary data.  This object will have the properties:
        //..
        // version           == '\0'
        // numIsGmt          == 0
        // numIsStd          == 0
        // numLeaps          == 0
        // numTransitions    == 0
        // numLocalTimeTypes == 1
        // abbrevDataSize    == 1
        //..
        // The created object will contain a valid Zoneinfo binary data in the
        // buffer.  The local time type will have an offset from UTC of 0,
        // DST is *not* in effect and an abbreviation index of 0.

    ZoneinfoData(const RawHeader& header);
        // Create a 'ZoneinfoData' object with Zoneinfo binary data that
        // matches the file description supplied by the specified 'header'.
        // The created object will contain a valid Zoneinfo binary data in the
        // buffer.  The buffer describes a time zone with the properties:
        //
        //: o Transition times -- starts from 0 and increment by 1 for each
        //:   subsequent transition.
        //:
        //: o Transition indexes -- starts from 0 and increment by 1 for each
        //:   subsequent transition.  Restart from 0 when the index equals to
        //:   the number of local time types.
        //:
        //: o Leap correction -- the time of leap-correction
        //:   times starts at 0, increase by 1 for each subsequent correction.
        //:   Each leap correction increases the accumulated leap correction by
        //:   1.
        //:
        //: o Abbreviation data -- filled with '\0'.
        //
        // If the 'header' is set to version '2', the Zoneinfo binary data
        // will correctly represent the version '2' file format with both
        // 32-bit and 64-bit transition values.

    ZoneinfoData(const char *data, bsl::size_t size);
        // Create a 'ZoneinfoData' object by duplicating the specified 'data'
        // with the specified 'size' into the buffer.  'data' is assumed to
        // contain Zoneinfo binary data.

    ~ZoneinfoData();
        // Destroy this object.

    // MANIPULATORS
    void setTimeZoneString(const char *data, bsl::size_t size);
        // Set time zone string value by duplicating the specified 'data' with
        // the specified 'size' into the buffer.  As origin buffer may not be
        // able to accommodate additional symbols, new buffer of appropriate
        // size is allocated and data from the old one are duplicated before
        // appending time zone string.  Old buffer is deleted afterwards.  Note
        // that this operation has no effect if binary data version is not
        // equal to '2' or '3'.

    // ACCESSORS
    char *buffer() const;
        // Return the address of the buffer containing the Zoneinfo data.

    int size() const;
        // Return the size of the buffer containing the Zoneinfo data.

    char *getVersion2Address() const;
        // Return the address of the portion in the buffer that is the start
        // of the version '2' header.

    RawHeader *getRawHeader() const;
        // Return the address of the portion in the buffer containing header
        // information.

    RawHeader *getRawHeader64() const;
        // Return the address of the portion in the buffer containing version
        // '2' header information.

    char *getTimeZoneString() const;
        // Return the address of the portion in the buffer containing
        // POSIX-TZ-environment-variable-style string for use in handling
        // instants after the last transition time stored in the file (with
        // nothing between the newlines if there is no POSIX representation for
        // such instants.

    bdlb::BigEndianInt32 *getTransitionTime() const;
        // Return the address of the portion in the buffer containing the times
        // of transitions.

    bdlb::BigEndianInt64 *getTransitionTime64() const;
        // Return the address of the portion in the buffer containing the
        // version '2' times of transitions.

    unsigned char *getTransitionIndex() const;
        // Return the address of the portion in the buffer containing the
        // indexes to local time types for each transition.

    unsigned char *getTransitionIndex64() const;
        // Return the address of the portion in the buffer containing the
        // version '2' indexes to local time types for each transition.

    RawLeapInfo *getRawLeapInfo() const;
        // Return the address of the portion in the buffer containing leap
        // correction information.

    RawLeapInfo64 *getRawLeapInfo64() const;
        // Return the address of the portion in the buffer containing version
        // '2' leap correction information.

    RawLocalTimeTypes *getRawLocalTimeTypes() const;
        // Return the address of the portion in the buffer containing local
        // time type information.

    RawLocalTimeTypes *getRawLocalTimeTypes64() const;
        // Return the address of the portion in the buffer containing version
        // '2' local time type information.

    char *getAbbrevData() const;
        // Return the address of the portion in the buffer containing
        // abbreviation string data.

    char *getAbbrevData64() const;
        // Return the address of the portion in the buffer containing
        // version '2' abbreviation string data.

    int timeZoneStringLength() const;
        // Return the length of the stored time zone string.
};

                             // ------------------
                             // class ZoneinfoData
                             // ------------------

// PRIVATE MANIPULATORS
void ZoneinfoData::populateBuffer(const RawHeader& header)
{
    d_size = sizeof(RawHeader)
             + bsl::max(header.numIsGmt(), 0) * 1
             + bsl::max(header.numIsStd(), 0) * 1
             + bsl::max(header.numLeaps(), 0) * sizeof(RawLeapInfo)
             + bsl::max(header.numTransitions(), 0) * (4 + 1)
             + bsl::max(header.numLocalTimeTypes(), 0) *
                                                      sizeof(RawLocalTimeTypes)
             + bsl::max(header.abbrevDataSize(), 0);
    if ('2' == header.version() || '3' == header.version()) {
        d_size += sizeof(RawHeader)
                  + bsl::max(header.numIsGmt(), 0) * 1
                  + bsl::max(header.numIsStd(), 0) * 1
                  + bsl::max(header.numLeaps(), 0) * sizeof(RawLeapInfo64)
                  + bsl::max(header.numTransitions(), 0) * (8 + 1)
                  + bsl::max(header.numLocalTimeTypes(), 0) *
                                                      sizeof(RawLocalTimeTypes)
                  + bsl::max(header.abbrevDataSize(), 0)
                  + 2;  // two 'newline' symbols for empty time zone string
    }
    d_buffer = new char[d_size];
    memset(d_buffer, 0, d_size);

    RawHeader *headerBuf = getRawHeader();
    memcpy(headerBuf, &header, sizeof *headerBuf);

    populateTransitionTimeBuf();
    populateTransitionIndexBuf();
    populateLeapCorrectionBuf();
    populateLocalTimeTypeBuf();
    populateAbbreviationData();

    if ('2' == header.version() || '3' == header.version()) {
        headerBuf = getRawHeader64();
        memcpy(headerBuf, &header, sizeof *headerBuf);

        populateTransitionTimeBuf64();
        populateTransitionIndexBuf64();
        populateLeapCorrectionBuf64();
        populateLocalTimeTypeBuf64();
        populateAbbreviationData64();
        populateTimeZoneString();
    }
}

void ZoneinfoData::populateTransitionTimeBuf()
{
    bdlb::BigEndianInt32 *buffer = getTransitionTime();
    for (int i = 0; i < getRawHeader()->numTransitions(); ++i) {
        buffer[i] = i;
    }
}

void ZoneinfoData::populateTransitionTimeBuf64()
{
    bdlb::BigEndianInt64 *buffer = getTransitionTime64();
    for (int i = 0; i < getRawHeader64()->numTransitions(); ++i) {
        buffer[i] = i;
    }
}

void ZoneinfoData::populateTransitionIndexBuf()
{
    unsigned char *buffer = getTransitionIndex();
    for (int i = 0; i < getRawHeader()->numTransitions(); ++i) {
        buffer[i] = i % getRawHeader()->numLocalTimeTypes();
    }
}

void ZoneinfoData::populateTransitionIndexBuf64()
{
    unsigned char *buffer = getTransitionIndex64();
    for (int i = 0; i < getRawHeader64()->numTransitions(); ++i) {
        buffer[i] = i % getRawHeader64()->numLocalTimeTypes();
    }
}

void ZoneinfoData::populateLeapCorrectionBuf()
{
    RawLeapInfo *buffer = getRawLeapInfo();
    for (int i = 0; i < getRawHeader()->numLeaps(); ++i) {
        buffer[i].setTransition(i);
        buffer[i].setCorrection(i);
    }
}

void ZoneinfoData::populateLeapCorrectionBuf64()
{
    RawLeapInfo64 *buffer = getRawLeapInfo64();
    for (int i = 0; i < getRawHeader64()->numLeaps(); ++i) {
        buffer[i].setTransition(i);
        buffer[i].setCorrection(i);
    }
}

void ZoneinfoData::populateLocalTimeTypeBuf()
{
    RawLocalTimeTypes *buffer = getRawLocalTimeTypes();
    for (int i = 0; i < getRawHeader()->numLocalTimeTypes(); ++i) {
        buffer[i].setOffset(i);
        buffer[i].setIsDst(i % 2);
        int abbrevDataSize = getRawHeader()->abbrevDataSize();
        if (abbrevDataSize) {
            buffer[i].setAbbreviationIndex(i % abbrevDataSize);
        }
    }
}

void ZoneinfoData::populateLocalTimeTypeBuf64()
{
    RawLocalTimeTypes *buffer = getRawLocalTimeTypes64();
    for (int i = 0; i < getRawHeader64()->numLocalTimeTypes(); ++i) {
        buffer[i].setOffset(i);
        buffer[i].setIsDst(i % 2);
        int abbrevDataSize = getRawHeader64()->abbrevDataSize();
        if (abbrevDataSize) {
            buffer[i].setAbbreviationIndex(i % abbrevDataSize);
        }
    }
}

void ZoneinfoData::populateAbbreviationData()
{
    if (getRawHeader()->abbrevDataSize() > 0) {
        char *buffer = getAbbrevData();
        memset(buffer, 0, getRawHeader()->abbrevDataSize());
    }
}

void ZoneinfoData::populateAbbreviationData64()
{
    if (getRawHeader64()->abbrevDataSize() > 0) {
        char *buffer = getAbbrevData64();
        memset(buffer, 0, getRawHeader64()->abbrevDataSize());
    }
}

void ZoneinfoData::populateTimeZoneString()
{
        char *buffer = getTimeZoneString();
        if (buffer) {
            // Adding two new lines symbols.

            memset(buffer - 1, 10, 1);  // enclosing data part with new line
            memset(buffer,     10, 1);  // enclosing empty zone info string
                                        // with new line
        }
}

// CREATORS
ZoneinfoData::ZoneinfoData()
{
    RawHeader header;
    populateBuffer(header);
}

ZoneinfoData::ZoneinfoData(const RawHeader& header)
{
    populateBuffer(header);
}

ZoneinfoData::ZoneinfoData(const char *data, bsl::size_t size)
{
    d_size   = size;
    d_buffer = new char[d_size];
    memcpy(d_buffer, data, d_size);
}

ZoneinfoData::~ZoneinfoData()
{
    delete[](d_buffer);
}

// MANIPULATORS
void ZoneinfoData::setTimeZoneString(const char *data, bsl::size_t size)
{
    RawHeader *head = getRawHeader();

    if ('2' != head->version() && '3' != head->version()) {
        return;                                                       // RETURN
    }

    RawHeader *head64 = getRawHeader64();

    bsl::size_t dataSize =
           sizeof(RawHeader)
         + bsl::max(head->numIsGmt(), 0)          * 1
         + bsl::max(head->numIsStd(), 0)          * 1
         + bsl::max(head->numLeaps(), 0)          * sizeof(RawLeapInfo)
         + bsl::max(head->numTransitions(), 0)    * (4 + 1)
         + bsl::max(head->numLocalTimeTypes(), 0) * sizeof(RawLocalTimeTypes)
         + bsl::max(head->abbrevDataSize(), 0)
         + sizeof(RawHeader)
         + bsl::max(head64->numIsGmt(), 0)          * 1
         + bsl::max(head64->numIsStd(), 0)          * 1
         + bsl::max(head64->numLeaps(), 0)          * sizeof(RawLeapInfo64)
         + bsl::max(head64->numTransitions(), 0)    * (8 + 1)
         + bsl::max(head64->numLocalTimeTypes(), 0) * sizeof(RawLocalTimeTypes)
         + bsl::max(head64->abbrevDataSize(), 0);

     // We need to store data, time zone string and two newline symbols.

     int   newSize = static_cast<int>(dataSize + size + 2);
     char *buffer  = new char[newSize];

     // Duplicating data.

     for (bsl::size_t i = 0; i < dataSize; ++i) {
         buffer[i] = d_buffer[i];
     }

     // Enclosing data.

     memset(buffer + dataSize, 10, 1);

     // Duplicating time zone string.

     for (bsl::size_t i = 0; i < size; ++i) {
         buffer[i + dataSize + 1] = data[i];
     }

     // Enclosing time zone string.

     memset(buffer + dataSize + 1 + size, 10, 1);

     d_size = newSize;
     delete [] d_buffer;
     d_buffer = buffer;
}


// ACCESSORS
char *ZoneinfoData::buffer() const
{
    return d_buffer;
}

int ZoneinfoData::size() const
{
    return d_size;
}

char *ZoneinfoData::getVersion2Address() const
{
    return &d_buffer[sizeof(RawHeader)
        + bsl::max(getRawHeader()->numTransitions(), 0) * 4
        + bsl::max(getRawHeader()->numTransitions(), 0)
        + bsl::max(getRawHeader()->numLocalTimeTypes(), 0) *
                                                      sizeof(RawLocalTimeTypes)
        + bsl::max(getRawHeader()->abbrevDataSize(), 0)
        + bsl::max(getRawHeader()->numLeaps(), 0) * sizeof(RawLeapInfo)
        + bsl::max(getRawHeader()->numIsStd(), 0)
        + bsl::max(getRawHeader()->numIsGmt(), 0)];
}

RawHeader *ZoneinfoData::getRawHeader() const
{
    return reinterpret_cast<RawHeader*>(&d_buffer[0]);
}

RawHeader *ZoneinfoData::getRawHeader64() const
{
    return reinterpret_cast<RawHeader*>(getVersion2Address());
}

char *ZoneinfoData::getTimeZoneString() const
{
    return reinterpret_cast<char *> (
        getVersion2Address()
        + sizeof(RawHeader)
        + bsl::max(getRawHeader64()->numIsGmt(), 0) * 1
        + bsl::max(getRawHeader64()->numIsStd(), 0) * 1
        + bsl::max(getRawHeader64()->numLeaps(), 0) * sizeof(RawLeapInfo64)
        + bsl::max(getRawHeader64()->numTransitions(), 0) * (8 + 1)
        + bsl::max(getRawHeader64()->numLocalTimeTypes(), 0) *
                                                      sizeof(RawLocalTimeTypes)
        + bsl::max(getRawHeader64()->abbrevDataSize(), 0))
        + 1;  // newline enclosing for data part
}

bdlb::BigEndianInt32 *ZoneinfoData::getTransitionTime() const
{
    return reinterpret_cast<bdlb::BigEndianInt32*>(
        &d_buffer[sizeof(RawHeader)]);
}

bdlb::BigEndianInt64 *ZoneinfoData::getTransitionTime64() const
{
    return reinterpret_cast<bdlb::BigEndianInt64*>(
        getVersion2Address() + sizeof(RawHeader));
}

unsigned char *ZoneinfoData::getTransitionIndex() const
{
    return reinterpret_cast<unsigned char *>(
        &d_buffer[sizeof(RawHeader)
        + bsl::max(getRawHeader()->numTransitions(), 0) * 4]);
}

unsigned char *ZoneinfoData::getTransitionIndex64() const
{
    return reinterpret_cast<unsigned char *>(
        getVersion2Address()
        + sizeof(RawHeader)
        + bsl::max(getRawHeader64()->numTransitions(), 0) * 8);
}

RawLeapInfo *ZoneinfoData::getRawLeapInfo() const
{
    return reinterpret_cast<RawLeapInfo*>(
        &d_buffer[sizeof(RawHeader)
        + bsl::max(getRawHeader()->numTransitions(), 0) * 4
        + bsl::max(getRawHeader()->numTransitions(), 0)
        + bsl::max(getRawHeader()->numLocalTimeTypes(), 0) *
                                                      sizeof(RawLocalTimeTypes)
        + bsl::max(getRawHeader()->abbrevDataSize(), 0)]);
}

RawLeapInfo64 *ZoneinfoData::getRawLeapInfo64() const
{
    return reinterpret_cast<RawLeapInfo64*>(
        getVersion2Address()
        + sizeof(RawHeader)
        + bsl::max(getRawHeader64()->numTransitions(), 0) * 8
        + bsl::max(getRawHeader64()->numTransitions(), 0)
        + bsl::max(getRawHeader64()->numLocalTimeTypes(), 0) *
                                                      sizeof(RawLocalTimeTypes)
        + bsl::max(getRawHeader64()->abbrevDataSize(), 0));
}

RawLocalTimeTypes *ZoneinfoData::getRawLocalTimeTypes() const
{
    return reinterpret_cast<RawLocalTimeTypes*>(
        &d_buffer[sizeof(RawHeader)
        + bsl::max(getRawHeader()->numTransitions(), 0) * 4
        + bsl::max(getRawHeader()->numTransitions(), 0)]);
}

RawLocalTimeTypes *ZoneinfoData::getRawLocalTimeTypes64() const
{
    return reinterpret_cast<RawLocalTimeTypes*>(
        getVersion2Address()
        + sizeof(RawHeader)
        + bsl::max(getRawHeader64()->numTransitions(), 0) * 8
        + bsl::max(getRawHeader64()->numTransitions(), 0));
}

char *ZoneinfoData::getAbbrevData() const
{
    return &d_buffer[
        sizeof(RawHeader)
        + bsl::max(getRawHeader()->numTransitions(), 0) * 4
        + bsl::max(getRawHeader()->numTransitions(), 0)
        + bsl::max(getRawHeader()->numLocalTimeTypes(), 0) *
                                                    sizeof(RawLocalTimeTypes)];
}

char *ZoneinfoData::getAbbrevData64() const
{
    return getVersion2Address()
        + sizeof(RawHeader)
        + bsl::max(getRawHeader64()->numTransitions(), 0) * 8
        + bsl::max(getRawHeader64()->numTransitions(), 0)
        + bsl::max(getRawHeader64()->numLocalTimeTypes(), 0) *
                                                     sizeof(RawLocalTimeTypes);
}

int ZoneinfoData::timeZoneStringLength() const
{
    if (getRawHeader()->version() != '2' && getRawHeader()->version() != '3') {
        return 0;                                                     // RETURN
    }

    return d_size - (getTimeZoneString() - d_buffer) - 1;
}

// ----------------------------------------------------------------------------

static int verifyTimeZone(const ZoneinfoData&     data,
                          const baltzo::Zoneinfo& timeZone,
                          int                     line,
                          bool                    expectToFail)
{

    const int LINE = line;
    const baltzo::Zoneinfo& X = timeZone;

    RawHeader *H = data.getRawHeader();

    char *abbrevDataBuf = data.getAbbrevData();

    RawLocalTimeTypes *localTimeTypeBuf = data.getRawLocalTimeTypes();

    // An extra transition is created for to handle time earlier than the first
    // transition.

    if (H->numTransitions() != X.numTransitions() - 1) {
        if (!expectToFail) {
            LOOP3_ASSERT(LINE, H->numTransitions(), X.numTransitions() - 1,
                        H->numTransitions() == X.numTransitions() - 1);
        }
        return 1;                                                     // RETURN
    }

    baltzo::Zoneinfo::TransitionConstIterator XT = X.beginTransitions();
    if (FIRST_TRANSITION != XT->utcTime()) {
        if (!expectToFail) {
            LOOP2_ASSERT(LINE, XT->utcTime(),
                         FIRST_TRANSITION == XT->utcTime());
        }
        return 2;                                                     // RETURN
    }

    ++XT;

    bdlb::BigEndianInt32 *transitionTimeBuf = data.getTransitionTime();
    unsigned char *transitionIndexBuf = data.getTransitionIndex();
    for (int i = 0; i < H->numTransitions(); ++i) {
        if (X.endTransitions() == XT) {
            if (!expectToFail) {
                LOOP_ASSERT(LINE, X.endTransitions() != XT);
            }
            return 3;                                                 // RETURN
        }

        int TT = *transitionTimeBuf;
        if (TT != XT->utcTime()) {
            if (!expectToFail) {
                LOOP4_ASSERT(LINE, i, TT, XT->utcTime(),
                             TT == XT->utcTime());
            }
            return 4;                                                 // RETURN
        }

        int index = transitionIndexBuf[i];

        baltzo::LocalTimeDescriptor D(
                  localTimeTypeBuf[index].offset(),
                  localTimeTypeBuf[index].isDst(),
                  &abbrevDataBuf[localTimeTypeBuf[index].abbreviationIndex()]);

        if (D != XT->descriptor()) {
            if (!expectToFail) {
                LOOP4_ASSERT(LINE, i, D, XT->descriptor(),
                             D == XT->descriptor());
            }
            return 5;                                                 // RETURN
        }

        ++XT;
        ++transitionTimeBuf;
    }

    return 0;
}

// ----------------------------------------------------------------------------

static int verifyTimeZoneVersion2Format(const ZoneinfoData&     data,
                                        const baltzo::Zoneinfo& timeZone,
                                        int                     line,
                                        bool                    expectToFail)
{
    const int                LINE             = line;
    const baltzo::Zoneinfo&  X                = timeZone;
    const RawHeader         *H                = data.getRawHeader64();
    char                    *abbrevDataBuf    = data.getAbbrevData64();
    RawLocalTimeTypes       *localTimeTypeBuf = data.getRawLocalTimeTypes64();

    // An extra transition is created for to handle time earlier than the first
    // transition.

    if (H->numTransitions() != X.numTransitions() - 1) {
        if (!expectToFail) {
            LOOP3_ASSERT(LINE, H->numTransitions(), X.numTransitions() - 1,
                        H->numTransitions() == X.numTransitions() - 1);
        }
        return 1;                                                     // RETURN
    }

    baltzo::Zoneinfo::TransitionConstIterator XT = X.beginTransitions();
    if (FIRST_TRANSITION != XT->utcTime()) {
        if (!expectToFail) {
            LOOP2_ASSERT(LINE, XT->utcTime(),
                         FIRST_TRANSITION == XT->utcTime());
        }
        return 2;                                                     // RETURN
    }

    ++XT;

    bdlb::BigEndianInt64 *transitionTimeBuf = data.getTransitionTime64();
    unsigned char *transitionIndexBuf = data.getTransitionIndex64();
    for (int i = 0; i < H->numTransitions(); ++i) {
        if (X.endTransitions() == XT) {
            if (!expectToFail) {
                LOOP_ASSERT(LINE, X.endTransitions() != XT);
            }
            return 3;                                                 // RETURN
        }

        bsls::Types::Int64 TT = *transitionTimeBuf;
        if (TT != XT->utcTime()) {
            if (!expectToFail) {
                LOOP4_ASSERT(LINE, i, TT, XT->utcTime(),
                             TT == XT->utcTime());
            }
            return 4;                                                 // RETURN
        }

        int index = transitionIndexBuf[i];

        baltzo::LocalTimeDescriptor D(
                  localTimeTypeBuf[index].offset(),
                  localTimeTypeBuf[index].isDst(),
                  &abbrevDataBuf[localTimeTypeBuf[index].abbreviationIndex()]);

        if (D != XT->descriptor()) {
            if (!expectToFail) {
                LOOP4_ASSERT(LINE, i, D, XT->descriptor(),
                             D == XT->descriptor());
            }
            return 5;                                                 // RETURN
        }

        ++XT;
        ++transitionTimeBuf;
    }

    // Verify time zone string.

    int length = data.timeZoneStringLength();

    bsl::string zoneInfoString = X.extendedTransitionsDescription();
    bsl::string zoneInfoDataString(data.getTimeZoneString(),
                                   data.timeZoneStringLength());

    if (zoneInfoString != zoneInfoDataString) {
        if (!expectToFail) {
            LOOP3_ASSERT(LINE, zoneInfoString, zoneInfoDataString,
                         zoneInfoString == zoneInfoDataString);
        }
        return 6;                                                     // RETURN
    }

    return 0;
}

// ----------------------------------------------------------------------------

static int testVerifyTimeZone(int verbose)
    // Run ad-hoc tests on the 'verifyTimeZone' function.  The tests will be
    // based on a Zoneinfo with 5 transitions and 3 local time types.
    // 'verifyTimeZone' will be called on various 'baltzo::Zoneinfo' objects
    // and the return code will be verified.  Return 0 if all the tests passed,
    // and a non-zero value otherwise.
{
    // Create a table of transition time.

    const int TRANSITION_TIMES[] = {
        INT_MIN,
        -1,
        0,
        1,
        INT_MAX
    };

    enum { NUM_TRANSITION_TIME =
                          sizeof TRANSITION_TIMES / sizeof *TRANSITION_TIMES };

    static const struct {
        int           d_offset;
        unsigned char d_dstFlag;
        unsigned char d_abbrevIndex;
    } LOCAL_TIME_TYPES[] = {

    // OFFSET  DST_FLAG  AB_INDEX
    // ------  --------  --------

    {  -86399,        0,        0 },
    {       0,        1,        1 },
    {   86399,        0,        3 }

    };

    enum { NUM_LOCAL_TIME_TYPES =
                          sizeof LOCAL_TIME_TYPES / sizeof *LOCAL_TIME_TYPES };

    const char AB_DATA[] = "\0A\0AB\0";
    enum { AB_DATA_SIZE = sizeof AB_DATA / sizeof *AB_DATA };

    // Create 'ZoneinfoData'

    RawHeader RH;
    RH.setVersion('\0');
    RH.setNumIsGmt(NUM_LOCAL_TIME_TYPES);
    RH.setNumIsStd(NUM_LOCAL_TIME_TYPES);
    RH.setNumTransitions(NUM_TRANSITION_TIME);
    RH.setNumLocalTimeTypes(NUM_LOCAL_TIME_TYPES);
    RH.setAbbrevDataSize(AB_DATA_SIZE);

    ZoneinfoData ZI(RH);

    bdlb::BigEndianInt32 *transitions = ZI.getTransitionTime();
    for (int i = 0; i < NUM_TRANSITION_TIME; ++i) {
        transitions[i] = TRANSITION_TIMES[i];
    }

    RawLocalTimeTypes *localTimeTypes = ZI.getRawLocalTimeTypes();
    for (int i = 0; i < NUM_LOCAL_TIME_TYPES; ++i) {
        localTimeTypes[i].setOffset(LOCAL_TIME_TYPES[i].d_offset);
        localTimeTypes[i].setIsDst(LOCAL_TIME_TYPES[i].d_dstFlag);
        localTimeTypes[i].setAbbreviationIndex(
                                            LOCAL_TIME_TYPES[i].d_abbrevIndex);
    }

    char *buffer = ZI.getAbbrevData();
    memcpy(buffer, AB_DATA, AB_DATA_SIZE);

    baltzo::LocalTimeDescriptor D[NUM_LOCAL_TIME_TYPES];
    for (int i = 0; i < NUM_LOCAL_TIME_TYPES; ++i) {
        D[i].setUtcOffsetInSeconds(LOCAL_TIME_TYPES[i].d_offset);
        D[i].setDstInEffectFlag(LOCAL_TIME_TYPES[i].d_dstFlag);
        D[i].setDescription(&AB_DATA[LOCAL_TIME_TYPES[i].d_abbrevIndex]);
    }

    if (verbose) cout << "\nCorrect Zoneinfo" << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(FIRST_TRANSITION, D[0]);
        TZ.addTransition(TRANSITION_TIMES[0], D[0]);
        TZ.addTransition(TRANSITION_TIMES[1], D[1]);
        TZ.addTransition(TRANSITION_TIMES[2], D[2]);
        TZ.addTransition(TRANSITION_TIMES[3], D[0]);
        TZ.addTransition(TRANSITION_TIMES[4], D[1]);

        ASSERT(0 == verifyTimeZone(ZI, TZ, L_, false));
    }

    if (verbose) cout << "\nMissing transition at 'Jan 1, 1'" << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(TRANSITION_TIMES[0], D[0]);
        TZ.addTransition(TRANSITION_TIMES[1], D[1]);
        TZ.addTransition(TRANSITION_TIMES[2], D[2]);
        TZ.addTransition(TRANSITION_TIMES[3], D[0]);
        TZ.addTransition(TRANSITION_TIMES[4], D[1]);

        ASSERT(0 != verifyTimeZone(ZI, TZ, L_, true));
    }

    if (verbose) cout << "\nIncorrect first transition" << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(FIRST_TRANSITION + 1, D[0]);
        TZ.addTransition(TRANSITION_TIMES[0], D[0]);
        TZ.addTransition(TRANSITION_TIMES[1], D[1]);
        TZ.addTransition(TRANSITION_TIMES[2], D[2]);
        TZ.addTransition(TRANSITION_TIMES[3], D[0]);
        TZ.addTransition(TRANSITION_TIMES[4], D[1]);

        ASSERT(0 != verifyTimeZone(ZI, TZ, L_, true));
    }

    if (verbose) cout << "\nMissing one transition" << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(FIRST_TRANSITION, D[0]);
        TZ.addTransition(TRANSITION_TIMES[0], D[0]);
        TZ.addTransition(TRANSITION_TIMES[1], D[1]);
        TZ.addTransition(TRANSITION_TIMES[2], D[2]);

        ASSERT(0 != verifyTimeZone(ZI, TZ, L_, true));
    }

    if (verbose) cout << "\nOne extra transition" << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(FIRST_TRANSITION, D[0]);
        TZ.addTransition(TRANSITION_TIMES[0],     D[0]);
        TZ.addTransition(TRANSITION_TIMES[1],     D[1]);
        TZ.addTransition(TRANSITION_TIMES[2],     D[2]);
        TZ.addTransition(TRANSITION_TIMES[3],     D[0]);
        TZ.addTransition(TRANSITION_TIMES[4] - 1, D[1]);
        TZ.addTransition(TRANSITION_TIMES[4],     D[1]);

        ASSERT(0 != verifyTimeZone(ZI, TZ, L_, true));
    }

    if (verbose) cout << "\nIncorrect transition data" << endl;
    {
        {
            baltzo::Zoneinfo TZ;
            TZ.addTransition(FIRST_TRANSITION, D[0]);
            TZ.addTransition(TRANSITION_TIMES[0] + 1, D[0]);
            TZ.addTransition(TRANSITION_TIMES[1],     D[1]);
            TZ.addTransition(TRANSITION_TIMES[2],     D[2]);
            TZ.addTransition(TRANSITION_TIMES[3],     D[0]);
            TZ.addTransition(TRANSITION_TIMES[4],     D[1]);

            ASSERT(0 != verifyTimeZone(ZI, TZ, L_, true));
        }
        {
            baltzo::Zoneinfo TZ;
            TZ.addTransition(FIRST_TRANSITION, D[0]);
            TZ.addTransition(TRANSITION_TIMES[0],     D[0]);
            TZ.addTransition(TRANSITION_TIMES[1] - 1, D[1]);
            TZ.addTransition(TRANSITION_TIMES[2],     D[2]);

            ASSERT(0 != verifyTimeZone(ZI, TZ, L_, true));
        }
        {
            baltzo::Zoneinfo TZ;
            TZ.addTransition(FIRST_TRANSITION, D[0]);
            TZ.addTransition(TRANSITION_TIMES[0],     D[0]);
            TZ.addTransition(TRANSITION_TIMES[1],     D[1]);
            TZ.addTransition(TRANSITION_TIMES[2] - 1, D[2]);
            TZ.addTransition(TRANSITION_TIMES[3],     D[0]);
            TZ.addTransition(TRANSITION_TIMES[4],     D[1]);

            ASSERT(0 != verifyTimeZone(ZI, TZ, L_, true));
        }
    }

    if (verbose) cout << "\nIncorrect index to local time descriptor" << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(FIRST_TRANSITION, D[0]);
        TZ.addTransition(TRANSITION_TIMES[0], D[0]);
        TZ.addTransition(TRANSITION_TIMES[1], D[1]);
        TZ.addTransition(TRANSITION_TIMES[2], D[2]);
        TZ.addTransition(TRANSITION_TIMES[3], D[0]);
        TZ.addTransition(TRANSITION_TIMES[4], D[0]);

        ASSERT(0 != verifyTimeZone(ZI, TZ, L_, true));
    }

    if (verbose) cout << "\nIncorrect local time descriptor data" << endl;
    {
        {
            baltzo::Zoneinfo TZ;
            baltzo::LocalTimeDescriptor dError(-1, 0, "");
            TZ.addTransition(FIRST_TRANSITION, D[0]);
            TZ.addTransition(TRANSITION_TIMES[0], dError);
            TZ.addTransition(TRANSITION_TIMES[1], D[1]);
            TZ.addTransition(TRANSITION_TIMES[2], D[2]);
            TZ.addTransition(TRANSITION_TIMES[3], D[0]);
            TZ.addTransition(TRANSITION_TIMES[4], D[1]);

            ASSERT(0 != verifyTimeZone(ZI, TZ, L_, true));
        }
        {
            baltzo::Zoneinfo TZ;
            baltzo::LocalTimeDescriptor dError(1, 2, "A");
            TZ.addTransition(FIRST_TRANSITION, D[0]);
            TZ.addTransition(TRANSITION_TIMES[0], D[0]);
            TZ.addTransition(TRANSITION_TIMES[1], dError);
            TZ.addTransition(TRANSITION_TIMES[2], D[2]);
            TZ.addTransition(TRANSITION_TIMES[3], D[0]);
            TZ.addTransition(TRANSITION_TIMES[4], D[1]);

            ASSERT(0 != verifyTimeZone(ZI, TZ, L_, true));
        }
        {
            baltzo::Zoneinfo TZ;
            baltzo::LocalTimeDescriptor dError(1, 1, "A");
            TZ.addTransition(FIRST_TRANSITION, D[0]);
            TZ.addTransition(TRANSITION_TIMES[0], D[0]);
            TZ.addTransition(TRANSITION_TIMES[1], D[1]);
            TZ.addTransition(TRANSITION_TIMES[2], D[2]);
            TZ.addTransition(TRANSITION_TIMES[3], D[0]);
            TZ.addTransition(TRANSITION_TIMES[4], dError);

            ASSERT(0 != verifyTimeZone(ZI, TZ, L_, true));
        }
    }
    return 0;
}

// ----------------------------------------------------------------------------

static int testVerifyTimeZoneVersion2Format(int verbose)
    // Run ad-hoc tests on the 'verifyTimeZoneVersion2Format' function.  The
    // tests will be based on a Zoneinfo with 5 transitions and 3 local time
    // types.  'verifyTimeZoneVersion2Format' will be called on various
    // 'baltzo::Zoneinfo' objects and the return code will be verified.  Return
    // 0 if all the tests passed, and a non-zero value otherwise.
{
    // Create a table of transition time.

    const int TRANSITION_TIMES[] = {
        INT_MIN,
        -1,
        0,
        1,
        INT_MAX
    };

    enum { NUM_TRANSITION_TIME =
                          sizeof TRANSITION_TIMES / sizeof *TRANSITION_TIMES };

    static const struct {
        int           d_offset;
        unsigned char d_dstFlag;
        unsigned char d_abbrevIndex;
    } LOCAL_TIME_TYPES[] = {

    // OFFSET  DST_FLAG  AB_INDEX
    // ------  --------  --------

    {  -86399,        0,        0 },
    {       0,        1,        1 },
    {   86399,        0,        3 }

    };

    enum { NUM_LOCAL_TIME_TYPES =
                          sizeof LOCAL_TIME_TYPES / sizeof *LOCAL_TIME_TYPES };

    const char AB_DATA[] = "\0A\0AB\0";
    enum { AB_DATA_SIZE = sizeof AB_DATA / sizeof *AB_DATA };

    // Create 'ZoneinfoData'

    RawHeader RH;
    RH.setVersion('2');
    RH.setNumIsGmt(NUM_LOCAL_TIME_TYPES);
    RH.setNumIsStd(NUM_LOCAL_TIME_TYPES);
    RH.setNumTransitions(NUM_TRANSITION_TIME);
    RH.setNumLocalTimeTypes(NUM_LOCAL_TIME_TYPES);
    RH.setAbbrevDataSize(AB_DATA_SIZE);

    ZoneinfoData ZI(RH);

    bdlb::BigEndianInt64 *transitions = ZI.getTransitionTime64();
    for (int i = 0; i < NUM_TRANSITION_TIME; ++i) {
        transitions[i] = TRANSITION_TIMES[i];
    }

    RawLocalTimeTypes *localTimeTypes = ZI.getRawLocalTimeTypes64();
    for (int i = 0; i < NUM_LOCAL_TIME_TYPES; ++i) {
        localTimeTypes[i].setOffset(LOCAL_TIME_TYPES[i].d_offset);
        localTimeTypes[i].setIsDst(LOCAL_TIME_TYPES[i].d_dstFlag);
        localTimeTypes[i].setAbbreviationIndex(
                                            LOCAL_TIME_TYPES[i].d_abbrevIndex);
    }

    char *buffer = ZI.getAbbrevData64();
    memcpy(buffer, AB_DATA, AB_DATA_SIZE);

    baltzo::LocalTimeDescriptor D[NUM_LOCAL_TIME_TYPES];
    for (int i = 0; i < NUM_LOCAL_TIME_TYPES; ++i) {
        D[i].setUtcOffsetInSeconds(LOCAL_TIME_TYPES[i].d_offset);
        D[i].setDstInEffectFlag(LOCAL_TIME_TYPES[i].d_dstFlag);
        D[i].setDescription(&AB_DATA[LOCAL_TIME_TYPES[i].d_abbrevIndex]);
    }

    if (verbose) cout << "\nCorrect Zoneinfo" << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(FIRST_TRANSITION, D[0]);
        TZ.addTransition(TRANSITION_TIMES[0], D[0]);
        TZ.addTransition(TRANSITION_TIMES[1], D[1]);
        TZ.addTransition(TRANSITION_TIMES[2], D[2]);
        TZ.addTransition(TRANSITION_TIMES[3], D[0]);
        TZ.addTransition(TRANSITION_TIMES[4], D[1]);

        ASSERT(0 == verifyTimeZoneVersion2Format(ZI, TZ, L_, false));
    }

    if (verbose) cout << "\nMissing transition at 'Jan 1, 1'" << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(TRANSITION_TIMES[0], D[0]);
        TZ.addTransition(TRANSITION_TIMES[1], D[1]);
        TZ.addTransition(TRANSITION_TIMES[2], D[2]);
        TZ.addTransition(TRANSITION_TIMES[3], D[0]);
        TZ.addTransition(TRANSITION_TIMES[4], D[1]);

        ASSERT(0 != verifyTimeZoneVersion2Format(ZI, TZ, L_, true));
    }

    if (verbose) cout << "\nIncorrect first transition" << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(FIRST_TRANSITION + 1, D[0]);
        TZ.addTransition(TRANSITION_TIMES[0], D[0]);
        TZ.addTransition(TRANSITION_TIMES[1], D[1]);
        TZ.addTransition(TRANSITION_TIMES[2], D[2]);
        TZ.addTransition(TRANSITION_TIMES[3], D[0]);
        TZ.addTransition(TRANSITION_TIMES[4], D[1]);

        ASSERT(0 != verifyTimeZoneVersion2Format(ZI, TZ, L_, true));
    }

    if (verbose) cout << "\nMissing one transition" << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(FIRST_TRANSITION, D[0]);
        TZ.addTransition(TRANSITION_TIMES[0], D[0]);
        TZ.addTransition(TRANSITION_TIMES[1], D[1]);
        TZ.addTransition(TRANSITION_TIMES[2], D[2]);

        ASSERT(0 != verifyTimeZoneVersion2Format(ZI, TZ, L_, true));
    }

    if (verbose) cout << "\nOne extra transition" << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(FIRST_TRANSITION, D[0]);
        TZ.addTransition(TRANSITION_TIMES[0],     D[0]);
        TZ.addTransition(TRANSITION_TIMES[1],     D[1]);
        TZ.addTransition(TRANSITION_TIMES[2],     D[2]);
        TZ.addTransition(TRANSITION_TIMES[3],     D[0]);
        TZ.addTransition(TRANSITION_TIMES[4] - 1, D[1]);
        TZ.addTransition(TRANSITION_TIMES[4],     D[1]);

        ASSERT(0 != verifyTimeZoneVersion2Format(ZI, TZ, L_, true));
    }

    if (verbose) cout << "\nIncorrect transition data" << endl;
    {
        {
            baltzo::Zoneinfo TZ;
            TZ.addTransition(FIRST_TRANSITION, D[0]);
            TZ.addTransition(TRANSITION_TIMES[0] + 1, D[0]);
            TZ.addTransition(TRANSITION_TIMES[1],     D[1]);
            TZ.addTransition(TRANSITION_TIMES[2],     D[2]);
            TZ.addTransition(TRANSITION_TIMES[3],     D[0]);
            TZ.addTransition(TRANSITION_TIMES[4],     D[1]);

            ASSERT(0 != verifyTimeZoneVersion2Format(ZI, TZ, L_, true));
        }
        {
            baltzo::Zoneinfo TZ;
            TZ.addTransition(FIRST_TRANSITION, D[0]);
            TZ.addTransition(TRANSITION_TIMES[0],     D[0]);
            TZ.addTransition(TRANSITION_TIMES[1] - 1, D[1]);
            TZ.addTransition(TRANSITION_TIMES[2],     D[2]);

            ASSERT(0 != verifyTimeZoneVersion2Format(ZI, TZ, L_, true));
        }
        {
            baltzo::Zoneinfo TZ;
            TZ.addTransition(FIRST_TRANSITION, D[0]);
            TZ.addTransition(TRANSITION_TIMES[0],     D[0]);
            TZ.addTransition(TRANSITION_TIMES[1],     D[1]);
            TZ.addTransition(TRANSITION_TIMES[2] - 1, D[2]);
            TZ.addTransition(TRANSITION_TIMES[3],     D[0]);
            TZ.addTransition(TRANSITION_TIMES[4],     D[1]);

            ASSERT(0 != verifyTimeZoneVersion2Format(ZI, TZ, L_, true));
        }
    }

    if (verbose) cout << "\nIncorrect index to local time descriptor" << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(FIRST_TRANSITION, D[0]);
        TZ.addTransition(TRANSITION_TIMES[0], D[0]);
        TZ.addTransition(TRANSITION_TIMES[1], D[1]);
        TZ.addTransition(TRANSITION_TIMES[2], D[2]);
        TZ.addTransition(TRANSITION_TIMES[3], D[0]);
        TZ.addTransition(TRANSITION_TIMES[4], D[0]);

        ASSERT(0 != verifyTimeZoneVersion2Format(ZI, TZ, L_, true));
    }

    if (verbose) cout << "\nIncorrect local time descriptor data" << endl;
    {
        {
            baltzo::Zoneinfo TZ;
            baltzo::LocalTimeDescriptor dError(-1, 0, "");
            TZ.addTransition(FIRST_TRANSITION, D[0]);
            TZ.addTransition(TRANSITION_TIMES[0], dError);
            TZ.addTransition(TRANSITION_TIMES[1], D[1]);
            TZ.addTransition(TRANSITION_TIMES[2], D[2]);
            TZ.addTransition(TRANSITION_TIMES[3], D[0]);
            TZ.addTransition(TRANSITION_TIMES[4], D[1]);

            ASSERT(0 != verifyTimeZoneVersion2Format(ZI, TZ, L_, true));
        }
        {
            baltzo::Zoneinfo TZ;
            baltzo::LocalTimeDescriptor dError(1, 2, "A");
            TZ.addTransition(FIRST_TRANSITION, D[0]);
            TZ.addTransition(TRANSITION_TIMES[0], D[0]);
            TZ.addTransition(TRANSITION_TIMES[1], dError);
            TZ.addTransition(TRANSITION_TIMES[2], D[2]);
            TZ.addTransition(TRANSITION_TIMES[3], D[0]);
            TZ.addTransition(TRANSITION_TIMES[4], D[1]);

            ASSERT(0 != verifyTimeZoneVersion2Format(ZI, TZ, L_, true));
        }
        {
            baltzo::Zoneinfo TZ;
            baltzo::LocalTimeDescriptor dError(1, 1, "A");
            TZ.addTransition(FIRST_TRANSITION, D[0]);
            TZ.addTransition(TRANSITION_TIMES[0], D[0]);
            TZ.addTransition(TRANSITION_TIMES[1], D[1]);
            TZ.addTransition(TRANSITION_TIMES[2], D[2]);
            TZ.addTransition(TRANSITION_TIMES[3], D[0]);
            TZ.addTransition(TRANSITION_TIMES[4], dError);

            ASSERT(0 != verifyTimeZoneVersion2Format(ZI, TZ, L_, true));
        }
    }

    ZI.setTimeZoneString("abc\n", 4);

    if (verbose) cout << "\nTesting time zone string." << endl;
    {
        baltzo::Zoneinfo TZ;
        TZ.addTransition(FIRST_TRANSITION, D[0]);
        TZ.addTransition(TRANSITION_TIMES[0], D[0]);
        TZ.addTransition(TRANSITION_TIMES[1], D[1]);
        TZ.addTransition(TRANSITION_TIMES[2], D[2]);
        TZ.addTransition(TRANSITION_TIMES[3], D[0]);
        TZ.addTransition(TRANSITION_TIMES[4], D[1]);

        TZ.setExtendedTransitionsDescription("abc\n");

        ASSERT(0 == verifyTimeZoneVersion2Format(ZI, TZ, L_, true));
    }
    return 0;
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    (void)veryVerbose;
    (void)veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 12: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

///Usage
///-----
// In this section, we illustrate how this component can be used.
//
///Example 1: Reading Zoneinfo binary data
///- - - - - - - - - - - - - - - - - - - -
// The following demonstrates how to read a byte stream in the Zoneinfo binary
// data format into a 'baltzo::Zoneinfo' object.  We start by creating Zoneinfo
// data in memory for "Asia/Bangkok", which was chosen due to its small size.
// Note that this data was generated by the 'zic' compiler, which is publicly
// obtainable as part of the standard Zoneinfo distribution (see
// 'http://www.twinsun.com/tz/tz-link.htm'):
//..
    const unsigned char ASIA_BANGKOK_DATA[] = {
        0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
        0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0xa2, 0x6a, 0x67, 0xc4,
        0x01, 0x00, 0x00, 0x5e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x62, 0x70, 0x00,
        0x04, 0x42, 0x4d, 0x54, 0x00, 0x49, 0x43, 0x54, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0c, 0xff, 0xff, 0xff,
        0xff, 0x56, 0xb6, 0x85, 0xc4, 0xff, 0xff, 0xff, 0xff, 0xa2, 0x6a, 0x67,
        0xc4, 0x01, 0x02, 0x00, 0x00, 0x5e, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x5e,
        0x3c, 0x00, 0x04, 0x00, 0x00, 0x62, 0x70, 0x00, 0x08, 0x4c, 0x4d, 0x54,
        0x00, 0x42, 0x4d, 0x54, 0x00, 0x49, 0x43, 0x54, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x0a, 0x49, 0x43, 0x54, 0x2d, 0x37, 0x0a
    };
//..
// Then, we load this data into a stream buffer.
//..
    bdlsb::FixedMemInStreamBuf inStreamBuf(
                            reinterpret_cast<const char *>(ASIA_BANGKOK_DATA),
                            sizeof(ASIA_BANGKOK_DATA));
    bsl::istream inputStream(&inStreamBuf);
//..
// Now, we read the 'inputStream' using 'baltzo::ZoneinfoBinaryReader::read'.
//..
    baltzo::Zoneinfo timeZone;
    if (0 != baltzo::ZoneinfoBinaryReader::read(&timeZone, inputStream)) {
        bsl::cerr << "baltzo::ZoneinfoBinaryReader::load failed"
                  << bsl::endl;
        return 1;                                                     // RETURN
    }
//..
// Finally, we write a description of the loaded Zoneinfo to the console.
//..
    if (verbose) timeZone.print(bsl::cout, 1, 3);
//..
// The output of the preceding statement should look like:
//..
// [
//
//    [
//       [ BMT 24124 DST:N ]
//       [ ICT 25200 DST:N ]
//    ]
//    [
//       [
//       01JAN0001_00:00:00.000
//       [ BMT 24124 DST:N ]
//       ]
//       [
//       31MAR1920_17:17:56.000
//       [ ICT 25200 DST:N ]
//       ]
//    ]
// ]
//..
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INVALID HEADER
        //
        // Concerns:
        //: 1 'read' fails when magic characters are invalid
        //: 2 'read' fails when version number is invalid
        //: 3 'read' fails when version '\0' 'numIsGmt' is invalid
        //: 4 'read' fails when version '2' 'numIsGmt' is invalid
        //: 5 'read' fails when version '\0' 'numIsStd' is invalid
        //: 6 'read' fails when version '2' 'numIsStd' is invalid
        //: 7 'read' fails when version '\0' 'numLeaps' is invalid
        //: 8 'read' fails when version '2' 'numLeaps' is invalid
        //: 9 'read' fails when version '\0' 'numTransitions' is invalid
        //: 10 'read' fails when version '2' 'numTransitions' is invalid
        //: 11 'read' fails when version '\0' 'numLocalTimeTypes' is invalid
        //: 12 'read' fails when version '2' 'numLocalTimeTypes' is invalid
        //: 13 'read' fails when version '\0' 'abbrevDataSize' is invalid
        //: 14 'read' fails when version '2' 'abbrevDataSize' is invalid
        //
        // Plan:
        //: 1 Create a string stream with incorrect header information for each
        //:   variable in the header and verify that 'read' returns a non-zero
        //:   value.
        //
        // Testing:
        //   CONCERN: 'read' fails when header information is invalid
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'read' failure mode" << endl
                          << "===========================" << endl;

        if (verbose) cout << "\nInvalid header identifier." << endl;
        {
            ZoneinfoData ZI;
            ZI.getRawHeader()->setHeaderId(0, 'S');
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-2 == Obj::read(&TZ, inputStream));
        }

        {
            ZoneinfoData ZI;
            ZI.getRawHeader()->setHeaderId(3, 'F');
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-2 == Obj::read(&TZ, inputStream));
        }

        if (verbose) cout << "\nInvalid version." << endl;
        {
            ZoneinfoData ZI;
            ZI.getRawHeader()->setVersion(1);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-3 == Obj::read(&TZ, inputStream));
        }
        {
            ZoneinfoData ZI;
            ZI.getRawHeader()->setVersion('2' - 1);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-3 == Obj::read(&TZ, inputStream));
        }
        {
            ZoneinfoData ZI;
            ZI.getRawHeader()->setVersion('3' + 1);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-3 == Obj::read(&TZ, inputStream));
        }

        if (verbose) cout << "\nInvalid version '\\0' 'numIsGmt'." << endl;
        {
            ZoneinfoData ZI;
            ZI.getRawHeader()->setNumIsGmt(-1);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-5 == Obj::read(&TZ, inputStream));
        }

        if (verbose) cout << "\nInvalid version '2' 'numIsGmt'." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumIsGmt(-1);
            ZoneinfoData ZI(RH);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-5 == Obj::read(&TZ, inputStream));
        }

        if (verbose) cout << "\nInvalid version '\0' 'numIsStd'." << endl;
        {
            ZoneinfoData ZI;
            ZI.getRawHeader()->setNumIsStd(-1);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-6 == Obj::read(&TZ, inputStream));
        }

        if (verbose) cout << "\nInvalid version '2' 'numIsStd'." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumIsStd(-1);
            ZoneinfoData ZI(RH);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-6 == Obj::read(&TZ, inputStream));
        }

        if (verbose) cout << "\nInvalid version '\0' 'numTransitions'.\n";
        {
            ZoneinfoData ZI;
            ZI.getRawHeader()->setNumTransitions(-1);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-8 == Obj::read(&TZ, inputStream));
        }

        if (verbose) cout << "\nInvalid version '2' 'numTransitions'." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumTransitions(-1);
            ZoneinfoData ZI(RH);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-8 == Obj::read(&TZ, inputStream));
        }

        if (verbose) cout <<
                        "\nInvalid version '\\0' 'numLocalTimeTypes'." << endl;
        {
            ZoneinfoData ZI;
            ZI.getRawHeader()->setNumLocalTimeTypes(0);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-4 == Obj::read(&TZ, inputStream));
        }

        if (verbose) cout <<
                          "\nInvalid version '2' 'numLocalTimeTypes'." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumLocalTimeTypes(0);
            ZoneinfoData ZI(RH);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-4 == Obj::read(&TZ, inputStream));
        }

        if (verbose) cout <<
                            "\nInvalid version '\\0' 'abbrevDataSize'" << endl;
        {
            ZoneinfoData ZI;
            ZI.getRawHeader()->setAbbrevDataSize(0);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-9 == Obj::read(&TZ, inputStream));
        }

        if (verbose) cout << "\nInvalid version '2' 'abbrevDataSize'" << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setAbbrevDataSize(0);
            ZoneinfoData ZI(RH);
            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);
            baltzo::Zoneinfo TZ;
            ASSERT(-9 == Obj::read(&TZ, inputStream));
        }

        if (verbose) cout << "\nInvalid version '2' 'abbrevDataSize'" << endl;
        {
            RawHeader RH;
            RH.setNumIsGmt(0);
            RH.setNumIsStd(0);
            RH.setNumTransitions(0);
            RH.setNumLocalTimeTypes(1);
            RH.setAbbrevDataSize(1);

            RH.setVersion('2');
            ZoneinfoData ZI2(RH);

            RH.setVersion('3');
            ZoneinfoData ZI3(RH);

            // Testing scenario, when there aren't any symbols, except data in
            // the stream.
            {
                // Don't add last two characters ("\n\n" to the stream).

                bdlsb::FixedMemInStreamBuf isb2(ZI2.buffer(), ZI2.size() - 2);
                bdlsb::FixedMemInStreamBuf isb3(ZI3.buffer(), ZI3.size() - 2);
                bsl::istream               inputStream2(&isb2);
                bsl::istream               inputStream3(&isb3);
                baltzo::Zoneinfo           TZ2;
                baltzo::Zoneinfo           TZ3;

                ASSERT(-33 == Obj::read(&TZ2, inputStream2));
                ASSERT(-33 == Obj::read(&TZ3, inputStream3));
            }

            // Data part isn't newline-enclosed scenario.
            {
                // Change newline symbols, enclosing data part.

                ZI2.buffer()[ZI2.size() - 1] = 'a';
                ZI3.buffer()[ZI3.size() - 1] = 'a';

                bdlsb::FixedMemInStreamBuf isb2(ZI2.buffer(), ZI2.size() - 2);
                bdlsb::FixedMemInStreamBuf isb3(ZI3.buffer(), ZI3.size() - 2);
                bsl::istream               inputStream2(&isb2);
                bsl::istream               inputStream3(&isb3);
                baltzo::Zoneinfo           TZ2;
                baltzo::Zoneinfo           TZ3;

                ASSERT(-33 == Obj::read(&TZ2, inputStream2));
                ASSERT(-33 == Obj::read(&TZ3, inputStream3));
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING REAL LIFE DATA
        //
        // Concerns:
        //: 1 'read' function works for real-life zoneinfo binary file.
        //
        // Plan:
        //: 1 Call the 'read' function on a number of real-life Zoneinfo data.
        //:   Verify the retrieved data with 'verifyTimeZoneVersion2Format'.
        //:   The chosen time zones are:
        //:   o America/New_York -- due to common usage.
        //:   o Europe/London -- due to common usage.
        //:   o Asia/Tokyo -- due to common usage.
        //:   o Pacific/Kiritimati -- due change in offset that is over 24
        //:     hours.
        //:   o Pacific/Chatham -- due to various uncommon properties such as
        //:     greater than 12 hours in offset from UTC and non-whole hour
        //:     offset.
        //
        // Testing:
        //   CONCERN: 'read' retrieve a real-life Zoneinfo data
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING REAL LIFE DATA" << endl
                          << "======================" << endl;

        {
            static const struct {
                const unsigned char   *d_buffer;
                int                    d_size;
                int                    d_numLocalTimeTypes;
                int                    d_numIsGmt;
                int                    d_numIsStd;
                int                    d_numLeaps;
                int                    d_abbrevDataSize;
            } DATA [] = {

            //BUFFER           SIZE                  LLT  GMT  STD LEAP  ABB
            //------           ----                  ---  ---  --- ----  ---

            { NEW_YORK_DATA,   sizeof NEW_YORK_DATA,   5,   5,   5,   0,  20 },
            { LONDON_DATA,     sizeof LONDON_DATA,     8,   8,   8,   0,  17 },
            { TOKYO_DATA,      sizeof TOKYO_DATA,      5,   5,   5,   0,  16 },
            { KIRITIMATI_DATA, sizeof KIRITIMATI_DATA, 4,   4,   4,   0,   9 },
            { CHATHAM_DATA,    sizeof CHATHAM_DATA,    4,   4,   4,   0,  16 },
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const unsigned char *BUFFER  = DATA[ti].d_buffer;
                const int            SIZE    = DATA[ti].d_size;
                const int            LTT     = DATA[ti].d_numLocalTimeTypes;
                const int            IS_GMT  = DATA[ti].d_numIsGmt;
                const int            IS_STD  = DATA[ti].d_numIsStd;
                const int            LEAPS   = DATA[ti].d_numLeaps;
                const int            AB_SIZE = DATA[ti].d_abbrevDataSize;

                bdlsb::FixedMemInStreamBuf inStreamBuf(
                                        reinterpret_cast<const char *>(BUFFER),
                                        SIZE);
                bsl::istream inputStream(&inStreamBuf);

                baltzo::Zoneinfo TZ;
                BinHeader HD;
                ASSERT(0 == Obj::read(&TZ, &HD, inputStream));

                if (verbose) { T_ P(HD) }

                ASSERT('2'     == HD.version());
                ASSERT(LTT     == HD.numLocalTimeTypes());
                ASSERT(IS_GMT  == HD.numIsGmt());
                ASSERT(IS_STD  == HD.numIsStd());
                ASSERT(LEAPS   == HD.numLeaps());
                ASSERT(AB_SIZE == HD.abbrevDataSize());

                if (verbose) { T_ P(TZ) }

                ZoneinfoData ZI(reinterpret_cast<const char *>(BUFFER),
                                SIZE);
                ASSERT(0 == verifyTimeZoneVersion2Format(ZI, TZ, L_));
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING TIME ZONE STRING READING
        //
        // Concerns:
        //: 1 'read' succeeds in loading version '\0' data into a
        //:   'baltzo::Zoneinfo' object, but default value of time zone string
        //:   is stored.
        //:
        //: 2 'read' succeeds in loading of varios time zone strings from
        //:   version '2' binary data into a 'baltzo::Zoneinfo' object and
        //:   correct value of time zone string is stored.
        //:
        //: 3 'read' succeeds in loading of varios time zone strings from
        //:   version '3' binary data into a 'baltzo::Zoneinfo' object and
        //:   correct value of time zone string is stored.
        //
        // Plan:
        //: 1 Using test apparatus, create a stream, containing binary data for
        //:   valid 'baltzo::Zoneinfo' object of version '\0'.  Verify that
        //:   'read' successfully load data into a 'baltzo::Zoneinfo' object.
        //:   (C-1)
        //:
        //: 2 Using the table-driven technique, specify a set of (unique) time
        //:   zone strings.
        //:
        //: 3 For each row (representing a distinct string value, 'V') in the
        //:   table described in P-2:
        //:
        //:   1 Using test apparatus, create two streams, containing binary
        //:     data for two valid 'baltzo::Zoneinfo' objects of version '2'
        //:     and '3' and having 'V' as a time zone string.
        //:
        //:   2 Verify that 'read' successfully load data into a
        //:     'baltzo::Zoneinfo' object.  (C-2..3)
        //
        // Testing:
        //   read(baltzo::Zoneinfo *, bsl::istream&, bA=0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING TIME ZONE STRING READING" << endl
                          << "================================" << endl;

        if (verbose) cout << "\nTesting version '\0' binary file." << endl;
        {
            RawHeader RH;
            RH.setNumIsGmt(0);
            RH.setNumIsStd(0);
            RH.setNumTransitions(0);
            RH.setNumLocalTimeTypes(1);
            RH.setAbbrevDataSize(1);

            ZoneinfoData ZI(RH);

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream               inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 == Obj::read(&TZ, inputStream));
            ASSERT(0 == verifyTimeZoneVersion2Format(ZI, TZ, L_));
        }

        if (verbose) cout << "\nTesting version '2' and '3' binary file."
                          << endl;
        {

            static const struct {
                int         d_line;      // line
                const char *d_original;  // original string
                const char *d_result;    // result string
            } DATA[] = {
                //LINE  ORIGINAL   RESULT
                //----  --------   --------
                { L_,   "",        ""       },
                { L_,   "\n",      ""       },
                { L_,   "a",       "a"      },
                { L_,   "\na",     ""       },
                { L_,   "a\n",     "a"      },
                { L_,   "ab",      "ab"     },
                { L_,   "abcdef",  "abcdef" },
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_line;
                const char *ORIG   = DATA[ti].d_original;
                const char *RESULT = DATA[ti].d_result;

                RawHeader RH;
                RH.setNumIsGmt(0);
                RH.setNumIsStd(0);
                RH.setNumTransitions(0);
                RH.setNumLocalTimeTypes(1);
                RH.setAbbrevDataSize(1);

                for (int tj = 0; tj < 2; ++tj) {
                    char version = tj ? '2' : '3';

                    RH.setVersion(version);

                    ZoneinfoData original(RH);
                    ZoneinfoData result(RH);

                    original.setTimeZoneString(ORIG, strlen(ORIG));
                    result.setTimeZoneString(RESULT, strlen(RESULT));

                    bdlsb::FixedMemInStreamBuf isb(original.buffer(),
                                                   original.size());
                    bsl::istream               inputStream(&isb);

                    baltzo::Zoneinfo TZ;
                    LOOP2_ASSERT(LINE, version,
                                 0 == Obj::read(&TZ, inputStream));
                    LOOP2_ASSERT(LINE, version,
                                 0 == verifyTimeZoneVersion2Format(result,
                                                                   TZ,
                                                                   LINE));
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'read' without FileDescription
        //
        // Concerns:
        //: 1 'read' succeeds in loading from various configurations of
        //:   version '\0' data into a 'baltzo::Zoneinfo' object.
        //:
        //: 2 'read' succeeds in loading from various configurations of
        //:   version '2' binary data into a 'baltzo::Zoneinfo' object.
        //
        // Plan:
        //: 1 Repeat case 7 using read without FileDescription
        //
        // Testing:
        //   read(baltzo::Zoneinfo *, bsl::istream&, bA=0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'read'" << endl
                          << "==============" << endl;

        if (verbose) cout <<
                         "\nCreate a table of distinct object values." << endl;

        static const struct {
            int  d_line;
            int  d_numIsGmt;
            int  d_numIsStd;
            int  d_numTransitions;
            int  d_numLocalTimeTypes;
            int  d_abbrevDataSize;
        } DATA[] = {

        //LINE  IS_GMT  IS_STD  TRANS  LCL_T_TYPE  AB_DATA
        //----  ------  ------  -----  ----------  -------

        // depth = 0
        { L_,        0,      0,     0,          1,       1 },

        // depth = 1
        { L_,        1,      0,     0,          1,       1 },
        { L_,        0,      1,     0,          1,       1 },
        { L_,        0,      0,     1,          1,       1 },
        { L_,        0,      0,     0,          2,       1 },
        { L_,        0,      0,     0,          1,       2 },

        // depth = 2
        { L_,        1,      1,     0,          1,       1 },
        { L_,        1,      0,     1,          1,       1 },
        { L_,        1,      0,     0,          2,       1 },
        { L_,        1,      0,     0,          1,       2 },
        { L_,        0,      1,     1,          1,       1 },
        { L_,        0,      1,     0,          2,       1 },
        { L_,        0,      1,     0,          1,       2 },
        { L_,        0,      0,     1,          2,       1 },
        { L_,        0,      0,     1,          1,       2 },
        { L_,        0,      0,     0,          2,       2 },
        { L_,        2,      0,     0,          1,       1 },
        { L_,        0,      2,     0,          1,       1 },
        { L_,        0,      0,     2,          1,       1 },
        { L_,        0,      0,     0,          3,       1 },
        { L_,        0,      0,     0,          1,       3 },

        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nTesting version '\0'." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE       = DATA[ti].d_line;
                const int IS_GMT     = DATA[ti].d_numIsGmt;
                const int IS_STD     = DATA[ti].d_numIsStd;
                const int TRANS      = DATA[ti].d_numTransitions;
                const int LCL_T_TYPE = DATA[ti].d_numLocalTimeTypes;
                const int AB_DATA    = DATA[ti].d_abbrevDataSize;

                RawHeader RH;
                RH.setNumIsGmt(IS_GMT);
                RH.setNumIsStd(IS_STD);
                RH.setNumTransitions(TRANS);
                RH.setNumLocalTimeTypes(LCL_T_TYPE);
                RH.setAbbrevDataSize(AB_DATA);

                ZoneinfoData ZI(RH);

                bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
                bsl::istream inputStream(&isb);

                baltzo::Zoneinfo TZ;
                LOOP_ASSERT(LINE, 0 == Obj::read(&TZ, inputStream));
                LOOP_ASSERT(LINE, 0 == verifyTimeZone(ZI, TZ, LINE));
            }
        }

        if (verbose) cout << "\nTesting version '2'." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE       = DATA[ti].d_line;
                const int IS_GMT     = DATA[ti].d_numIsGmt;
                const int IS_STD     = DATA[ti].d_numIsStd;
                const int TRANS      = DATA[ti].d_numTransitions;
                const int LCL_T_TYPE = DATA[ti].d_numLocalTimeTypes;
                const int AB_DATA    = DATA[ti].d_abbrevDataSize;

                RawHeader RH;
                RH.setVersion('2');
                RH.setNumIsGmt(IS_GMT);
                RH.setNumIsStd(IS_STD);
                RH.setNumTransitions(TRANS);
                RH.setNumLocalTimeTypes(LCL_T_TYPE);
                RH.setAbbrevDataSize(AB_DATA);

                ZoneinfoData ZI(RH);

                bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
                bsl::istream stream(&isb);

                baltzo::Zoneinfo TZ;

                LOOP_ASSERT(LINE, 0 == Obj::read(&TZ, stream));
                LOOP_ASSERT(LINE, 0 ==
                                   verifyTimeZoneVersion2Format(ZI, TZ, LINE));
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'read' with FileDescription
        //
        // Concerns:
        //: 1 'read' succeeds in loading from various configurations of
        //:   version '\0' data into a 'baltzo::Zoneinfo' object and return
        //:   the correct header information.
        //:
        //: 2 'read' succeeds in loading from various configurations of
        //:   version '2' data into a 'baltzo::Zoneinfo' object and return
        //:   the correct header information.
        //
        // Plan:
        //: 1 Use a table driven approach with enumeration in five dimension
        //:   for each variable in the header, and verify that 'read'
        //:   successfully load data into a 'baltzo::Zoneinfo'.  process the
        //:   stream.
        //
        // Testing:
        //   read(baltzo::Zoneinfo *, FileDescription *, bsl::istream&, bA=0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'read'" << endl
                          << "==============" << endl;

        if (verbose) cout <<
                         "\nCreate a table of distinct object values." << endl;

        static const struct {
            int  d_line;
            int  d_numIsGmt;
            int  d_numIsStd;
            int  d_numTransitions;
            int  d_numLocalTimeTypes;
            int  d_abbrevDataSize;
        } DATA[] = {

        //LINE  IS_GMT  IS_STD  TRANS  LCL_T_TYPE  AB_DATA
        //----  ------  ------  -----  ----------  -------

        // depth = 0
        { L_,        0,      0,     0,          1,       1 },

        // depth = 1
        { L_,        1,      0,     0,          1,       1 },
        { L_,        0,      1,     0,          1,       1 },
        { L_,        0,      0,     1,          1,       1 },
        { L_,        0,      0,     0,          2,       1 },
        { L_,        0,      0,     0,          1,       2 },

        // depth = 2
        { L_,        1,      1,     0,          1,       1 },
        { L_,        1,      0,     1,          1,       1 },
        { L_,        1,      0,     0,          2,       1 },
        { L_,        1,      0,     0,          1,       2 },
        { L_,        0,      1,     1,          1,       1 },
        { L_,        0,      1,     0,          2,       1 },
        { L_,        0,      1,     0,          1,       2 },
        { L_,        0,      0,     1,          2,       1 },
        { L_,        0,      0,     1,          1,       2 },
        { L_,        0,      0,     0,          2,       2 },
        { L_,        2,      0,     0,          1,       1 },
        { L_,        0,      2,     0,          1,       1 },
        { L_,        0,      0,     2,          1,       1 },
        { L_,        0,      0,     0,          3,       1 },
        { L_,        0,      0,     0,          1,       3 },

        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nTesting version '\0'." << endl;

        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE       = DATA[ti].d_line;
                const int IS_GMT     = DATA[ti].d_numIsGmt;
                const int IS_STD     = DATA[ti].d_numIsStd;
                const int TRANS      = DATA[ti].d_numTransitions;
                const int LCL_T_TYPE = DATA[ti].d_numLocalTimeTypes;
                const int AB_DATA    = DATA[ti].d_abbrevDataSize;

                RawHeader RH;
                RH.setNumIsGmt(IS_GMT);
                RH.setNumIsStd(IS_STD);
                RH.setNumTransitions(TRANS);
                RH.setNumLocalTimeTypes(LCL_T_TYPE);
                RH.setAbbrevDataSize(AB_DATA);

                ZoneinfoData ZI(RH);

                bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
                bsl::istream inputStream(&isb);

                baltzo::Zoneinfo TZ;
                BinHeader HD;

                ASSERT(0 == Obj::read(&TZ, &HD, inputStream));

                ASSERT(0          == HD.version());
                ASSERT(LCL_T_TYPE == HD.numLocalTimeTypes());
                ASSERT(IS_GMT     == HD.numIsGmt());
                ASSERT(IS_STD     == HD.numIsStd());
                ASSERT(0          == HD.numLeaps());
                ASSERT(TRANS      == HD.numTransitions());
                ASSERT(AB_DATA    == HD.abbrevDataSize());

                LOOP_ASSERT(LINE, 0 == verifyTimeZone(ZI, TZ, LINE));
            }
        }

        if (verbose) cout << "\nTesting version '2'." << endl;

        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE       = DATA[ti].d_line;
                const int IS_GMT     = DATA[ti].d_numIsGmt;
                const int IS_STD     = DATA[ti].d_numIsStd;
                const int TRANS      = DATA[ti].d_numTransitions;
                const int LCL_T_TYPE = DATA[ti].d_numLocalTimeTypes;
                const int AB_DATA    = DATA[ti].d_abbrevDataSize;

                RawHeader RH;
                RH.setVersion('2');
                RH.setNumIsGmt(IS_GMT);
                RH.setNumIsStd(IS_STD);
                RH.setNumTransitions(TRANS);
                RH.setNumLocalTimeTypes(LCL_T_TYPE);
                RH.setAbbrevDataSize(AB_DATA);

                ZoneinfoData ZI(RH);

                bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
                bsl::istream inputStream(&isb);

                baltzo::Zoneinfo TZ;
                BinHeader HD;

                ASSERT(0 == Obj::read(&TZ, &HD, inputStream));

                ASSERT('2'        == HD.version());
                ASSERT(LCL_T_TYPE == HD.numLocalTimeTypes());
                ASSERT(IS_GMT     == HD.numIsGmt());
                ASSERT(IS_STD     == HD.numIsStd());
                ASSERT(0          == HD.numLeaps());
                ASSERT(TRANS      == HD.numTransitions());
                ASSERT(AB_DATA    == HD.abbrevDataSize());

                LOOP_ASSERT(LINE, 0 ==
                                   verifyTimeZoneVersion2Format(ZI, TZ, LINE));
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING MULTIPLE TRANSITIONS
        //
        // Concerns:
        //: 1 'read' succeeds in reading the version '\0' binary data with
        //:   multiple transitions.
        //:
        //: 2 'read' succeeds in reading the version '2' binary data with
        //:   multiple transitions.
        //
        // Plan:
        //: 1 Use a table-based approach to create a string stream with
        //:   multiple transitions and local time types.
        //
        // Testing:
        //   CONCERN: 'read' retrieve data with multiple transitions
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING TRANSITION" << endl
                          << "==================" << endl;

        if (verbose) cout <<
                      "\nCreate a table of distinct transition times." << endl;

        static const struct {
            int   d_line;
            int   d_numTransitions;
        } DATA[] = {

        //LINE  NUM_TRANSITIONS
        //----  ---------------

        { L_,                 0 },
        { L_,                 1 },
        { L_,                 2 },
        { L_,                 3 },
        { L_,                 4 },
        { L_,                 5 },
        { L_,                 6 },
        { L_,                 7 },
        { L_,                 8 },
        { L_,                 9 },
        { L_,                10 },
        { L_,                11 },
        { L_,                12 },
        { L_,                13 },
        { L_,                14 },
        { L_,                15 },
        { L_,                16 },
        { L_,                17 },
        { L_,                18 },
        { L_,                19 },
        { L_,                20 },

        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        static const struct {
            int   d_line;
            int   d_numLocalTimeTypes;
        } U_DATA[] = {

        //LINE  NUM_LLT
        //----  -------

        { L_,         1 },
        { L_,         2 },
        { L_,         5 },
        { L_,        10 },
        { L_,        15 },
        { L_,        20 },

        };

        enum { NUM_U_DATA = sizeof U_DATA / sizeof *U_DATA };

        if (verbose) cout <<
                         "\nTesting version '\0' multiple transitions.\n";
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                for (int i = 0; i < NUM_U_DATA; ++i) {
                    const int   LINE      = DATA[ti].d_line;
                    const int   NUM_TRANS = DATA[ti].d_numTransitions;
                    const int   LLT_LINE  = U_DATA[i].d_line;
                    const int   NUM_LLT   = U_DATA[i].d_numLocalTimeTypes;

                    RawHeader RH;
                    RH.setNumTransitions(NUM_TRANS);
                    RH.setNumLocalTimeTypes(NUM_LLT);

                    ZoneinfoData ZI(RH);

                    bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
                    bsl::istream inputStream(&isb);

                    baltzo::Zoneinfo TZ;
                    LOOP2_ASSERT(LINE, LLT_LINE,
                                 0 == Obj::read(&TZ, inputStream));
                    LOOP2_ASSERT(LINE, LLT_LINE,
                                 0 == verifyTimeZone(ZI, TZ, LINE));
                }
            }
        }

        if (verbose) cout <<
                         "\nTesting version '2' multiple transitions." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                for (int i = 0; i < NUM_U_DATA; ++i) {
                    const int   LINE      = DATA[ti].d_line;
                    const int   NUM_TRANS = DATA[ti].d_numTransitions;
                    const int   LLT_LINE  = U_DATA[i].d_line;
                    const int   NUM_LLT   = U_DATA[i].d_numLocalTimeTypes;

                    RawHeader RH;
                    RH.setVersion('2');
                    RH.setNumTransitions(NUM_TRANS);
                    RH.setNumLocalTimeTypes(NUM_LLT);

                    ZoneinfoData ZI(RH);

                    bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
                    bsl::istream inputStream(&isb);

                    baltzo::Zoneinfo TZ;
                    LOOP2_ASSERT(LINE, LLT_LINE,
                                             0 == Obj::read(&TZ, inputStream));
                    LOOP2_ASSERT(LINE, LLT_LINE,
                              0 == verifyTimeZoneVersion2Format(ZI, TZ, LINE));
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'read' FOR TRANSITIONS
        //
        // Concerns:
        //: 1 'read' succeeds in retrieving version '\0' transition data.
        //:
        //: 2 'read' succeeds in retrieving version '2' transition data.
        //:
        //: 3 'read' fails when there are duplicated version '\0' transition
        //:   times.
        //:
        //: 4 'read' fails when there are duplicated version '2' transition
        //:   times.
        //:
        //: 5 'read' fails when version '\0' transitions are not in ascending
        //:   order of time.
        //:
        //: 6 'read' fails when version '2' transitions are not in ascending
        //:   order of time.
        //:
        //: 7 'read' fails when a version '\0' index to local time type is out
        //:   of bound.
        //:
        //: 8 'read' fails when a version '2' index to local time type is out
        //:   of bound.
        //:
        //
        // Plan:
        //: 1 Use a table driven approach to create a string stream with
        //:   boundary values of version '\0' transition times, and verify that
        //:   'read' successfully load data into a 'baltzo::Zoneinfo'.
        //:
        //: 2 Use a table driven approach to create a string stream with
        //:   boundary values of version '2' transition times, and verify that
        //:   'read' successfully load data into a 'baltzo::Zoneinfo'.
        //:
        //: 3 Create a string stream with transition times that are not in
        //:   ascending out of order, and verify that 'read' returns a non-zero
        //:   value.
        //:
        //: 4 Create a string stream with duplicated transition times, and
        //:   verify that 'read' returns a non-zero value.
        //:
        //: 5 Create a string stream with transition that refers to a
        //:   local-time type that is out of bound, and verify that 'read'
        //:   returns a non-zero value.
        //
        // Testing:
        //   CONCERN: 'read' properly process transition data
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING TRANSITION" << endl
                          << "==================" << endl;

        if (verbose) cout <<
                      "\nCreate a table of distinct transition times." << endl;

        static const struct {
            int                  d_line;
            int                  d_transitionTime;
            bsls::Types::Int64   d_transitionTime64;
        } DATA[] = {

        //LINE  TRANSITION
        //----  ----------

        { L_,            0,                    0LL },
        { L_,            1,                    1LL },
        { L_,           -1,                   -1LL },
        { L_,   0x01234567,           0x01234567LL },
        { L_,   0x76543210,           0x76543210LL },
        { L_,      INT_MIN,
                          static_cast<bsls::Types::Int64>(0xFFFFFFFF7C558180LL)
                                                   },  // "01/01/0000 00:00:00"
        { L_,      INT_MAX,         0x3AFFF4417FLL },  // "12/31/9999 23:59:59"

        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout << "\nTesting version '\0' transition time." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE       = DATA[ti].d_line;
                const int   TRANSITION = DATA[ti].d_transitionTime;

                RawHeader RH;
                RH.setNumTransitions(1);

                ZoneinfoData ZI(RH);
                bdlb::BigEndianInt32 *transitions = ZI.getTransitionTime();
                transitions[0] = TRANSITION;

                bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
                bsl::istream inputStream(&isb);

                baltzo::Zoneinfo TZ;
                LOOP_ASSERT(LINE, 0 == Obj::read(&TZ, inputStream));
                LOOP_ASSERT(LINE, 0 == verifyTimeZone(ZI, TZ, LINE));

                baltzo::Zoneinfo::TransitionConstIterator iter =
                                                         TZ.beginTransitions();
                ++iter;

                LOOP2_ASSERT(LINE, iter->utcTime(),
                             TRANSITION == iter->utcTime());
            }
        }

        if (verbose) cout << "\nTesting version '2' transition time." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int                  LINE       = DATA[ti].d_line;
                const bsls::Types::Int64   TRANSITION =
                                                  DATA[ti].d_transitionTime64;

                RawHeader RH;
                RH.setVersion('2');
                RH.setNumTransitions(1);

                ZoneinfoData ZI(RH);
                bdlb::BigEndianInt64 *transitions = ZI.getTransitionTime64();
                transitions[0] = TRANSITION;

                bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
                bsl::istream inputStream(&isb);

                baltzo::Zoneinfo TZ;
                LOOP_ASSERT(LINE, 0 == Obj::read(&TZ, inputStream));
                LOOP_ASSERT(LINE, 0 ==
                                   verifyTimeZoneVersion2Format(ZI, TZ, LINE));

                baltzo::Zoneinfo::TransitionConstIterator iter =
                                                         TZ.beginTransitions();
                ++iter;

                LOOP2_ASSERT(LINE, iter->utcTime(),
                             TRANSITION == iter->utcTime());
            }
        }

        if (verbose) cout <<
                  "\nTesting version '\0' unordered transition index." << endl;
        {
            RawHeader RH;
            RH.setNumTransitions(2);
            RH.setNumLocalTimeTypes(2);

            ZoneinfoData ZI(RH);
            unsigned char *indexes = ZI.getTransitionIndex();
            indexes[0] = 1;
            indexes[1] = 0;

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 == Obj::read(&TZ, inputStream));
            ASSERT(0 == verifyTimeZone(ZI, TZ, L_));
        }

        if (verbose) cout <<
                   "\nTesting version '2' unordered transition index." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumTransitions(2);
            RH.setNumLocalTimeTypes(2);

            ZoneinfoData ZI(RH);
            unsigned char *indexes = ZI.getTransitionIndex64();
            indexes[0] = 1;
            indexes[1] = 0;

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 == Obj::read(&TZ, inputStream));
            ASSERT(0 == verifyTimeZoneVersion2Format(ZI, TZ, L_));
        }

        if (verbose) cout <<
                  "\nTesting version '\0' duplicated transition time." << endl;
        {
            RawHeader RH;
            RH.setNumTransitions(2);

            ZoneinfoData ZI(RH);
            bdlb::BigEndianInt32 *transitions = ZI.getTransitionTime();
            transitions[0] = 0;
            transitions[1] = 0;

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 != Obj::read(&TZ, inputStream));
        }

        if (verbose) cout <<
                   "\nTesting version '2' duplicated transition time." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumTransitions(2);

            ZoneinfoData ZI(RH);
            bdlb::BigEndianInt64 *transitions = ZI.getTransitionTime64();
            transitions[0] = 0;
            transitions[1] = 0;

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 != Obj::read(&TZ, inputStream));
        }

        if (verbose) cout <<
                "\nTesting version '\0' out of order transition time." << endl;
        {
            RawHeader RH;
            RH.setNumTransitions(2);

            ZoneinfoData ZI(RH);
            bdlb::BigEndianInt32 *transitions = ZI.getTransitionTime();
            transitions[0] = 1;
            transitions[1] = 0;

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 != Obj::read(&TZ, inputStream));
        }

        if (verbose) cout <<
                 "\nTesting version '2' out of order transition time." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumTransitions(2);

            ZoneinfoData ZI(RH);
            bdlb::BigEndianInt64 *transitions = ZI.getTransitionTime64();
            transitions[0] = 1;
            transitions[1] = 0;

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 != Obj::read(&TZ, inputStream));
        }

        if (verbose) cout <<
            "\nTesting version '\0' duplicated local time type index." << endl;
        {
            RawHeader RH;
            RH.setNumTransitions(2);

            ZoneinfoData ZI(RH);
            unsigned char *index = ZI.getTransitionIndex();
            index[0] = 0;
            index[1] = 0;

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 == Obj::read(&TZ, inputStream));
            ASSERT(0 == verifyTimeZone(ZI, TZ, L_));
        }

        if (verbose) cout <<
             "\nTesting version '2' duplicated local time type index." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumTransitions(2);

            ZoneinfoData ZI(RH);
            unsigned char *index = ZI.getTransitionIndex64();
            index[0] = 0;
            index[1] = 0;

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 == Obj::read(&TZ, inputStream));
            ASSERT(0 == verifyTimeZoneVersion2Format(ZI, TZ, L_));
        }

        if (verbose) cout <<
                    "\nTesting version '\0' invalid local time index." << endl;
        {
            RawHeader RH;
            RH.setNumTransitions(1);

            ZoneinfoData ZI(RH);
            unsigned char *index = ZI.getTransitionIndex();
            index[0] = ZI.getRawHeader()->numLocalTimeTypes();

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 != Obj::read(&TZ, inputStream));
        }

        if (verbose) cout <<
                     "\nTesting version '2' invalid local time index." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumTransitions(1);

            ZoneinfoData ZI(RH);
            unsigned char *index = ZI.getTransitionIndex64();
            index[0] = ZI.getRawHeader64()->numLocalTimeTypes();

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 != Obj::read(&TZ, inputStream));
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'read' FOR LOCAL TIME TYPES
        //
        // Concerns:
        //: 1 'read' succeeds in retrieving version '\0' local time types data.
        //:
        //: 2 'read' succeeds in retrieving version '2' local time types data.
        //:
        //: 3 'read' succeeds with version '\0' duplicated local time types.
        //:
        //: 4 'read' succeeds with version '2' duplicated local time types.
        //:
        //: 5 'read' fails when a UTC offset is greater than 24 hours.
        //:
        //: 6 'read' fails when the abbreviation index is out of bound.
        //
        // Plan:
        //: 1 Use a table driven approach to create a string stream with
        //:   boundary values of a local time type, and verify that
        //:   'read' successfully load data into a 'baltzo::Zoneinfo'.
        //:
        //: 2 Create a string stream with duplicated local time types,
        //:   and verify that 'read' successfully load data into a
        //:   'baltzo::Zoneinfo'.
        //:
        //: 3 Create a string stream with UTC offset greater than 24 hours, and
        //:   verify that 'read' returns a non-zero value.
        //:
        //: 4 Create a string stream with local-time type that refers to
        //:   an abbreviation string index that is out of bound, and verify
        //:   that 'read' returns a non-zero value.
        //
        // Testing:
        //   CONCERN: 'read' properly process local time types data
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING LOCAL TIME TYPES" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                         "\nCreate a table of distinct object values." << endl;

        const int UTC_MIN = -4 * 60 * 60 + 1;
        const int UTC_MAX =  4 * 60 * 60 - 1;

        static const struct {
            int         d_line;
            int         d_offset;
            int         d_dstFlag;
            int         d_descIndex;
            const char *d_abbrevData;
            int         d_abbrevDataSize;
        } DATA[] = {

        //LINE   OFFSET  DST  DESC_INDEX  AB_DATA  AB_DATE_SIZE
        //----   ------  ---  ----------  -------  ------------

        { L_,         0,   0,          0, "",                 1},
        { L_,   UTC_MIN,   0,          0, "",                 1},
        { L_,   UTC_MAX,   0,          0, "",                 1},
        { L_,         0,   1,          0, "",                 1},
        { L_,         0,   0,          0, "A",                2},
        { L_,         0,   0,          1, "A",                2},
        { L_,         0,   0,          2, "A\0BC",            5},

        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout <<
                            "\nTesting version '\0' local time types." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE         = DATA[ti].d_line;
                const int   OFFSET       = DATA[ti].d_offset;
                const int   DST          = DATA[ti].d_dstFlag;
                const int   DESC_IDX     = DATA[ti].d_descIndex;
                const char *AB_DATA      = DATA[ti].d_abbrevData;
                const int   AB_DATA_SIZE = DATA[ti].d_abbrevDataSize;

                RawHeader RH;
                RH.setAbbrevDataSize(AB_DATA_SIZE);

                ZoneinfoData ZI(RH);
                RawLocalTimeTypes& LTT = *ZI.getRawLocalTimeTypes();
                LTT.setOffset(OFFSET);
                LTT.setIsDst(DST);
                LTT.setAbbreviationIndex(DESC_IDX);
                memcpy(ZI.getAbbrevData(), AB_DATA, AB_DATA_SIZE);

                bdlsb::FixedMemInStreamBuf isb(ZI.buffer(),
                                              ZI.size());
                bsl::istream inputStream(&isb);

                baltzo::Zoneinfo TZ;

                LOOP_ASSERT(LINE, 0 == Obj::read(&TZ, inputStream));
                LOOP_ASSERT(LINE, 0 == verifyTimeZone(ZI, TZ, LINE));
            }
        }

        if (verbose) cout << "\nTesting version '2' local time types." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE         = DATA[ti].d_line;
                const int   OFFSET       = DATA[ti].d_offset;
                const int   DST          = DATA[ti].d_dstFlag;
                const int   DESC_IDX     = DATA[ti].d_descIndex;
                const char *AB_DATA      = DATA[ti].d_abbrevData;
                const int   AB_DATA_SIZE = DATA[ti].d_abbrevDataSize;

                RawHeader RH;
                RH.setVersion('2');
                RH.setAbbrevDataSize(AB_DATA_SIZE);

                ZoneinfoData ZI(RH);
                RawLocalTimeTypes& LTT = *ZI.getRawLocalTimeTypes64();
                LTT.setOffset(OFFSET);
                LTT.setIsDst(DST);
                LTT.setAbbreviationIndex(DESC_IDX);
                memcpy(ZI.getAbbrevData64(), AB_DATA, AB_DATA_SIZE);

                bdlsb::FixedMemInStreamBuf isb(ZI.buffer(),
                                              ZI.size());
                bsl::istream inputStream(&isb);

                baltzo::Zoneinfo TZ;

                LOOP_ASSERT(LINE, 0 == Obj::read(&TZ, inputStream));
                LOOP_ASSERT(LINE, 0 ==
                                   verifyTimeZoneVersion2Format(ZI, TZ, LINE));
            }
        }

        if (verbose) cout <<
                 "\nTesting version '\0' duplicated local time types." << endl;
        {
            RawHeader RH;
            RH.setNumTransitions(3);
            RH.setNumLocalTimeTypes(2);

            ZoneinfoData ZI(RH);
            RawLocalTimeTypes *lttPtr = ZI.getRawLocalTimeTypes();
            RawLocalTimeTypes& LTT0 = lttPtr[0];
            LTT0.setOffset(0);
            LTT0.setIsDst(0);
            LTT0.setAbbreviationIndex(0);
            RawLocalTimeTypes& LTT1 = lttPtr[1];
            LTT1.setOffset(0);
            LTT1.setIsDst(0);
            LTT1.setAbbreviationIndex(0);

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 == Obj::read(&TZ, inputStream));
            ASSERT(0 == verifyTimeZone(ZI, TZ, L_));
        }

        if (verbose) cout <<
                  "\nTesting version '2' duplicated local time types." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumTransitions(3);
            RH.setNumLocalTimeTypes(2);

            ZoneinfoData ZI(RH);
            RawLocalTimeTypes *lttPtr = ZI.getRawLocalTimeTypes64();
            RawLocalTimeTypes& LTT0 = lttPtr[0];
            LTT0.setOffset(0);
            LTT0.setIsDst(0);
            LTT0.setAbbreviationIndex(0);
            RawLocalTimeTypes& LTT1 = lttPtr[1];
            LTT1.setOffset(0);
            LTT1.setIsDst(0);
            LTT1.setAbbreviationIndex(0);

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 == Obj::read(&TZ, inputStream));
            ASSERT(0 == verifyTimeZoneVersion2Format(ZI, TZ, L_));
        }

        if (verbose) cout << "\nTesting invalid UTC offset." << endl;
        {
            ZoneinfoData ZI;
            RawLocalTimeTypes& LTT = *ZI.getRawLocalTimeTypes();
            LTT.setOffset(-86400);

            {
                bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
                bsl::istream inputStream(&isb);

                baltzo::Zoneinfo TZ;
                ASSERT(0 != Obj::read(&TZ, inputStream));
            }

            LTT.setOffset(86400);

            {
                bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
                bsl::istream inputStream(&isb);

                baltzo::Zoneinfo TZ;
                ASSERT(0 != Obj::read(&TZ, inputStream));
            }
        }

        if (verbose) cout <<
                        "\nTesting invalid abbreviation string index." << endl;
        {
            ZoneinfoData ZI;
            RawLocalTimeTypes& LTT = *ZI.getRawLocalTimeTypes();
            LTT.setAbbreviationIndex(ZI.getRawHeader()->abbrevDataSize());

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 != Obj::read(&TZ, inputStream));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'read' FOR ABBREVIATION STRINGS
        //
        // Concerns:
        //: 1 'read' successfully retrieves the version '\0' abbreviation
        //:   strings.
        //:
        //: 2 'read' successfully retrieves the version '2' abbreviation
        //:   strings.
        //:
        //: 3 'read' successfully retrieves the version '\0' abbreviation
        //:   strings when the abbreviation string indexes in the local time
        //:   descriptors are not in order.
        //:
        //: 4 'read' successfully retrieves the version '2' abbreviation
        //:   strings when the abbreviation string indexes in the local time
        //:   descriptors are not in order.
        //:
        //: 5 'read' fails when the version '\0' abbreviation string is not
        //:   null terminated.
        //:
        //: 6 'read' fails when the version '2' abbreviation string is not
        //:   null terminated.
        //
        // Plan:
        //: 1 Use a table-based approach and test that local time descriptors
        //:   retrieved using the 'read' function has the expected abbreviation
        //:   string.
        //:
        //: 2 Use a table-based approach and create binary data where the
        //:   abbreviation string index of the local time descriptors are in
        //:   reversed order.  Test that descriptors retrieved using the 'read'
        //:   function has the expected abbreviation string.
        //:
        //: 3 Create a string stream with abbreviation string where the last
        //:   character of the abbreviation string buffer is not null, and
        //:   verify that 'read' returns a non-zero value.
        //
        // Testing:
        //   CONCERN: 'read' properly process the abbreviation strings
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'read' FOR ABBREVIATION STRINGS"
                          << "=======================================" << endl;

        const char AB_DATA[] = "A\0BC\0DEF\0";

        static const struct {
            int d_line;
            const char *d_abbrevString;
        } DATA[] = {

        //LINE  AB_STRING
        //----  ---------

        { L_,   "A" },
        { L_,   "" },
        { L_,   "BC" },
        { L_,   "C" },
        { L_,   "" },
        { L_,   "DEF" },
        { L_,   "EF" },
        { L_,   "F" },
        { L_,   "" },

        };

        enum { AB_DATA_SIZE = sizeof AB_DATA / sizeof *AB_DATA };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout
                        << "\nTesting version '\0' abbreviation data." << endl;
        {
            RawHeader RH;
            RH.setNumTransitions(AB_DATA_SIZE);
            RH.setNumLocalTimeTypes(AB_DATA_SIZE);
            RH.setAbbrevDataSize(AB_DATA_SIZE);

            ZoneinfoData ZI(RH);

            memcpy(ZI.getAbbrevData(), AB_DATA, AB_DATA_SIZE);

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 == Obj::read(&TZ, inputStream));
            ASSERT(0 == verifyTimeZone(ZI, TZ, L_));

            baltzo::Zoneinfo::TransitionConstIterator iter =
                                                         TZ.beginTransitions();

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const char *AB_STRING = DATA[ti].d_abbrevString;

                ++iter;
                if (iter == TZ.endTransitions()) {
                    LOOP_ASSERT(ti, !"Unexpected number of transitions");
                    continue;
                }

                LOOP_ASSERT(LINE,
                            AB_STRING == iter->descriptor().description());
            }
        }

        if (verbose) cout
                         << "\nTesting version '2' abbreviation data." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumTransitions(AB_DATA_SIZE);
            RH.setNumLocalTimeTypes(AB_DATA_SIZE);
            RH.setAbbrevDataSize(AB_DATA_SIZE);

            ZoneinfoData ZI(RH);

            memcpy(ZI.getAbbrevData64(), AB_DATA, AB_DATA_SIZE);

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 == Obj::read(&TZ, inputStream));
            ASSERT(0 == verifyTimeZoneVersion2Format(ZI, TZ, L_));

            baltzo::Zoneinfo::TransitionConstIterator iter =
                                                         TZ.beginTransitions();

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const char *AB_STRING = DATA[ti].d_abbrevString;

                ++iter;
                if (iter == TZ.endTransitions()) {
                    LOOP_ASSERT(ti, !"Unexpected number of transitions");
                    continue;
                }

                LOOP_ASSERT(LINE,
                            AB_STRING == iter->descriptor().description());
            }
        }

        if (verbose) cout <<
      "\nTesting version '\0' out of order abbreviation string index." << endl;
        {
            RawHeader RH;
            RH.setNumTransitions(AB_DATA_SIZE);
            RH.setNumLocalTimeTypes(AB_DATA_SIZE);
            RH.setAbbrevDataSize(AB_DATA_SIZE);

            ZoneinfoData ZI(RH);

            memcpy(ZI.getAbbrevData(), AB_DATA, AB_DATA_SIZE);

            // Set abbreviation indexes in reverse order.

            RawLocalTimeTypes *localTimeTypes = ZI.getRawLocalTimeTypes();
            for (int i = 0; i < AB_DATA_SIZE; ++i) {
                localTimeTypes[i].setAbbreviationIndex(AB_DATA_SIZE - 1 - i);
            }

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 == Obj::read(&TZ, inputStream));
            ASSERT(0 == verifyTimeZone(ZI, TZ, L_));

            baltzo::Zoneinfo::TransitionConstIterator iter =
                                                           TZ.endTransitions();

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const char *AB_STRING = DATA[ti].d_abbrevString;

                --iter;
                if (iter == TZ.beginTransitions()) {
                    LOOP_ASSERT(ti, !"Unexpected number of transitions");
                    continue;
                }

                LOOP_ASSERT(LINE,
                            AB_STRING == iter->descriptor().description());
            }
        }

        if (verbose) cout <<
       "\nTesting version '2' out of order abbreviation string index." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumTransitions(AB_DATA_SIZE);
            RH.setNumLocalTimeTypes(AB_DATA_SIZE);
            RH.setAbbrevDataSize(AB_DATA_SIZE);

            ZoneinfoData ZI(RH);

            memcpy(ZI.getAbbrevData64(), AB_DATA, AB_DATA_SIZE);

            // Set abbreviation indexes in reverse order.

            RawLocalTimeTypes *localTimeTypes = ZI.getRawLocalTimeTypes64();
            for (int i = 0; i < AB_DATA_SIZE; ++i) {
                localTimeTypes[i].setAbbreviationIndex(AB_DATA_SIZE - 1 - i);
            }

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 == Obj::read(&TZ, inputStream));
            ASSERT(0 == verifyTimeZoneVersion2Format(ZI, TZ, L_));

            baltzo::Zoneinfo::TransitionConstIterator iter =
                                                           TZ.endTransitions();

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const char *AB_STRING = DATA[ti].d_abbrevString;

                --iter;
                if (iter == TZ.beginTransitions()) {
                    LOOP_ASSERT(ti, !"Unexpected number of transitions");
                    continue;
                }

                LOOP_ASSERT(LINE,
                            AB_STRING == iter->descriptor().description());
            }
        }

        if (verbose) cout
               << "\nTesting version '\0' non-null terminated string." << endl;
        {
            ZoneinfoData ZI;

            char *abbrevData = ZI.getAbbrevData();
            abbrevData[ZI.getRawHeader()->abbrevDataSize() - 1] = 'A';

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 != Obj::read(&TZ, inputStream));
        }

        if (verbose) cout
                << "\nTesting version '2' non-null terminated string." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');

            ZoneinfoData ZI(RH);

            char *abbrevData = ZI.getAbbrevData64();
            abbrevData[ZI.getRawHeader64()->abbrevDataSize() - 1] = 'A';

            bdlsb::FixedMemInStreamBuf isb(ZI.buffer(), ZI.size());
            bsl::istream inputStream(&isb);

            baltzo::Zoneinfo TZ;
            ASSERT(0 != Obj::read(&TZ, inputStream));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING APPARATUS
        //
        // Concerns:
        //: 1 A default constructed 'ZoneinfoData' object have the expected
        //:   header values and size.
        //:
        //: 2 A value constructed version '\0' 'ZoneinfoData' object has the
        //:   expected header values and size.
        //:
        //: 2 A value constructed version '2' 'ZoneinfoData' object has the
        //:   expected header values and size.
        //:
        //: 3 'ZoneinfoData' creates a buffer with expected version '\0' data.
        //:
        //: 3 'ZoneinfoData' creates a buffer with expected version '2' data.
        //:
        //: 4 'verifyTimeZone' correctly reports whether a 'baltzo::Zoneinfo'
        //:   matches the version '\0' Zoneinfo binary data.
        //:
        //: 4 'verifyTimeZoneVersion2Format' correctly reports whether a
        //:   'baltzo::Zoneinfo' matches the version '2' Zoneinfo binary data.
        //
        // Plan:
        //: 1 Test that a default constructed 'ZoneinfoData' object have the
        //:   expected header values and size
        //:
        //: 2 Use a table-based approach and test that a value constructed
        //:   'ZoneinfoData' object have the expected header values and size.
        //:
        //: 3 Test that the buffer in 'ZoneinfoData' object is created as
        //:   expected.
        //:
        //: 4 Use an ad hoc approach to test various edge cases of the
        //:   function.
        //
        // Testing:
        //   CONCERN: Test apparatus functions as documented.
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING 'ZoneinfoData" << endl
                          << "=====================" << endl;

        if (verbose)
                  cout << "\nTesting default values of 'ZoneinfoData'" << endl;
        {
            ZoneinfoData ZI;
            BinHeader HD;
            const RawHeader& ZIH = *ZI.getRawHeader();

            ASSERT(HD.numIsGmt()          == ZIH.numIsGmt());
            ASSERT(HD.numIsStd()          == ZIH.numIsStd());
            ASSERT(HD.numTransitions()    == ZIH.numTransitions());
            ASSERT(HD.numLocalTimeTypes() == ZIH.numLocalTimeTypes());
            ASSERT(HD.abbrevDataSize()    == ZIH.abbrevDataSize());
        }

        if (verbose) cout <<
            "\nCreate a table of version '\0' distinct object values." << endl;

        static const struct {
            int  d_line;
            int  d_numIsGmt;
            int  d_numIsStd;
            int  d_numTransitions;
            int  d_numLocalTimeTypes;
            int  d_abbrevDataSize;
            int  d_size;
        } DATA[] = {

        //LINE  IS_GMT  IS_STD  TRANS  LCL_T_TYPE  AB_DATA  SIZE
        //----  ------  ------  -----  ----------  -------  ----

        { L_,        0,      0,     0,          1,       1,   51 },
        { L_,        1,      0,     0,          1,       1,   52 },
        { L_,        0,      1,     0,          1,       1,   52 },
        { L_,        0,      0,     1,          1,       1,   56 },
        { L_,        0,      0,     0,          2,       1,   57 },
        { L_,        0,      0,     0,          1,       2,   52 },

        };

        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        if (verbose) cout <<
      "\nTesting basic accessor for version '\0' header information.." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE       = DATA[ti].d_line;
                const int IS_GMT     = DATA[ti].d_numIsGmt;
                const int IS_STD     = DATA[ti].d_numIsStd;
                const int TRANS      = DATA[ti].d_numTransitions;
                const int LCL_T_TYPE = DATA[ti].d_numLocalTimeTypes;
                const int AB_DATA    = DATA[ti].d_abbrevDataSize;
                const int SIZE       = DATA[ti].d_size;

                RawHeader RH;
                RH.setNumIsGmt(IS_GMT);
                RH.setNumIsStd(IS_STD);
                RH.setNumTransitions(TRANS);
                RH.setNumLocalTimeTypes(LCL_T_TYPE);
                RH.setAbbrevDataSize(AB_DATA);

                ZoneinfoData ZI(RH);

                LOOP3_ASSERT(LINE, SIZE, ZI.size(), SIZE == ZI.size());

                const RawHeader& ZIH = *ZI.getRawHeader();

                LOOP3_ASSERT(LINE, IS_GMT, ZIH.numIsGmt(),
                             IS_GMT == ZIH.numIsGmt());
                LOOP3_ASSERT(LINE, IS_STD, ZIH.numIsStd(),
                             IS_STD == ZIH.numIsStd());
                LOOP3_ASSERT(LINE, TRANS, ZIH.numTransitions(),
                             TRANS == ZIH.numTransitions());
                LOOP3_ASSERT(LINE, LCL_T_TYPE, ZIH.numLocalTimeTypes(),
                             LCL_T_TYPE == ZIH.numLocalTimeTypes());
                LOOP3_ASSERT(LINE, AB_DATA, ZIH.abbrevDataSize(),
                             AB_DATA == ZIH.abbrevDataSize());
            }
        }

        if (verbose) cout <<
             "\nCreate a table of version '2' distinct object values." << endl;

        static const struct {
            int  d_line;
            int  d_numIsGmt;
            int  d_numIsStd;
            int  d_numTransitions;
            int  d_numLocalTimeTypes;
            int  d_abbrevDataSize;
            int  d_size;
        } DATA64[] = {

        //LINE  IS_GMT  IS_STD  TRANS  LCL_T_TYPE  AB_DATA  SIZE
        //----  ------  ------  -----  ----------  -------  ----

        { L_,        0,      0,     0,          1,       1,   104 },
        { L_,        1,      0,     0,          1,       1,   106 },
        { L_,        0,      1,     0,          1,       1,   106 },
        { L_,        0,      0,     1,          1,       1,   118 },
        { L_,        0,      0,     0,          2,       1,   116 },
        { L_,        0,      0,     0,          1,       2,   106 },

        };

        enum { NUM_DATA64 = sizeof DATA64 / sizeof *DATA64 };

        if (verbose) cout <<
       "\nTesting basic accessor for version '2' header information.." << endl;
        {
            for (int ti = 0; ti < NUM_DATA64; ++ti) {
                const int LINE       = DATA64[ti].d_line;
                const int IS_GMT     = DATA64[ti].d_numIsGmt;
                const int IS_STD     = DATA64[ti].d_numIsStd;
                const int TRANS      = DATA64[ti].d_numTransitions;
                const int LCL_T_TYPE = DATA64[ti].d_numLocalTimeTypes;
                const int AB_DATA    = DATA64[ti].d_abbrevDataSize;
                const int SIZE       = DATA64[ti].d_size;

                RawHeader RH;
                RH.setVersion('2');
                RH.setNumIsGmt(IS_GMT);
                RH.setNumIsStd(IS_STD);
                RH.setNumTransitions(TRANS);
                RH.setNumLocalTimeTypes(LCL_T_TYPE);
                RH.setAbbrevDataSize(AB_DATA);

                ZoneinfoData ZI(RH);

                LOOP3_ASSERT(LINE, SIZE, ZI.size(), SIZE == ZI.size());

                const RawHeader& ZIH = *ZI.getRawHeader64();

                LOOP3_ASSERT(LINE, IS_GMT, ZIH.numIsGmt(),
                             IS_GMT == ZIH.numIsGmt());
                LOOP3_ASSERT(LINE, IS_STD, ZIH.numIsStd(),
                             IS_STD == ZIH.numIsStd());
                LOOP3_ASSERT(LINE, TRANS, ZIH.numTransitions(),
                             TRANS == ZIH.numTransitions());
                LOOP3_ASSERT(LINE, LCL_T_TYPE, ZIH.numLocalTimeTypes(),
                             LCL_T_TYPE == ZIH.numLocalTimeTypes());
                LOOP3_ASSERT(LINE, AB_DATA, ZIH.abbrevDataSize(),
                             AB_DATA == ZIH.abbrevDataSize());
            }
        }

        if (verbose) cout << "\nTesting created version '\0' buffer." << endl;
        {
            RawHeader RH;
            RH.setNumIsGmt(2);
            RH.setNumIsStd(2);
            RH.setNumTransitions(2);
            RH.setNumLocalTimeTypes(2);
            RH.setAbbrevDataSize(2);
            ZoneinfoData ZI(RH);

            enum { TEST_DATA_SIZE = sizeof TEST_DATA / sizeof *TEST_DATA };

            ASSERT(TEST_DATA_SIZE == ZI.size());
            ASSERT(0 == memcmp(ZI.buffer(), TEST_DATA, TEST_DATA_SIZE));
        }

        if (verbose) cout << "\nTesting created version '2' buffer." << endl;
        {
            RawHeader RH;
            RH.setVersion('2');
            RH.setNumIsGmt(2);
            RH.setNumIsStd(2);
            RH.setNumTransitions(2);
            RH.setNumLocalTimeTypes(2);
            RH.setAbbrevDataSize(2);
            ZoneinfoData ZI(RH);

            enum { TEST_DATA_VERSION2_SIZE =
                      sizeof TEST_DATA_VERSION2 / sizeof *TEST_DATA_VERSION2 };

            ASSERT(TEST_DATA_VERSION2_SIZE == ZI.size());
            ASSERT(0 == memcmp(ZI.buffer(), TEST_DATA_VERSION2,
                                                     TEST_DATA_VERSION2_SIZE));


        }

        if (verbose) cout << "\nTesting time zone string modification/access."
                          << endl;
        {
            static const struct {
                int         d_line;    // line
                const char *d_string;  // time zone string
                int         d_length;  // string length
            } TZ[] = {
               //LINE  STRING    LENGTH
               //----  ------    ------
               { L_,   "",       0      },
               { L_,   "a",      1      },
               { L_,   "\n",     1      },
               { L_,   "a\n",    2      },
               { L_,   "\na",    2      },
               { L_,   "abc",    3      },
            };
            enum { NUM_TZ = sizeof TZ / sizeof *TZ };

            for (int ti = 0; ti < NUM_DATA64; ++ti) {
                const int LINE       = DATA64[ti].d_line;
                const int IS_GMT     = DATA64[ti].d_numIsGmt;
                const int IS_STD     = DATA64[ti].d_numIsStd;
                const int TRANS      = DATA64[ti].d_numTransitions;
                const int LCL_T_TYPE = DATA64[ti].d_numLocalTimeTypes;
                const int AB_DATA    = DATA64[ti].d_abbrevDataSize;

                RawHeader RH;

                RH.setNumIsGmt(IS_GMT);
                RH.setNumIsStd(IS_STD);
                RH.setNumTransitions(TRANS);
                RH.setNumLocalTimeTypes(LCL_T_TYPE);
                RH.setAbbrevDataSize(AB_DATA);

                RH.setVersion('2');
                ZoneinfoData ZI2(RH);

                RH.setVersion('3');
                ZoneinfoData ZI3(RH);

                LOOP2_ASSERT(LINE, *ZI2.getTimeZoneString(),
                             '\n' == *ZI2.getTimeZoneString());

                LOOP2_ASSERT(LINE, ZI2.timeZoneStringLength(),
                             0 == ZI2.timeZoneStringLength());

                LOOP2_ASSERT(LINE, *ZI3.getTimeZoneString(),
                             '\n' == *ZI3.getTimeZoneString());

                LOOP2_ASSERT(LINE, ZI3.timeZoneStringLength(),
                             0 == ZI3.timeZoneStringLength());

                for (int tj = 0; tj < NUM_TZ; ++tj) {
                   const int   TZ_LINE = TZ[tj].d_line;
                   const char *STRING  = TZ[tj].d_string;
                   const int   LENGTH  = TZ[tj].d_length;

                   ZI2.setTimeZoneString(STRING, LENGTH);
                   ZI3.setTimeZoneString(STRING, LENGTH);

                   LOOP2_ASSERT(LINE, TZ_LINE,
                                0 == strncmp(STRING,
                                             ZI2.getTimeZoneString(),
                                             LENGTH));
                   LOOP2_ASSERT(LINE, ZI2.timeZoneStringLength(),
                                LENGTH == ZI2.timeZoneStringLength());

                   LOOP2_ASSERT(LINE, TZ_LINE,
                                0 == strncmp(STRING,
                                             ZI3.getTimeZoneString(),
                                             LENGTH));
                   LOOP2_ASSERT(LINE, ZI3.timeZoneStringLength(),
                                LENGTH == ZI3.timeZoneStringLength());
                }
            }
        }

        if (verbose) cout << "\nTesting real life data." << endl;
        {
            static const struct {
                int                  d_line;
                const unsigned char *d_buffer;
                int                  d_size;
                int                  d_numLocalTimeTypes;
                int                  d_numIsGmt;
                int                  d_numIsStd;
                int                  d_numTransitions;
                int                  d_abbrevDataSize;
            } DATA [] = {
    //---------^
    //LINE BUFFER           SIZE                   LLT  GMT  STD  TRANS  ABB
    //---- ---------------  ---------------------- ---  ---  ---  -----  ---
    { L_,  NEW_YORK_DATA,   sizeof NEW_YORK_DATA,    5,   5,   5,   236,  20 },
    { L_,  LONDON_DATA,     sizeof LONDON_DATA,      8,   8,   8,   243,  17 },
    { L_,  TOKYO_DATA,      sizeof TOKYO_DATA,       5,   5,   5,    11,  16 },
    { L_,  KIRITIMATI_DATA, sizeof KIRITIMATI_DATA,  4,   4,   4,     3,   9 },
    { L_,  CHATHAM_DATA,    sizeof CHATHAM_DATA,     4,   4,   4,   128,  16 },
    //---------v
            };

            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int            LINE       = DATA[ti].d_line;
                const unsigned char *BUFFER     = DATA[ti].d_buffer;
                const int            SIZE       = DATA[ti].d_size;
                const int            LCL_T_TYPE = DATA[ti].d_numLocalTimeTypes;
                const int            IS_GMT     = DATA[ti].d_numIsGmt;
                const int            IS_STD     = DATA[ti].d_numIsStd;
                const int            TRANS      = DATA[ti].d_numTransitions;
                const int            AB_DATA    = DATA[ti].d_abbrevDataSize;

                ZoneinfoData ZI(reinterpret_cast<const char *>(BUFFER),
                                SIZE);

                LOOP3_ASSERT(LINE, SIZE, ZI.size(), SIZE == ZI.size());

                const RawHeader& ZIH = *ZI.getRawHeader64();

                LOOP3_ASSERT(LINE, IS_GMT, ZIH.numIsGmt(),
                             IS_GMT == ZIH.numIsGmt());
                LOOP3_ASSERT(LINE, IS_STD, ZIH.numIsStd(),
                             IS_STD == ZIH.numIsStd());
                LOOP3_ASSERT(LINE, TRANS, ZIH.numTransitions(),
                             TRANS == ZIH.numTransitions());
                LOOP3_ASSERT(LINE, LCL_T_TYPE, ZIH.numLocalTimeTypes(),
                             LCL_T_TYPE == ZIH.numLocalTimeTypes());
                LOOP3_ASSERT(LINE, AB_DATA, ZIH.abbrevDataSize(),
                             AB_DATA == ZIH.abbrevDataSize());

            }
        }

        if (verbose) cout << "\nTesting 'verifyTimeZone'." << endl;
        {
            testVerifyTimeZone(verbose);
        }

        if (verbose) cout << "\nTesting 'verifyTimeZoneVersion2Format'."
                                                                       << endl;
        {
            testVerifyTimeZoneVersion2Format(verbose);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but doesn't fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Call 'read' on real-life data, 'Pacific/Chatham', and verify that
        //:   read successfully load data into a 'baltzo::Zoneinfo'.
        //
        // Testing:
        //   BREATHING TEST
        //
        // --------------------------------------------------------------------

        baltzo::Zoneinfo TZ;

        bdlsb::FixedMemInStreamBuf inStreamBuf(
                                  reinterpret_cast<const char *>(CHATHAM_DATA),
                                  sizeof(CHATHAM_DATA));
        bsl::istream inputStream(&inStreamBuf);

        BinHeader HD;
        ASSERT(0 == Obj::read(&TZ, &HD, inputStream));
        if (verbose) {
            cout << HD << endl;
            cout << TZ << endl;
        }

        ASSERT('2' == HD.version());
        ASSERT(128 == HD.numTransitions());
        ASSERT(4   == HD.numLocalTimeTypes());
        ASSERT(4   == HD.numIsGmt());
        ASSERT(4   == HD.numIsStd());
        ASSERT(0   == HD.numLeaps());
        ASSERT(16  == HD.abbrevDataSize());

        ZoneinfoData ZI(reinterpret_cast<const char *>(CHATHAM_DATA),
                        sizeof(CHATHAM_DATA));
        verifyTimeZoneVersion2Format(ZI, TZ, L_);


        baltzo::Zoneinfo TZ1;

        bdlsb::FixedMemInStreamBuf inStreamBuf1(
                                 reinterpret_cast<const char *>(NEW_YORK_DATA),
                                 sizeof(NEW_YORK_DATA));
        bsl::istream inputStream1(&inStreamBuf1);

        BinHeader HD1;
        ASSERT(0 == Obj::read(&TZ1, &HD1, inputStream1));
        if (verbose) {
          //  cout << HD << endl;
            cout << TZ1 << endl;
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // DUMP TIMEZONE FILE:
        //   This loads a TZ Database binary file whose name is provided on
        //   the console, and dumps its contents to stdout.
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------
        baltzo::Zoneinfo timeZone;

        if (argc != 3) {
            cout << "Usage " << argv[0] << " -1 [data file path]" << endl;
            exit(-1);
        }

        bsl::ifstream istream(argv[2]);

        if (!istream.good()) {
            cout << "Bad file name: " << argv[2] << endl;
            exit(-1);
        }

        if (0 != Obj::read(&timeZone, istream)) {
            cout << "Failed to read file" << endl;
            exit(-1);
        }

        timeZone.print(cout, 1, 3);
      } break;
      case -2: {
        // --------------------------------------------------------------------
        // TEST TIMEZONE FILE:
        //   This loads a TZ Database binary file whose name is provided on
        //   the console, and validate its contents.  Its purpose is to test
        //   that a Zoneinfo binary data file is in the correct format rather
        //   than testing this class.
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------
        baltzo::Zoneinfo timeZone;

        if (argc != 3) {
            cout << "Usage " << argv[0] << " -1 [data file path]" << endl;
            exit(-1);
        }

        cout << argv[2] << endl;
        bsl::ifstream istream(argv[2]);

        if (!istream.good()) {
            cout << "Bad file name: " << argv[2] << endl;
            exit(-1);
        }

        int size;
        istream.seekg(0, bsl::ios::end);
        size = istream.tellg();
        istream.seekg(0, bsl::ios::beg);
        char *buffer = new char[size];
        istream.read(buffer, size);
        istream.close();

        bdlsb::FixedMemInStreamBuf inStreamBuf(buffer, size);
        bsl::istream inputStream(&inStreamBuf);

        if (0 != Obj::read(&timeZone, inputStream)) {
            cout << "Failed to read file" << endl;
            exit(-1);
        }

        ZoneinfoData data(buffer, size);
        if ('2' == data.getRawHeader()->version() &&
            0 != verifyTimeZoneVersion2Format(data, timeZone, L_)) {
            cout << "Failed to parse file" << endl;
            exit(-1);
        } else if (0 != verifyTimeZone(data, timeZone, L_)) {
            cout << "Failed to parse file" << endl;
            exit(-1);
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
