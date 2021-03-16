// bsls_pre.h
#ifndef INCLUDED_BSLS_PRECH
#define INCLUDED_BSLS_PRECH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")


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
// fuzz testing narrow contract functions.  When fuzz testing is not enabled,
// BSLS_PRE_DONE should expand to nothing. When fuzz testing is enabled,
// BSLS_PRE_DONE calls a dynamic handler function that passes the file and line
// number where the BSLS_PRE_DONE was invoked.
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

#include <bsls_assert.h>

class PreDoneHandler
{
	private:
		static BloombergLP::bsls::Assert::ViolationHandler d_handler;

	public:
		//PreDoneHandler(pointerToFunction handler) {}

		static void installHandler( BloombergLP::bsls::Assert::ViolationHandler handler)
		{
			d_handler = handler;
		}
		static void invokeHandler()
		{
			BloombergLP::bsls::AssertViolation av("preDone", __FILE__, __LINE__, "");
			d_handler(av);
		}
};

#define BSLS_PRE_DONE() PreDoneHandler::invokeHandler();


#else // fuzzing disabled

#define BSLS_PRE_DONE()

#endif

#endif