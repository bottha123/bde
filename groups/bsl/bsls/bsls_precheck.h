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
// When enabled (i.e. BSLS_PRE_DONE_ENABLED is defined), BSLS_PRE_DONE
// calls a dynamic handler function similar to how BSLS_ASSERT calls
// Assert::invokeHandler. This passes the file and line number where the
// BSLS_PRE_DONE was invoked.
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
//    BSLS_PRECHECK( Date d(year,month,day); );
/*       sethandlers();
      try {
        PreCheck::initStaticState(__FILE__,__LINE__);
        Date d(year,month,day);
      } catch (bsls::AssertTestException& ex) {
	    PreCheck::checkException(ex);
      }      */
// }
//..
// Functions with narrow contracts that are to be tested should be decorated
// with the companion macro, BSLS_PRE_DONE, after the function preconditions
// have been checked.
//

#include <bsls_assert.h>
#include <bsls_review.h>
#include <exception>
#include <cstdlib>

class FuzzTestPreconditionFailedException : public std::exception {


};


#define BSLS_PRECHECK(X)                                                     \
    FuzzTestHandlerGuard hG;                                                 \
    try {                                                                    \
        PreCheck::initStaticState(__FILE__,__LINE__);                        \
        X;                                                                   \
    } catch (FuzzTestPreconditionFailedException& ex) {                      \
        PreCheck::checkException(ex);                                        \
    }                                                                        \


struct PreCheck {

  private:
    static const char *d_file;
    static int         d_line;
    static bool        d_checking;

  public:

    static void initStaticState(const char * fileName, int lineNumber)
        // Store the file and line from the testing component
    {
        d_file     = fileName;
        d_line     = lineNumber;
        d_checking = true;
    }

    //(*ViolationHandler)(const AssertViolation&);
   static void assertViolationHandler(const BloombergLP::bsls::AssertViolation& av)
        // If "checking preconditions" is true, throw an AssertTestException ...If
        // "checking preconditions" is false, std::abort()
    {
        if (d_checking)
            throw FuzzTestPreconditionFailedException(); // prior to PRE_DONE, an assertion was triggered
        else
        {
            BloombergLP::bsls::Assert::failByAbort(av);
            //std::abort(); //
        }
    }

    static void preDoneHandler(...)
        // Set the "checking preconditions" flag to false.  Eventually:
        // Validate that this is called where expected.  This requires a
        // best-effort attempt to identify the source, and we will eventually
        // want to refactor this logic out of bsls_asserttest.  This will be to
        // catch the "my precondition check violated the precondition of a
        // different function" case.
    {
        d_checking = false;
    }

    static void checkException(const FuzzTestPreconditionFailedException& )
        // Eventually: Verify that exception came from the expected component.
        // This requires the same logic as preDoneHandler which we will want to
        // refactor out of bsls_asserttest.
    {

    }

    static void setFlagToFalse()
    {
        d_checking = false;
    }
};


class FuzzTestHandlerGuard {
    // DATA
    BloombergLP::bsls::AssertFailureHandlerGuard d_assertGuard;
    //ReviewFailureHandlerGuard d_reviewGuard;
    // function pointer       d_preDoneHandlerGuard;

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
//, d_reviewGuard(&AssertTest::failTestDriverByReview)
{
    //d_preDoneHandlerGuard = PreCheck::preDoneHandler();
}

#endif