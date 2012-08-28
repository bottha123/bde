// bslalg_bidirectionallink.t.cpp                                     -*-C++-*-
#include <bslalg_bidirectionallink.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//
// Certain standard value-semantic-type test cases are omitted:
//: o [ 8] -- 'swap' is not implemented for this class.
//
// Global Concerns:
//: o The test driver is robust w.r.t. reuse in other, similar components.
//: o ACCESSOR methods are declared 'const'.
//: o CREATOR & MANIPULATOR pointer/reference parameters are declared 'const'.
//: o Precondition violations are detected in appropriate build modes.
//: o No memory is allocated from any allocator.
//
// Note that all contracts are wide, so there are no precondition violations
// to be detect in any build mode.
//
// Global Assumptions:
//: o ACCESSOR methods are 'const' thread-safe.
//: o Individual attribute types are presumed to be *alias-safe*; hence, only
//:   certain methods require the testing of this property:
//:   o copy-assignment
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] BidirectionalLink()
// [ 6] BidirectionalLink(const BidirectionalLink& original);
// [ 2] ~BidirectionalLink();
//
// MANIPULATORS
// [ 7] BidirectionalLink& operator=(const BidirectionalLink& rhs);
// [ 2] void setNext(BidirectionalLink *next);
// [ 2] void setPrev(BidirectionalLink *prev);
//
// ACCESSORS
// [ 8] BidirectionalLink *next();
// [ 8] BidirectionalLink *prev();
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
// [ 3] 'gg' FUNCTION
// [ 2] CONCERN: All creator/manipulator ptr./ref. parameters are 'const'.
// [ 4] CONCERN: All accessor methods are declared 'const'.
// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslalg::BidirectionalLink Obj;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
namespace BloombergLP {
namespace bslalg {

void debugprint(const Obj& val)
{
    printf("prev=0x%x, next=0x%x", val.prev(), val.next());
}

} // close namespace bslalg
} // close enterprise namespace

static Obj& gg(Obj *result, Obj *prev, Obj *next)
    // Initialize the specified 'result' with the specified 'prev', and 'next'.
{
    ASSERT(result);

    result->setPrev(prev);
    result->setNext(next);
    return *result;
}

static bool operator==(const Obj& lhs, const Obj& rhs)
    // Convenience function to verify all attributes of the specified 'lhs' has
    // the same value as the attributes of the specified 'rhs'.
{
    return (lhs.prev() == rhs.prev()
         && lhs.next() == rhs.next());
}

static bool operator!=(const Obj& lhs, const Obj& rhs)
    // Convenience function to verify at least one attribute of the specified
    // 'lhs' has different value from the attributes of the specified 'rhs'.
{
    return !(lhs == rhs);
}

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA (potentially) used by test cases 3, 7, (8), 9, and (10)

struct DefaultDataRow {
    int    d_line;           // source line number
    Obj *  d_prevNode;
    Obj *  d_nextNode;
};

Obj *const PTR1 = (Obj *)0xbaadf00ddeadc0deULL;
Obj *const PTR2 = (Obj *)0xbaadf00ddeadbeefULL;

