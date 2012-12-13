// baejsn_parser.t.cpp                                                -*-C++-*-
#include <baejsn_parser.h>

#include <bsl_sstream.h>
#include <bsl_cfloat.h>
#include <bsl_climits.h>
#include <bsl_limits.h>
#include <bsl_iostream.h>
#include <bdepu_typesparser.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bdesb_memoutstreambuf.h>            // for testing only
#include <bdesb_fixedmemoutstreambuf.h>       // for testing only
#include <bdesb_fixedmeminstreambuf.h>        // for testing only

using namespace BloombergLP;
using namespace bsl;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

#define LOOP0_ASSERT ASSERT

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP1_ASSERT LOOP_ASSERT

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// The 'BSLS_BSLTESTUTIL_EXPAND' macro is required to workaround a
// pre-proccessor issue on windows that prevents __VA_ARGS__ to be expanded in
// the definition of 'BSLS_BSLTESTUTIL_NUM_ARGS'
#define EXPAND(X)                                            \
    X

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)        \
    N

#define NUM_ARGS(...)                                        \
    EXPAND(NUM_ARGS_IMPL( __VA_ARGS__, 5, 4, 3, 2, 1, 0, ""))

#define LOOPN_ASSERT_IMPL(N, ...)                            \
    EXPAND(LOOP ## N ## _ASSERT(__VA_ARGS__))

#define LOOPN_ASSERT(N, ...)                                 \
    LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...)                                         \
    LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)


#define WS "   \t       \n      \v       \f       \r       "

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

enum { SUCCESS = 0, FAILURE = -1 };

typedef baejsn_Parser Obj;

bsl::ostream& operator<<(bsl::ostream& stream, Obj::TokenType value)
{
#define CASE(X) case(Obj::X): stream << #X; break;

    switch (value) {
      CASE(BAEJSN_ERROR)
      CASE(BAEJSN_ELEMENT_NAME)
      CASE(BAEJSN_START_OBJECT)
      CASE(BAEJSN_END_OBJECT)
      CASE(BAEJSN_START_ARRAY)
      CASE(BAEJSN_END_ARRAY)
      CASE(BAEJSN_ELEMENT_VALUE)
      default: stream << "(* UNKNOWN *)"; break;
    }

#undef CASE
}

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

