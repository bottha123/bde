// bsls_precheck.h
#ifndef INCLUDED_BSLS_PRECHECK
#define INCLUDED_BSLS_PRECHECK

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide build-specific, macros for use in fuzz testing.
//
//@CLASSES:
//  bsls::PreCheck: namespace for "assert" management functions
//
//@MACROS:
//  BSLS_PRECHECK
//
//@SEE_ALSO: bsls_assert, bsls_review, bsls_asserttest
//
//
//
//
//@DESCRIPTION: This component provides a macro, BSLS_PRECHECK, that
// can be used in fuzz testing narrow contract functions to
// determine the location of an assertion failure.
// It is intended to be used in conjunction with BSLS_PRE_DONE, a macro
// that will expand to nothing in most builds. When fuzzing is enabled,
// BSLS_PRE_DONE_ENABLED should be defined, else undefined.
//
//
///Usage
///-----
//
// ...
//  extern "C"
//  int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
// {
//    const char *FUZZ   = reinterpret_cast<const char *>(data);
//    if (size < (sizeof(char) + (3*sizeof(int)))) {
//        return 0;
//    }
//
//    const int *year = reinterpret_cast<const int*>(FUZZ);
//    const int *month = reinterpret_cast<const int*>(FUZZ + sizeof(int));
//    const int *day = reinterpret_cast<const
//    int*>(FUZZ + sizeof(int) + sizeof(int));
//
//    bdlt::Date d1;
//    BSLS_PRECHECK( d1.setYearMonthDay(*year, *month, *day); );
//..
// Functions with narrow contracts that are to be tested should be decorated
// with the macro, BSLS_PRE_DONE, after the function preconditions
// have been checked.
//

#include <bsls_assert.h>
#include <bsls_review.h>
#include <bsls_atomic.h>

#include <exception>
#include <cstdlib>
#include <bsls_pre.h>

//#include <bsl_iostream.h>
#include <iostream>

namespace BloombergLP {
namespace bsls {

class FuzzTestPreconditionFailedException : public std::exception {
    private:
    //std::string d_filename;
    const char* d_filename;

    public:
    FuzzTestPreconditionFailedException( const char* filename)
    : d_filename(filename) {}
    
    const char* filename()
    {
        return d_filename;
    }
};


#define BSLS_PRECHECK(X)                                                     \
    BloombergLP::bsls::FuzzTestHandlerGuard hG;                              \
    try {                                                                    \
        BloombergLP::bsls::PreCheck::initStaticState(__FILE__,__LINE__);     \
        X;                                                                   \
    } catch (BloombergLP::bsls::FuzzTestPreconditionFailedException& ex) {   \
        BloombergLP::bsls::PreCheck::checkException(ex);                     \
    }                                                                        \



#define IS_POWER_OF_TWO(X) (0 == ((X) & ((X) - 1)))

static int g_numberOfExceptions = 0;

struct PreCheck {

  private:
    static const char *d_file;
    static int         d_line;
    static AtomicBool  d_checking;

  public:

    static void initStaticState(const char * fileName, int lineNumber)
        // Store the file and line from the testing component
    {
        d_file     = fileName;
        d_line     = lineNumber;
        d_checking.store(true);
    }

    static void assertViolationHandler(const AssertViolation &av)
    // If "checking preconditions" is true, throw an AssertTestException ...If
    // "checking preconditions" is false, std::abort()
    {
        if (d_checking.load())
        {
            ++g_numberOfExceptions;
            //if (0 == g_numberOfExceptions % 10000)
            if (IS_POWER_OF_TWO(g_numberOfExceptions))
            {
                std::cerr << "   # exceptions caught: " 
                          << g_numberOfExceptions << std::endl;
            }
            // prior to PRE_DONE, an assertion was triggered
            throw FuzzTestPreconditionFailedException(av.fileName());
        }
        else
        {
            BloombergLP::bsls::Assert::failByAbort(av);
        }
    }

    static void preDoneHandler( const BloombergLP::bsls::AssertViolation& )
        // Set the "checking preconditions" flag to false.  Eventually:
        // Validate that this is called where expected.  This requires a
        // best-effort attempt to identify the source, and we will eventually
        // want to refactor this logic out of bsls_asserttest.  This will be to
        // catch the "my precondition check violated the precondition of a
        // different function" case.
        // 
        /*
            void narrowfunc1(int i){
                BSLS_ASSERT( X().capacity() > i);
                BSLS_ASSERT(i > 0);
                BSLS_PREDONE();
                g();
            }
        */
       // HJB: see bsls_asserttest::catchProbe()
    {
        d_checking.store(false);
    }

    static void checkException(const FuzzTestPreconditionFailedException& ex)
        // Eventually: Verify that exception came from the expected component.
        // This requires the same logic as preDoneHandler which we will want to
        // refactor out of bsls_asserttest.
    {
        //std::string filename = ex.filename();
        // 

    }
};


class FuzzTestHandlerGuard {
    // DATA
    BloombergLP::bsls::AssertFailureHandlerGuard d_assertGuard;
    //ReviewFailureHandlerGuard d_reviewGuard;
    //BloombergLP::bsls::AssertFailureHandlerGuard d_preDoneHandlerGuard;

  public:
    // CREATORS
    FuzzTestHandlerGuard();

    //! ~FuzzTestHandlerGuard() = default;
        // Destroy this object and uninstall 'AssertTest::failTestDriver' as
        // the current assertion handler.
};

inline
FuzzTestHandlerGuard::FuzzTestHandlerGuard()
: d_assertGuard(&PreCheck::assertViolationHandler)
//d_reviewGuard(&AssertTest::failTestDriverByReview)
{
    PreDoneHandler::installHandler(&PreCheck::preDoneHandler);
}

}  // close package namespace
}  // close enterprise namespace


#endif