static
const DefaultDataRow DEFAULT_DATA[] =
{
    //LINE     PREV        NEXT
    //----     ----        ----

    { L_,         0,          0 },

    // 'prev'
    { L_,  (Obj *)4,          0 },
    { L_,  PTR2,              0 },

    // 'next'
    { L_,         0,   (Obj *)4 },
    { L_,         0,       PTR2 },

    // other
    { L_,  (Obj *)4,       PTR2 },
    { L_,      PTR2,   (Obj *)4 },
};
const int DEFAULT_NUM_DATA = sizeof DEFAULT_DATA / sizeof *DEFAULT_DATA;


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2;
    bool veryVerbose         = argc > 3;
    bool veryVeryVerbose     = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Using the table-driven technique, specify a set of (unique) valid
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 3 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3:  (C-1, 3..4)
        //:
        //:   1 Create two 'const' 'Obj', 'Z' and 'ZZ', each having the value
        //:     'V'.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-3.2):  (C-1, 3..4)
        //:
        //:     1 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:       having the value 'W'.
        //:
        //:     2 Assign 'mX' from 'Z'.
        //:
        //:     3 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-3)
        //:
        //:     4 Use the equality-comparison operator to verify that: (C-1, 4)
        //:
        //:       1 The target object, 'mX', now has the same value as that of
        //:         'Z'.  (C-1)
        //:
        //:       2 'Z' still has the same value as that of 'ZZ'.  (C-4)
        //:
        //: 4 Repeat steps similar to those described in P-2 except that, this
        //:   time, the source object, 'Z', is a reference to the target
        //:   object, 'mX', and both 'mX' and 'ZZ' are initialized to have the
        //:   value 'V'.  For each row (representing a distinct object value,
        //:   'V') in the table described in P-2:  (C-5)
        //:
        //:   1 Use the value constructor to create a modifiable 'Obj' 'mX';
        //:     also use the value constructor to create a 'const' 'Obj' 'ZZ'.
        //:
        //:   2 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   3 Assign 'mX' from 'Z'.
        //:
        //:   4 Verify that the address of the return value is the same as that
        //:     of 'mX'.  (C-3)
        //:
        //:   5 Use the equality-comparison operator to verify that the
        //:     target object, 'Z', still has the same value as that of 'ZZ'.
        //:     (C-5)
        //
        // Testing:
        //   bslalg::RbTreeNode& operator=(const bslalg::RbTreeNode& rhs);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY-ASSIGNMENT OPERATOR"
                            "\n========================\n");

        if (verbose) printf(
                      "\nAssign the address of the operator to a variable.\n");
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) printf("\nUse table of distinct object values.\n");

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) printf("\nCopy-assign every value into every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int  LINE1 = DATA[ti].d_line;
            Obj *const PREV1 = DATA[ti].d_prevNode;
            Obj *const NEXT1 = DATA[ti].d_nextNode;

            Obj  mZ; gg( &mZ, PREV1, NEXT1);
            const Obj& Z = mZ;
            Obj mZZ; gg(&mZZ, PREV1, NEXT1);
            const Obj& ZZ = mZZ;

            if (veryVerbose) { T_ P_(LINE1) P_(Z) P(ZZ) }

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int  LINE2 = DATA[tj].d_line;
                Obj *const PREV2 = DATA[tj].d_prevNode;
                Obj *const NEXT2 = DATA[tj].d_nextNode;

                Obj mX; const Obj& X = gg(&mX, PREV2, NEXT2);

                if (veryVerbose) { T_ P_(LINE2) P(X) }

                ASSERTV(LINE1, LINE2, Z, X, (Z == X) == (LINE1 == LINE2));

                Obj *mR = &(mX = Z);
                ASSERTV(LINE1, LINE2,  Z,   X,  Z == X);
                ASSERTV(LINE1, LINE2, mR, &mX, mR == &mX);

                ASSERTV(LINE1, LINE2, ZZ, Z, ZZ == Z);
            }

            if (verbose) printf("Testing self-assignment\n");

            {
                Obj  mX; gg( &mX, PREV1, NEXT1);
                Obj mZZ; const Obj& ZZ = gg(&mZZ, PREV1, NEXT1);

                const Obj& Z = mX;

                ASSERTV(LINE1, ZZ, Z, ZZ == Z);

                Obj *mR = &(mX = Z);
                ASSERTV(LINE1, ZZ,   Z, ZZ == Z);
                ASSERTV(LINE1, mR, &mX, mR == &mX);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of (unique) valid
        //:   object values (one per row) in terms of their individual
        //:   attributes, including (a) first, the default value, and (b)
        //:   boundary values corresponding to every range of values that each
        //:   individual attribute can independently attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1:  (C-1..3)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V'.
        //:
        //:   2 Use the copy constructor to create an object 'X',
        //:     supplying it the 'const' object 'Z'.  (C-2)
        //:
        //:   3 Use the equality-comparison operator to verify that:
        //:     (C-1, 3)
        //:
        //:     1 The newly constructed object, 'X', has the same value as 'Z'.
        //:       (C-1)
        //:
        //:     2 'Z' still has the same value as 'ZZ'.  (C-3)
        //
        // Testing:
        //   BidirectionalLink(const BidirectionalLink& original);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCOPY CONSTRUCTOR"
                            "\n================\n");

        if (verbose) printf("\nUse table of distinct object values.\n");

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) printf("\nCopy construct an object from every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_line;
            Obj *const  PREV  = DATA[ti].d_prevNode;
            Obj *const  NEXT  = DATA[ti].d_nextNode;

            Obj  mZ;  const Obj  Z = gg( &mZ, PREV, NEXT);
            Obj mZZ;  const Obj ZZ = gg(&mZZ, PREV, NEXT);

            if (veryVerbose) { T_ P_(Z) P(ZZ) }

            Obj mX(Z);  const Obj& X = mX;

            if (veryVerbose) { T_ T_ P(X) }

            // Verify the value of the object.

            ASSERTV(LINE,  Z, X,  Z == X);

            // Verify that the value of 'Z' has not changed.

            ASSERTV(LINE, ZZ, Z, ZZ == Z);
        }  // end foreach row

      } break;

      case 5: {
        // There is no test case 5 ...
      } break;

      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //
        // Plan:
        //   In case 3 we demonstrated that all basic accessors work properly
        //   with respect to attributes initialized by the value constructor.
        //   Here we use the default constructor and primary manipulators,
        //   which were fully tested in case 2, to further corroborate that
        //   these accessors are properly interpreting object state.
        //
        //: 1 Use the default constructor to create an object (having default
        //:   attribute values).
        //:
        //: 2 Verify that each basic accessor, invoked on a reference providing
        //:   non-modifiable access to the object created in P2, returns the
        //:   expected value.  (C-2)
        //:
        //: 3 For each salient attribute (contributing to value):  (C-1)
        //:   1 Use the corresponding primary manipulator to set the attribute
        //:     to a unique value.
        //:
        //:   2 Use the corresponding basic accessor to verify the new
        //:     expected value.  (C-1)
        //
        // Testing:
        //   BidirectionalLink *prev() const
        //   BidirectionalLink *next() const
        // --------------------------------------------------------------------

        if (verbose) printf("\nBASIC ACCESSORS"
                            "\n===============\n");

        // Attribute Types

        typedef Obj * T3;  // 'prev'
        typedef Obj * T4;  // 'next'

        if (verbose) printf("\nEstablish suitable attribute values.\n");

        // -----------------------------------------------------
        // 'D' values: These are the default-constructed values.
        // -----------------------------------------------------

        const T3 D3 = 0;    // 'prev'
        const T4 D4 = 0;    // 'next'

        // -------------------------------------------------------
        // 'A' values: Boundary values.
        // -------------------------------------------------------

        const T3 A3 = PTR1;
        const T4 A4 = PTR2;

        if (verbose) printf("\nCreate an object.\n");

        Obj mX;  const Obj& X = gg(&mX, D3, D4);

        if (verbose) printf(
                     "\nVerify all basic accessors report expected values.\n");
        {
            ASSERTV(D3, X.prev(), D3 == X.prev());
            ASSERTV(D4, X.next(), D4 == X.next());
        }

        if (verbose) printf(
                 "\nApply primary manipulators and verify expected values.\n");

        if (veryVerbose) { T_ Q(prev) }
        {
            mX.setPrev(A3);

            const Obj *const prev = X.prev();
            ASSERTV(A3, X.prev(), A3 == X.prev());
        }

        if (veryVerbose) { T_ Q(next) }
        {
            mX.setNext(A4);

            const Obj *const next = X.next();
            ASSERTV(A4, X.next(), A4 == X.next());
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'gg' FUNCTION
        //   Ensure that we can put an object into any initial state relevant
        //   for thorough testing.
        //
        // Concerns:
        //: 1 The 'gg' can create an object having any value that does not
        //:   violate the constructor's documented preconditions.
        //:
        //: 2 Any argument can be 'const'.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:   1 Specify a set of (unique) valid object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, and (b) boundary values corresponding to every
        //:     range of values that each individual attribute can
        //:     independently attain.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..2)
        //:
        //:   1 Use the value constructor to create an object having the value
        //:     'V' supplying all the arguments as 'const'. (C-2)
        //:
        //:   2 Use the (as yet unproven) salient attribute accessors to verify
        //:     that all of the attributes of each object have their expected
        //:     values.  (C-1)
        //
        // Testing:
        //   Obj& gg(Obj *r, Obj *l, Obj *r)
        // --------------------------------------------------------------------

        if (verbose) printf("\nVALUE CTOR"
                            "\n==========\n");

        if (verbose) printf("\nUse table of distinct object values.\n");

        const int NUM_DATA                     = DEFAULT_NUM_DATA;
        const DefaultDataRow (&DATA)[NUM_DATA] = DEFAULT_DATA;

        if (verbose) printf("\nCreate an object with every value.\n");

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_line;
            Obj *const  LEFT   = DATA[ti].d_prevNode;
            Obj *const  RIGHT  = DATA[ti].d_nextNode;

            if (veryVerbose) { T_ P_(LEFT) P_(RIGHT) }

            Obj mX;  const Obj& X = gg(&mX, LEFT, RIGHT);

            if (veryVerbose) { T_ T_ P(X) }

            // -------------------------------------
            // Verify the object's attribute values.
            // -------------------------------------

            ASSERTV(LINE, LEFT,  X.prev(), LEFT  == X.prev());
            ASSERTV(LINE, RIGHT, X.next(), RIGHT == X.next());
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // DEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR
        //   Ensure that we can use the default constructor to create an
        //   object (having the default-constructed value), use the primary
        //   manipulators to put that object into any state relevant for
        //   thorough testing, and use the destructor to destroy it safely.
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 Any argument can be 'const'.
        //:
        //: 3 Each attribute is modifiable independently.
        //:
        //: 4 Each attribute can be set to represent any value that does not
        //:   violate that attribute's documented constraints.
        //
        // Plan:
        //: 1 Create three sets of attribute values for the object: 'D', 'A',
        //:   and 'B'.  'D' values correspond to the default attribute values,
        //:   and 'A' and 'B' values are chosen to be distinct boundary values
        //:   where possible.
        //:
        //: 2 Use the default constructor to create an object 'X'.
        //:
        //: 3 Use the individual (as yet unproven) salient attribute
        //:   accessors to verify the default-constructed value.  (C-1)
        //:
        //: 4 For each attribute 'i', in turn, create a local block.  Then
        //:   inside the block, using brute force, set that attribute's
        //:   value, passing a 'const' argument representing each of the
        //:   three test values, in turn (see P-1), first to 'Ai', then to
        //:   'Bi', and finally back to 'Di'.  After each transition, use the
        //:   (as yet unproven) basic accessors to verify that only the
        //:   intended attribute value changed.  (C-2, 4)
        //:
        //: 5 Corroborate that attributes are modifiable independently by
        //:   first setting all of the attributes to their 'A' values.  Then
        //:   incrementally set each attribute to its 'B' value and verify
        //:   after each manipulation that only that attribute's value
        //:   changed.  (C-3)
        //
        // Testing:
        //   BidirectionalLink();
        //   ~BidirectionalLink();
        //   void setPrev(BidirectionalLink *address)
        //   void setNext(BidirectionalLink *address)
        // --------------------------------------------------------------------

        if (verbose) printf("\nDEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR"
                            "\n==========================================\n");

        if (verbose) printf("\nEstablish suitable attribute values.\n");

        // 'D' values: These are the default-constructed values.

        Obj *const  D3 = 0;          // 'prev'
        Obj *const  D4 = 0;          // 'next'

        // 'A' values.

        Obj *const  A3 = (Obj *)0x08;
        Obj *const  A4 = (Obj *)0x0c;

        // 'B' values.

        Obj *const  B3 = PTR1;
        Obj *const  B4 = PTR2;

        if (verbose) printf(
                          "Create an object using the default constructor.\n");

        Obj mX;  const Obj& X = mX;

        if (verbose) printf("Verify the object's attribute values.\n");

        // -------------------------------------
        // Verify the object's attribute values.
        // -------------------------------------

        mX.setPrev(D3);
        mX.setNext(D4);

        ASSERTV(D3, X.prev(),  D3 == X.prev());
        ASSERTV(D4, X.next(),  D4 == X.next());

        if (verbose) printf(
                    "Verify that each attribute is independently settable.\n");

        // -----------
        // 'prev'
        // -----------
        {
            mX.setPrev(A3);
            ASSERT(A3 == X.prev());
            ASSERT(D4 == X.next());

            mX.setPrev(B3);
            ASSERT(B3 == X.prev());
            ASSERT(D4 == X.next());

            mX.setPrev(D3);
            ASSERT(D3 == X.prev());
            ASSERT(D4 == X.next());
        }

        // ------------
        // 'next'
        // ------------
        {
            mX.setNext(A4);
            ASSERT(D3 == X.prev());
            ASSERT(A4 == X.next());

            mX.setNext(B4);
            ASSERT(D3 == X.prev());
            ASSERT(B4 == X.next());

            mX.setNext(D4);
            ASSERT(D3 == X.prev());
            ASSERT(D4 == X.next());
        }

        if (verbose) printf("Corroborate attribute independence.\n");
        {
            // ---------------------------------------
            // Set all attributes to their 'A' values.
            // ---------------------------------------

            mX.setPrev(A3);
            mX.setNext(A4);

            ASSERT(A3 == X.prev());
            ASSERT(A4 == X.next());


            // ---------------------------------------
            // Set all attributes to their 'B' values.
            // ---------------------------------------

            mX.setPrev(B3);
            ASSERT(B3 == X.prev());
            ASSERT(A4 == X.next());

            mX.setNext(B4);
            ASSERT(B3 == X.prev());
            ASSERT(B4 == X.next());

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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        // Attribute Types

        typedef Obj * T1;  // 'prev'
        typedef Obj * T2;  // 'next'

        // Attribute 1 Values: 'prev'

        const T1 D1 = 0;        // default value
        const T1 A1 = PTR2;

        // Attribute 2 Values: 'next'

        const T2 D2 = 0;        // default value
        const T2 A2 = PTR2;


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n Create an object 'w'.\n");

        Obj mW;  const Obj& W = mW;
        mW.setPrev(D1);
        mW.setNext(D2);

        if (veryVerbose) printf("\ta. Check initial value of 'w'.\n");
        if (veryVeryVerbose) { T_ T_ P(W) }

        ASSERT(D1 == W.prev());
        ASSERT(D2 == W.next());

        if (veryVerbose) printf(
                               "\tb. Try equality operators: 'w' <op> 'w'.\n");

        ASSERT(1 == (W == W));        ASSERT(0 == (W != W));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n Create 'X' from 'W').\n");

        Obj mX(W);  const Obj& X = mX;

        if (veryVerbose) printf("\ta. Check new value of 'x'.\n");
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(D1 == X.prev());
        ASSERT(D2 == X.next());


        if (veryVerbose) printf(
                          "\tb. Try equality operators: 'x' <op> 'w', 'x'.\n");

        ASSERT(1 == (X == W));        ASSERT(0 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf(
                    "\n Set 'X' with values 'A' (value distinct from 'D').\n");

        mX.setPrev(A1);
        mX.setNext(A2);

        if (veryVerbose) printf("\ta. Check new value of 'x'.\n");
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == X.prev());
        ASSERT(A2 == X.next());


        if (veryVerbose) printf(
                          "\tb. Try equality operators: 'x' <op> 'w', 'x'.\n");

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) printf("\n Create 'Y' and set 'Y' with 'X'.\n");

        Obj mY;  const Obj& Y = mY;
        mY = X;

        if (veryVerbose) printf("\ta. Check new value of 'x'.\n");
        if (veryVeryVerbose) { T_ T_ P(X) }

        ASSERT(A1 == Y.prev());
        ASSERT(A2 == Y.next());


        if (veryVerbose) printf(
                          "\tb. Try equality operators: 'x' <op> 'w', 'x'.\n");

        ASSERT(0 == (X == W));        ASSERT(1 == (X != W));
        ASSERT(1 == (X == X));        ASSERT(0 == (X != X));
        ASSERT(1 == (X == Y));        ASSERT(0 == (X != Y));

      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