typedef bsls_PlatformUtil::Int64  Int64;
typedef bsls_PlatformUtil::Uint64 Uint64;

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    bool verbose         = argc > 2;
    bool veryVerbose     = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator          globalAllocator("global", veryVeryVerbose);
    bslma_Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO BAEJSN_END_ARRAY
        //
        // Concerns:
        //: 1 START_ARRAY    -> END_ARRAY                            '[' -> ']'
        //: 2 VALUE (number) -> END_ARRAY                          VALUE -> ']'
        //: 3 VALUE (string) -> END_ARRAY                            '"' -> ']'
        //: 4 VALUE (object) -> END_ARRAY                            '}' -> ']'
        //
        // Plan:
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                 << "TESTING 'advanceToNextToken' TO BAEJSN_END_ARRAY" << endl
                 << "================================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // '[' - ']'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                              "]",
                3,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS               "]" WS,
                3,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                     "]",
                3,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock " WS " price\""
                WS                          ":"
                WS                            "["
                WS                            "]" WS,
                3,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },

            // value (integer) -> ']'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012"
                                         "]",
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1, 2"
                                     "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012"
                WS                          "]",
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                       "1.500012"
                WS                                "]",
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },

            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345"
                                          "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345,"
                                "0"
                                          "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                       "1.500012" WS ","
                WS                       "-2.12345"
                WS                                       "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012,"
                WS                 "-2.12345,"
                WS                 "0"
                WS                           "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012" WS ","
                WS                 "-2.12345" WS ","
                WS                 "0"
                WS                                 "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },

            // value (string) -> ']'
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\""
                                         "]",
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""
                WS                          "]",
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS             "\"" WS "John" WS "\""
                WS                                   "]" WS,
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                     "\"" WS "John" WS "Doe" WS "\""
                WS                                                   "]",
                4,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },

            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\""
                                          "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\","
                                "\"Ryan\""
                                           "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John,\""
                WS                 "\"Smith\""
                WS                           "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John,\""
                WS                 "\"Smith,\""
                WS                 "\"Ryan\""
                WS                           "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""  WS ","
                WS                 "\"Smith\"" WS 
                WS                                  "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""  WS ","
                WS                 "\"Smith\"" WS ","
                WS                 "\"Ryan\""  WS 
                WS                                  "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS              "\"" WS "John"  WS "\"" WS ","
                WS              "\"" WS "Smith" WS "\"" WS
                WS                                           "]" WS,
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS              "\"" WS "John"  WS "\"" WS ","
                WS              "\"" WS "Smith" WS "\"" WS ","
                WS              "\"" WS "Ryan" WS "\""  WS
                WS                                           "]" WS,
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                    "\"" WS "John"  WS "Doe"  WS "\"" WS ","
                WS                    "\"" WS "Black" WS "Jack" WS "\"" WS 
                WS                                                         "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                    "\"" WS "John"  WS "Doe"  WS "\"" WS ","
                WS                    "\"" WS "Black" WS "Jack" WS "\"" WS "," 
                WS                    "\"" WS "New"   WS "Deal" WS "\"" WS
                WS                                                         "]",
                6,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },

            // value (object) -> ']', i.e. '}' -> ']'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "}"
                              "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "}"
                WS               "]",
                5,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"bid_price\":1.500012"
                                "}"
                              "]",
                7,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]",
                7,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"name\":\"CompanyName\","
                                  "\"bid_price\":1.500012"
                                 "}"
                               "]",
                9,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\""
                WS                 "},"
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]",
                11,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\""
                WS                 "},"
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]" WS,
                11,
                true,
                Obj::BAEJSN_END_ARRAY,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO BAEJSN_START_ARRAY
        //
        // Concerns:
        //: 1 NAME        -> START_ARRAY                             ':' -> '['
        //: 2 START_ARRAY -> START_ARRAY                             '[' -> '['
        //
        // Plan:
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                      << "TESTING 'advanceToNextToken' TO BAEJSN_NAME" << endl
                      << "===========================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // name -> value (array), i.e. name -> '['
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "[",
                2,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "[" WS,
                2,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "[" WS,
                2,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock " WS " price\""
                WS                          ":"
                WS                            "[" WS,
                2,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },

            // '[' -> '[' (array of arrays)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "[",
                3,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "[",
                3,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "[" WS,
                3,
                true,
                Obj::BAEJSN_START_ARRAY,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO BAEJSN_END_OBJECT
        //
        // Concerns:
        //: 1 START_OBJECT   -> END_OBJECT                           '{' -> '}'
        //: 2 VALUE (number) -> END_OBJECT                         VALUE -> '}'
        //: 3 VALUE (string) -> END_OBJECT                           '"' -> '}'
        //: 4 START_OBJECT   -> END_OBJECT                    '[' -> '{' -> '}'
        //: 5 START_OBJECT   -> END_OBJECT                    '{' -> '{' -> '}'
        //: 6 END_OBJECT     -> END_OBJECT             '{' -> '{' -> '}' -> '}'
        //: 7 END_OBJECT     -> END_OBJECT      '{' -> '{' -> '{' -> '}' -> '}'
        //: 8 END_OBJECT     -> END_OBJECT      '[' -> '{' -> '}' -> '{' -> '}'
        //
        // Plan:
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                << "TESTING 'advanceToNextToken' TO BAEJSN_END_OBJECT" << endl
                << "=================================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // '{' -> '}'
            {
                L_,
                "{"
                "}",
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{\n"
                "}",
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                   "}",
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{" WS
                "}",
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                "}" WS,
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{\n"
                WS "}",
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS "}"
                WS,
                1,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // value (integer) -> '}'
            {
                L_,
                "{"
                 "\"price\""
                          ":"
                           "1.500012"
                                    "}",
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"price\""
                WS           ":"
                WS            "1.500012"
                WS                     "}" WS,
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"stock " WS " price\""
                WS                        ":"
                WS                          "1.500012"
                WS                                   "}" WS,
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // value (string) -> '}'
            {
                L_,
                "{"
                 "\"name\""
                         ":"
                          "\"John\""
                                   "}",
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"John\""
                WS                    "}" WS,
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"" WS "John" WS "\""
                WS                                "}" WS,
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"full " WS " name\""
                WS                      ":"
                WS                       "\"" WS "John" WS " Doe" WS "\""
                WS                                                      "}" WS,
                3,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // '[' -> '{' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "}",
                4,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "}",
                4,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "}" WS,
                4,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },


            // '{' -> '{' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"full name\""
                                              ":"
                                                "{"
                                                "}",
                5,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}",
                5,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}" WS,
                5,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // '{' -> '{' -> '}' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"full name\""
                                              ":"
                                                "{"
                                                "}"
                              "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}"
                WS               "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}" WS
                WS               "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // '{' -> '{' -> '{' -> '}' -> '}' -> '}
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"full name\""
                                              ":"
                                                "{"
                                                "}"
                              "}"
                "}",
                7,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}"
                WS               "}"
                WS "}",
                7,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"full name\""
                WS                               ":"
                WS                                 "{"
                WS                                 "}" WS
                WS               "}"
                WS "}",
                7,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            // '[' -> '{' -> '}' -> '{' -> '}'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "},"
                                "{"
                                "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "},"
                WS                 "{"
                WS                 "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "},"
                WS                 "{"
                WS                 "}" WS,
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },

            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "}"
                              "]"
                "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "}"
                WS               "]"
                WS "}",
                6,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"bid_price\":1.500012"
                                "}"
                              "]"
                "}",
                8,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]"
                WS "}",
                8,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"name\":\"CompanyName\","
                                  "\"bid_price\":1.500012"
                                 "}"
                               "]"
                "}",
                10,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\""
                WS                 "},"
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS               "]"
                WS "}",
                12,
                true,
                Obj::BAEJSN_END_OBJECT,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO BAEJSN_VALUE
        //
        // Concerns:
        //: 1 NAME           -> VALUE (number)                     ':' -> VALUE
        //: 2 NAME           -> VALUE (string)                     ':' -> VALUE
        //: 3 START_ARRAY    -> VALUE (number)                     '[' -> VALUE
        //: 4 START_ARRAY    -> VALUE (string)                     '[' -> VALUE
        //: 5 VALUE (number) -> VALUE (number)                   VALUE -> VALUE
        //: 6 VALUE (string) -> VALUE (string)                   VALUE -> VALUE
        //: 7 VALUE (number) -> VALUE (string)                   VALUE -> VALUE
        //: 8 VALUE (string) -> VALUE (number)                   VALUE -> VALUE
        //
        // Plan:
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                     << "TESTING 'advanceToNextToken' TO BAEJSN_VALUE" << endl
                     << "============================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // name -> value (integer)
            {
                L_,
                "{"
                 "\"price\""
                          ":"
                           "1.500012"
                                    "}",
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "1.500012"
            },
            {
                L_,
                WS "{"
                WS  "\"price\""
                WS           ":"
                WS            "1.500012"
                WS                     "}" WS,
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "1.500012"
            },
            {
                L_,
                WS "{"
                WS  "\"stock " WS " price\""
                WS                        ":"
                WS                          "1.500012"
                WS                                   "}" WS,
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "1.500012"
            },

            // name -> value (string)
            {
                L_,
                "{"
                 "\"name\""
                         ":"
                          "\"John\""
                                   "}",
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"John\""
                WS                    "}" WS,
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS           "\"" WS "John" WS "\""
                WS                                "}" WS,
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "John" WS "\""
            },
            {
                L_,
                WS "{"
                WS  "\"full " WS " name\""
                WS                      ":"
                WS                       "\"" WS "John" WS " Doe" WS "\""
                WS                                                      "}" WS,
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "John" WS " Doe" WS "\""
            },
            {
                L_,
                WS "{"
                WS  "\"full " WS " name\""
                WS                      ":"
                WS                        "\"John " WS " Doe\""
                WS                                            "}" WS,
                2,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"John " WS " Doe\""
            },

            // '[' -> value (integer)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012"
                                         "]",
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "1.500012"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012"
                WS                          "]",
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "1.500012"
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                       "1.500012"
                WS                                "]",
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "1.500012"
            },

            // '[' -> value (string)
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\""
                                         "]",
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""
                WS                          "]",
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"John\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS             "\"" WS "John" WS "\""
                WS                                   "]" WS,
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "John" WS "\""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                     "\"" WS "John" WS "Doe" WS "\""
                WS                                                   "]",
                3,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "John" WS "Doe" WS "\""
            },

            // value (integer) -> value (integer)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345"
                                          "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "-2.12345"
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "1.500012,"
                                "-2.12345,"
                                "0"
                                          "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "0"
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "["
                WS                       "1.500012" WS ","
                WS                       "-2.12345"
                WS                                       "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "-2.12345"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012,"
                WS                 "-2.12345,"
                WS                 "0"
                WS                           "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "0"
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "1.500012" WS ","
                WS                 "-2.12345" WS ","
                WS                 "0"
                WS                                 "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "0"
            },

            // value (string) -> value (string)
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\""
                                          "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Smith\","
                                "\"Ryan\""
                                           "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John,\""
                WS                 "\"Smith\""
                WS                           "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John,\""
                WS                 "\"Smith,\""
                WS                 "\"Ryan\""
                WS                           "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "\"John\""  WS ","
                WS                "\"Smith\"" WS 
                WS                                 "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""  WS ","
                WS                 "\"Smith\"" WS ","
                WS                 "\"Ryan\""  WS 
                WS                                  "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS              "\"" WS "John"  WS "\"" WS ","
                WS              "\"" WS "Smith" WS "\"" WS
                WS                                           "]" WS,
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "Smith" WS "\""
            },
            {
                L_,
                WS "{"
                WS  "\"name\""
                WS          ":"
                WS            "["
                WS              "\"" WS "John"  WS "\"" WS ","
                WS              "\"" WS "Smith" WS "\"" WS ","
                WS              "\"" WS "Ryan" WS "\"" WS
                WS                                           "]" WS,
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "Ryan" WS "\""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                    "\"" WS "John"  WS "Doe"  WS "\"" WS ","
                WS                    "\"" WS "Black" WS "Jack" WS "\"" WS 
                WS                                                         "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "Black" WS "Jack" WS "\""
            },
            {
                L_,
                WS "{"
                WS   "\"full name\""
                WS                 ":"
                WS                   "["
                WS                    "\"" WS "John"  WS "Doe"  WS "\"" WS ","
                WS                    "\"" WS "Black" WS "Jack" WS "\"" WS "," 
                WS                    "\"" WS "New"   WS "Deal" WS "\"" WS 
                WS                                                         "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"" WS "New" WS "Deal" WS "\""
            },

            // value (integer) -> value (string)
            // value (string)  -> value (integer)
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "12345,"
                                "\"Smith\""
                                          "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "12345,"
                                "\"Ryan\""
                                           "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"Smith\","
                                "12345"
                                          "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "12345"
            },
            {
                L_,
                "{"
                  "\"name\""
                           ":"
                              "["
                                "\"John\","
                                "\"Ryan\","
                                "12345"
                                          "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "12345"
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "12345,"
                WS                 "\"Smith\""
                WS                           "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Smith\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John,\""
                WS                 "12345,"
                WS                 "\"Ryan\""
                WS                           "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "\"Ryan\""
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS              "["
                WS                "\"John\""  WS ","
                WS                "12345"     WS 
                WS                                 "]",
                4,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "12345"
            },
            {
                L_,
                WS "{"
                WS   "\"name\""
                WS            ":"
                WS               "["
                WS                 "\"John\""  WS ","
                WS                 "\"Smith\"" WS ","
                WS                 "12345"     WS 
                WS                                  "]",
                5,
                true,
                Obj::BAEJSN_ELEMENT_VALUE,
                true,
                "12345"
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO BAEJSN_NAME
        //
        // Concerns:
        //: 1 START_OBJECT -> NAME                                  '{' -> NAME
        //
        // Plan:
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                      << "TESTING 'advanceToNextToken' TO BAEJSN_NAME" << endl
                      << "===========================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // '{' -> name
            {
                L_,
                "{"
                 "\"name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{\n"
                 "\"name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                "{\n"
                 "\"name\"\n",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                    "\"name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                WS " \"name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{"
                WS " \"name\"" WS,
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "name"
            },
            {
                L_,
                WS "{\"element name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "element name"
            },
            {
                L_,
                WS "{\"element " WS " name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "element " WS " name"
            },
            {
                L_,
                WS "{"
                WS " \"element " WS " name\"",
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "element " WS " name"
            },
            {
                L_,
                WS "{"
                WS " \"element " WS " name\"" WS,
                1,
                true,
                Obj::BAEJSN_ELEMENT_NAME,
                true,
                "element " WS " name"
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' TO BAEJSN_START_OBJECT
        //
        // Concerns:
        //: 1 BEGIN          -> START_OBJECT                       ERROR -> '{'
        //: 2 NAME           -> START_OBJECT                         ':' -> '{'
        //: 3 START_ARRAY    -> START_OBJECT                         '[' -> '{'
        //: 4 VALUE (number) -> START_OBJECT         ':' -> VALUE -> ',' -> '{'
        //: 5 END_OBJECT     -> START_OBJECT    '[' -> '{' -> '}' -> ',' -> '{'
        //
        // Plan:
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
              << "TESTING 'advanceToNextToken' TO BAEJSN_START_OBJECT" << endl
              << "===================================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {

            // BEGIN -> '{'
            {
                L_,
                "{",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{" WS,
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{" WS,
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                 "\"name\"",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{\n"
                 "\"name\"",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{\n"
                 "\"name\"\n",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                    "\"name\"",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                WS "\"name\"",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                 "\"name\"" WS,
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS  "\"name\"",
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS " \"name\"" WS,
                0,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },

            // name -> value (object), i.e. name -> '{'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{",
                2,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{" WS,
                2,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock price\""
                WS                   ":"
                WS                     "{" WS,
                2,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"stock" WS "price\""
                WS                        ":"
                WS                          "{" WS,
                2,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },

            // '[' -> '{' (array of objects)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{",
                3,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{",
                3,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{" WS,
                3,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },

            // '}' -> '{'
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                 "\"bid_price\":1.500012"
                                  "},"
                              "{",
                6,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"bid_price\":1.500012"
                WS               "},"
                WS               "{",
                6,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"bid_price\""
                WS                               ":"
                WS                                 "1.500012"
                WS               "},"
                WS               "{",
                6,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"bid_price\""
                WS                               ":"
                WS                                 "1.500012"
                WS               "}"
                WS                 ","
                WS               "{" WS,
                6,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "{"
                                "\"name\":\"CompanyName\","
                                "\"bid_price\":1.500012"
                              "},"
                              "{",
                8,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"name\""
                WS                          ":"
                WS                            "\"CompanyName\""
                WS                 "\"bid_price\""
                WS                               ":"
                WS                                 "1.500012"
                WS               "},"
                WS               "{",
                8,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "{"
                WS                 "\"name\""
                WS                          ":"
                WS                            "\"CompanyName\""
                WS                 "\"bid_price\""
                WS                               ":"
                WS                                 "1.500012"
                WS               "}"
                WS                 ","
                WS               "{" WS,
                8,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },

            // '}' -> '{' (array of objects)
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                "},"
                                "{",
                5,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                 "},"
                WS                 "{",
                5,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"bid_price\":1.500012"
                                "},"
                                "{",
                7,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\":1.500012"
                WS                 "},"
                WS                 "{",
                7,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "},"
                WS                 "{",
                7,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS                   ","
                WS                 "{" WS,
                7,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                "{"
                  "\"price\""
                            ":"
                              "["
                                "{"
                                  "\"name\":\"CompanyName\","
                                  "\"bid_price\":1.500012"
                                "},"
                                "{",
                9,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\""
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "},"
                WS                 "{",
                9,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },
            {
                L_,
                WS "{"
                WS   "\"price\""
                WS             ":"
                WS               "["
                WS                 "{"
                WS                   "\"name\""
                WS                            ":"
                WS                              "\"CompanyName\""
                WS                   "\"bid_price\""
                WS                                 ":"
                WS                                   "1.500012"
                WS                 "}"
                WS                   ","
                WS                 "{" WS,
                9,
                true,
                Obj::BAEJSN_START_OBJECT,
                false,
                ""
            },

            // Error

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'advanceToNextToken' FIRST CHARACTER
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   int advanceToNextToken();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                    << "TESTING 'advanceToNextToken'  FIRST CHARACTER" << endl
                    << "=============================================" << endl;

        const struct {
            int             d_line;
            const char     *d_text_p;
            int             d_preMoves;
            bool            d_validFlag;
            Obj::TokenType  d_expTokenType;
            bool            d_hasValue;
            const char     *d_value_p;
        } DATA[] = {
            // Whitespace only
            {
                L_,
                "",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "          ",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "   \t       ",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "\n          ",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "      \n    ",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "\n\t          ",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "    \t  \n    ",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                " \t\n\v\f\r",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS,
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },

            // Error - First character
            {
                L_,
                "[",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "[",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },

            {
                L_,
                "]",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "]",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },

            {
                L_,
                "}",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "}",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },

            {
                L_,
                "\"",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "\"",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                ",",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS ",",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                ":",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS ":",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "1",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "1",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "*",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "*",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                "A",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
            {
                L_,
                WS "A",
                0,
                false,
                Obj::BAEJSN_ERROR,
                false,
                ""
            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ ti) {
            const int            LINE      = DATA[ti].d_line;
            const string         TEXT      = DATA[ti].d_text_p;
            const int            PRE_MOVES = DATA[ti].d_preMoves;
            const bool           IS_VALID  = DATA[ti].d_validFlag;
            const Obj::TokenType EXP_TOKEN = DATA[ti].d_expTokenType;
            const bool           HAS_VALUE = DATA[ti].d_hasValue;
            const string         EXP_VALUE = DATA[ti].d_value_p;

            bsl::istringstream iss(TEXT);

            if (veryVerbose) {
                P(LINE) P(TEXT) P(PRE_MOVES) P(IS_VALID)
                P(EXP_TOKEN) P(HAS_VALUE) P(EXP_VALUE)
            }

            Obj mX;  const Obj& X = mX;
            ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR == X.tokenType());

            mX.reset(iss.rdbuf());

            for (int i = 0; i < PRE_MOVES; ++i) {
                ASSERTV(i, 0 == mX.advanceToNextToken());
                ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR != X.tokenType());
            }

            if (IS_VALID) {
                ASSERTV(LINE, 0 == mX.advanceToNextToken());
                ASSERTV(LINE, X.tokenType(), EXP_TOKEN,
                        EXP_TOKEN == X.tokenType());

                if (HAS_VALUE) {
                    bslstl::StringRef value;
                    ASSERTV(LINE, 0 == X.value(&value));
                    ASSERTV(LINE, value, EXP_VALUE, value == EXP_VALUE);
                }
            }
            else {
                ASSERTV(LINE, 0 != mX.advanceToNextToken());
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        Obj mX;  const Obj& X = mX;
        ASSERTV(X.tokenType(), Obj::BAEJSN_ERROR == X.tokenType());
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global/default allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}
