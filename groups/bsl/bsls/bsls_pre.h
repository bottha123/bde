// bsls_pre.h

//@PURPOSE: Provide a macro for use in fuzz testing narrow contract functions.
//
//@CLASSES:
//
//@MACROS:
//  BSLS_PRE_DONE
//
//@SEE_ALSO: bsls_precheck, bsls_assert, bsls_review, bsls_asserttest
//
//
//@DESCRIPTION: This component provides a macro, BSLS_PRE_DONE to facilitate
// fuzz testing narrow contract functions.  When fuzz testing is enabled,
// BSLS_PRE_DONE_ENABLED is defined. When it is not defined BSLS_PRE_DONE
// should expand to nothing. When fuzz testing is enabled, BSLS_PRE_DONE calls
// a dynamic handler function, similar to how BSLS_ASSERT calls
// Assert::invokeHandler. This passes the file and line number where the
// BSLS_PRE_DONE was invoked.
//
// BSLS_PRE_DONE is used as a marker to identify when precondition checks are
// complete.
//
//
///Usage
///-----
// The following example shows the addition of the BSLS_PREDONE macro to a
// narrow contract function. Note that the macro is inserted after the function
// preconditions have been checked.
//
// ...
//  inline
//  void Date::setYearMonthDay(int year, int month, int day)
//  {
//    BSLS_ASSERT_SAFE(isValidYearMonthDay(year, month, day));
//    BSLS_PREDONE();
//
//    d_serialDate = SerialDateImpUtil::ymdToSerial(year, month, day);
//  }

#define FUZZING_ENABLED

#ifdef FUZZING_ENABLED

#include <bsls_precheck.h>

#define BSLS_PRE_DONE() PreCheck::setFlagToFalse();

#else // fuzzing disabled

#define BSLS_PRE_DONE()

#endif