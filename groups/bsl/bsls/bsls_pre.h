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


#ifdef BDE_ACTIVATE_FUZZ_TESTING

#define BSLS_PRE_DONE_ENABLED

#include <bsls_assert.h>
#include <bsls_pointercastutil.h>

namespace BloombergLP {
namespace bsls {

class PreDoneHandler
{
	private:
	 	static AtomicOperations::AtomicTypes::Pointer  d_handler;

		static Assert::ViolationHandler getPreDoneHandler() {
			return (Assert::ViolationHandler) AtomicOperations::getPtrAcquire(&d_handler);
		}

	public:
		static void installHandler(Assert::ViolationHandler handler)
		{
			AtomicOperations::setPtrRelease(&d_handler, PointerCastUtil::cast<void *>(handler));
		}
		static void invokeHandler()
		{
			AssertViolation av("preDone", __FILE__, __LINE__, "");
					
			Assert::ViolationHandler preDoneHandlerPtr = getPreDoneHandler();

			preDoneHandlerPtr(av);
		}
};

#define BSLS_PRE_DONE() bsls::PreDoneHandler::invokeHandler();


#else // fuzzing disabled

#define BSLS_PRE_DONE()

#endif

}  // close package namespace
}  // close enterprise namespace

#endif