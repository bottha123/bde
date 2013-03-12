// bdefi_vfunc0.h               -*-C++-*-
#ifndef INCLUDED_BDEFI_VFUNC0
#define INCLUDED_BDEFI_VFUNC0

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")


//@PURPOSE: Provide concrete representations for 0-arg void function objects.
//
//@CLASSES:
// bdefi_Vfunc0Null: a 0-arg void no-op functor
//
// bdefi_Vfunc0F0: a 0-arg void functor created from a 0-arg function
// bdefi_Vfunc0F1: a 0-arg void functor created from a 1-arg function
// bdefi_Vfunc0F2: a 0-arg void functor created from a 2-arg function
// bdefi_Vfunc0F3: a 0-arg void functor created from a 3-arg function
// bdefi_Vfunc0F4: a 0-arg void functor created from a 4-arg function
// bdefi_Vfunc0F5: a 0-arg void functor created from a 5-arg function
// bdefi_Vfunc0F6: a 0-arg void functor created from a 6-arg function
// bdefi_Vfunc0F7: a 0-arg void functor created from a 7-arg function
// bdefi_Vfunc0F8: a 0-arg void functor created from a 8-arg function
// bdefi_Vfunc0F9: a 0-arg void functor created from a 9-arg function
//
// bdefi_Vfunc0C0: a 0-arg void functor created from obj & 0-arg method
// bdefi_Vfunc0C1: a 0-arg void functor created from obj & 1-arg method
// bdefi_Vfunc0C2: a 0-arg void functor created from obj & 2-arg method
// bdefi_Vfunc0C3: a 0-arg void functor created from obj & 3-arg method
// bdefi_Vfunc0C4: a 0-arg void functor created from obj & 4-arg method
// bdefi_Vfunc0C5: a 0-arg void functor created from obj & 5-arg method
// bdefi_Vfunc0C6: a 0-arg void functor created from obj & 6-arg method
// bdefi_Vfunc0C7: a 0-arg void functor created from obj & 7-arg method
// bdefi_Vfunc0C8: a 0-arg void functor created from obj & 8-arg method
// bdefi_Vfunc0C9: a 0-arg void functor created from obj & 9-arg method
//
// bdefi_Vfunc0M0: a 0-arg void functor created from obj ptr & 0-arg method
// bdefi_Vfunc0M1: a 0-arg void functor created from obj ptr & 1-arg method
// bdefi_Vfunc0M2: a 0-arg void functor created from obj ptr & 2-arg method
// bdefi_Vfunc0M3: a 0-arg void functor created from obj ptr & 3-arg method
// bdefi_Vfunc0M4: a 0-arg void functor created from obj ptr & 4-arg method
// bdefi_Vfunc0M5: a 0-arg void functor created from obj ptr & 5-arg method
// bdefi_Vfunc0M6: a 0-arg void functor created from obj ptr & 6-arg method
// bdefi_Vfunc0M7: a 0-arg void functor created from obj ptr & 7-arg method
// bdefi_Vfunc0M8: a 0-arg void functor created from obj ptr & 8-arg method
// bdefi_Vfunc0M9: a 0-arg void functor created from obj ptr & 9-arg method
//
//@AUTHOR: John Lakos (jlakos)
//
//@DEPRECATED: This component should not be used in new code, and will be
// deleted in the near future.  Please see 'bdef_function', 'bdef_bind', etc.
// for alternatives that should be used for all future development.
//
//@DESCRIPTION: This component contains a suite of classes to implement the
// polymorphic representation of a function object (functor) whose
// function-call operator ('execute') takes zero argument and returns 'void'
// (hence the root name 'vfunc0' -- "v" for 'void' and "0" for the number of
// arguments).  Direct clients of this component choose a concrete
// implementation of the base 'bdefr_Vfunc0' representation based on the
// number of additional arguments required by the underlying procedure, and on
// the specific kind or "flavor" of function that the functor is to invoke.
// If the user does not wish to invoke any function on the functor invocation,
// then the "null functor" 'bdefi_Vfunc0Null' may be used.
//
///Language-Imposed Details
///------------------------
// For practical reasons, this component must distinguish among four types of
// functors.  The first type is a "null functor" 'bdefi_Vfunc0Null', which
// differs from the other three types in that it does not invoke any function.
//
// The other functors are distinguished by three flavors 'X' of functions they
// invoke, and also by the total number 'N' of arguments (N >= 0) that the
// function takes.  This component supplies a separate concrete implementation
// for each of the specified types.  These 30 non-null concrete
// implementations are named 'bdefi_Vfunc0XN' where 'X' is one of 'F', 'C', or
// 'M', and 'N' is in the range [0..9].
//
// The possible function flavors (and labels) are classified as:
//..
//     Label   Classification
//     -----   --------------
//       F     free function, static member function, or "free" functor object
//       C     'const' (accessor) member function
//       M     non-'const' (manipulator) member function
//..
// Note that a 'const' member function may be wrapped by a 'bdefi_Vfunc0MN'
// functor, but wrapping a non-'const' method by a 'bdefi_Vfunc0CN' will
// result in compile time error in most compiler implementations.
//
// Note also that, to accommodate the preservation of 'const', the
// 'bdefi_Vfunc0CN' functors hold an object 'Obj', supplied by a 'const'
// 'Obj&' in the constructor, while 'bdefi_Vfunc0MN' functors hold a pointer
// to 'Obj' supplied by a modifiable 'Obj*' argument in the constructor.
//
// The (N) arguments specified at the construction of the functor are held by
// the functor, and passed into the virtual 'execute' method when the
// underlying function is invoked by 'execute'.  Any value returned from the
// underlying procedure is ignored.
//
// Note that the respective types of the parameters of the underlying procedure
// must correspond to the overall parameterization of the particular template
// representation class.  The virtual 'execute' method in each representation
// class defined in this component takes zero argument.  The respective types
// of parameters to the underlying procedure must match (or be convertible
// from) those specified in the particular representation class's constructor.
//
///USAGE
///-----
// Given the following free function, which takes four arguments (and happens
// to return an 'int' value)
//..
//   int myFunction(int a, double b, const char *c, int d);
//..
// we can create a (dynamically-allocated) zero-argument functor object that,
// at construction, wraps up the function along with the four additional
// arguments, and subsequently refer to this 'bdefi_Vfunc0F4' functor only
// through its base class, 'bdefr_Vfunc0':
//..
//   typedef int (*MyFunctionPtr)(int, double, const char *, int);
//   typedef bdefi_Vfunc0F4<MyFunctionPtr,
//                        int, double, const char *, int> MyFunctorRep;
//
//   bslma::Allocator *ba = bslma::Default::allocator(0);
//
//   bdefr_Vfunc0 *rep =
//                      new(ba) MyFunctorRep(myFunction, 1, "3", 4, 10, ba);
//..
// The virtual 'execute' method (taking no arguments) can now be called through
// the 'bdefr_Vfunc0' base class pointer 'rep'.  This method will, in turn,
// invoke the underlying function with no argument supplied by the caller of
// the 'execute' method, concatenated with the sequence of three additional
// (trailing) arguments supplied at construction.
//..
//   rep->execute();
//..
// The derived object is destroyed through the 'deleteObject' class method of
// the base class, which allows us to declare its destructor 'private'.
//..
//   bdefr_Vfunc0::deleteObject(rep);
//..
// This mechanism allows us to hide the details of destruction and memory
// deallocation from the user of this class.  By declaring the destructor
// private we also prevent creation of (automatic) instances on the program
// stack, which precludes accidental use of 'delete' and also prevents further
// derivation.
//
// Finally, note that the integer returned to the functor by 'myFunction' is
// ignored.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_CONSTRUCTORPROXY
#include <bslalg_constructorproxy.h>
#endif

#ifndef INCLUDED_BDEFR_VFUNC0
#include <bdefr_vfunc0.h>
#endif


namespace BloombergLP {

                        // ======================
                        // class bdefi_Vfunc0Null
                        // ======================

class bdefi_Vfunc0Null : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no argument, returning
    // 'void', and performing no operations.

    // NOT IMPLEMENTED
    bdefi_Vfunc0Null(const bdefi_Vfunc0Null&);
    bdefi_Vfunc0Null& operator=(const bdefi_Vfunc0Null&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0Null();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the no-op function.

  public:
    // CREATORS
    bdefi_Vfunc0Null(bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments, returning 'void', and performing no operation.
};

                        // ====================
                        // class bdefi_Vfunc0F0
                        // ====================

template <class F>
class bdefi_Vfunc0F0 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking no additional arguments.

    // DATA
    bslalg::ConstructorProxy<F> d_f;  // function pointer (or functor)

    // NOT IMPLEMENTED
    bdefi_Vfunc0F0(const bdefi_Vfunc0F0<F>&);
    bdefi_Vfunc0F0<F>& operator=(const bdefi_Vfunc0F0<F>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0F0();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor).

  public:
    // CREATORS
    bdefi_Vfunc0F0(F procedure, bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking no
        // additional arguments.  Use the specified 'basicAllocator' to supply
        // memory.  Note that any value returned from 'procedure' will be
        // ignored.
};

                        // ====================
                        // class bdefi_Vfunc0F1
                        // ====================

template <class F, class D1>
class bdefi_Vfunc0F1 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking one additional trailing
    // argument, and that argument's corresponding value.

    // DATA
    bslalg::ConstructorProxy<F>  d_f;   // function pointer (or functor)
    bslalg::ConstructorProxy<D1> d_d1;  // first embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0F1(const bdefi_Vfunc0F1<F, D1>&);
    bdefi_Vfunc0F1<F, D1>& operator=(const bdefi_Vfunc0F1<F, D1>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0F1();
        // Destroy this functor.  Note that this destructor can be only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the trailing argument value specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0F1(F                 procedure,
                   const D1&         embeddedArg1,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // one additional trailing argument, and this corresponding argument's
        // specified 'embeddedArg1' value.  Use the specified 'basicAllocator'
        // to supply memory.  Note that any value returned from 'procedure'
        // will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0F2
                        // ====================

template <class F, class D1, class D2>
class bdefi_Vfunc0F2 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking two additional trailing
    // arguments, and those arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<F>  d_f;   // function pointer (or functor)
    bslalg::ConstructorProxy<D1> d_d1;  // first embedded argument
    bslalg::ConstructorProxy<D2> d_d2;  // second embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0F2(const bdefi_Vfunc0F2<F, D1, D2>&);
    bdefi_Vfunc0F2<F, D1, D2>& operator=(const bdefi_Vfunc0F2<F, D1, D2>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0F2();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the sequence of two argument values
        // specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc0F2(F                 procedure,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // two additional trailing arguments, and the corresponding arguments'
        // specified 'embeddedArg1' and 'embeddedArg2' values.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'procedure' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0F3
                        // ====================

template <class F, class D1, class D2, class D3>
class bdefi_Vfunc0F3 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking three additional trailing
    // arguments, and those arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<F>  d_f;   // function pointer (or functor)
    bslalg::ConstructorProxy<D1> d_d1;  // first embedded argument
    bslalg::ConstructorProxy<D2> d_d2;  // second embedded argument
    bslalg::ConstructorProxy<D3> d_d3;  // third embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0F3(const bdefi_Vfunc0F3<F, D1, D2, D3>&);
    bdefi_Vfunc0F3<F, D1, D2, D3>& operator=(
                   const bdefi_Vfunc0F3<F, D1, D2, D3>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0F3();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the sequence of three argument values
        // specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc0F3(F                 procedure,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // three additional trailing arguments and the corresponding
        // arguments' specified 'embeddedArg1', 'embeddedArg2, and
        // 'embeddedArg3' values.  Use the specified 'basicAllocator' to
        // supply memory.  Note that any value returned from 'procedure' will
        // be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0F4
                        // ====================

template <class F, class D1, class D2, class D3, class D4>
class bdefi_Vfunc0F4 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking four additional trailing
    // arguments, and those arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<F>  d_f;   // function pointer (or functor)
    bslalg::ConstructorProxy<D1> d_d1;  // first embedded argument
    bslalg::ConstructorProxy<D2> d_d2;  // second embedded argument
    bslalg::ConstructorProxy<D3> d_d3;  // third embedded argument
    bslalg::ConstructorProxy<D4> d_d4;  // fourth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0F4(const bdefi_Vfunc0F4<F, D1, D2, D3, D4>&);
    bdefi_Vfunc0F4<F, D1, D2, D3, D4>& operator=(
                   const bdefi_Vfunc0F4<F, D1, D2, D3, D4>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0F4();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the sequence of four argument values
        // specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc0F4(F                 procedure,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // four additional trailing arguments and the corresponding arguments'
        // specified 'embeddedArg1', 'embeddedArg2, 'embeddedArg3', and
        // 'embeddedArg4' values.  Use the specified 'basicAllocator' to
        // supply memory.  Note that any value returned from 'procedure' will
        // be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0F5
                        // ====================

template <class F, class D1, class D2, class D3, class D4,
          class D5>
class bdefi_Vfunc0F5 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking five additional trailing
    // arguments, and those arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<F>  d_f;   // function pointer (or functor)
    bslalg::ConstructorProxy<D1> d_d1;  // first embedded argument
    bslalg::ConstructorProxy<D2> d_d2;  // second embedded argument
    bslalg::ConstructorProxy<D3> d_d3;  // third embedded argument
    bslalg::ConstructorProxy<D4> d_d4;  // fourth embedded argument
    bslalg::ConstructorProxy<D5> d_d5;  // fifth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0F5(const bdefi_Vfunc0F5<F, D1, D2, D3, D4, D5>&);
    bdefi_Vfunc0F5<F, D1, D2, D3, D4, D5>& operator=(
                   const bdefi_Vfunc0F5<F, D1, D2, D3, D4, D5>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0F5();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the sequence of five argument values
        // specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc0F5(F                 procedure,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // five additional trailing arguments and the corresponding arguments'
        // specified 'embeddedArg1', 'embeddedArg2, 'embeddedArg3',
        // 'embeddedArg4', and 'embeddedArg5' values.  Use the specified
        // 'basicAllocator' to supply memory.  Note that any value returned
        // from 'procedure' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0F6
                        // ====================

template <class  F, class D1, class D2, class D3, class D4,
          class D5, class D6>
class bdefi_Vfunc0F6 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking six additional trailing
    // arguments, and those arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<F>  d_f;   // function pointer (or functor)
    bslalg::ConstructorProxy<D1> d_d1;  // first embedded argument
    bslalg::ConstructorProxy<D2> d_d2;  // second embedded argument
    bslalg::ConstructorProxy<D3> d_d3;  // third embedded argument
    bslalg::ConstructorProxy<D4> d_d4;  // fourth embedded argument
    bslalg::ConstructorProxy<D5> d_d5;  // fifth embedded argument
    bslalg::ConstructorProxy<D6> d_d6;  // sixth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0F6(const bdefi_Vfunc0F6<F, D1, D2, D3, D4, D5, D6>&);
    bdefi_Vfunc0F6<F, D1, D2, D3, D4, D5, D6>& operator=(
                   const bdefi_Vfunc0F6<F, D1, D2, D3, D4, D5, D6>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0F6();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the sequence of six argument values
        // specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc0F6(F                 procedure,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   const D6&         embeddedArg6,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // six additional trailing arguments and the corresponding arguments'
        // specified 'embeddedArg1', 'embeddedArg2, 'embeddedArg3',
        // 'embeddedArg4', 'embeddedArg5', and 'embeddedArg6' values.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'procedure' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0F7
                        // ====================

template <class  F, class D1, class D2, class D3, class D4,
          class D5, class D6, class D7>
class bdefi_Vfunc0F7 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking seven additional trailing
    // arguments, and those arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<F>  d_f;   // function pointer (or functor)
    bslalg::ConstructorProxy<D1> d_d1;  // first embedded argument
    bslalg::ConstructorProxy<D2> d_d2;  // second embedded argument
    bslalg::ConstructorProxy<D3> d_d3;  // third embedded argument
    bslalg::ConstructorProxy<D4> d_d4;  // fourth embedded argument
    bslalg::ConstructorProxy<D5> d_d5;  // fifth embedded argument
    bslalg::ConstructorProxy<D6> d_d6;  // sixth embedded argument
    bslalg::ConstructorProxy<D7> d_d7;  // seventh embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0F7(const bdefi_Vfunc0F7<F, D1, D2, D3, D4, D5, D6, D7>&);
    bdefi_Vfunc0F7<F, D1, D2, D3, D4, D5, D6, D7>& operator=(
                   const bdefi_Vfunc0F7<F, D1, D2, D3, D4, D5, D6, D7>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0F7();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the sequence of seven argument values
        // specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc0F7(F                 procedure,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   const D6&         embeddedArg6,
                   const D7&         embeddedArg7,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // seven additional trailing arguments and the corresponding
        // arguments' specified 'embeddedArg1', 'embeddedArg2, 'embeddedArg3',
        // 'embeddedArg4', 'embeddedArg5', 'embeddedArg6', and 'embeddedArg7'
        // values.  Use the specified 'basicAllocator' to supply memory.  Note
        // that any value returned from 'procedure' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0F8
                        // ====================

template <class  F, class D1, class D2, class D3, class D4,
          class D5, class D6, class D7, class D8>
class bdefi_Vfunc0F8 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking eight additional trailing
    // arguments, and those arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<F>  d_f;   // function pointer (or functor)
    bslalg::ConstructorProxy<D1> d_d1;  // first embedded argument
    bslalg::ConstructorProxy<D2> d_d2;  // second embedded argument
    bslalg::ConstructorProxy<D3> d_d3;  // third embedded argument
    bslalg::ConstructorProxy<D4> d_d4;  // fourth embedded argument
    bslalg::ConstructorProxy<D5> d_d5;  // fifth embedded argument
    bslalg::ConstructorProxy<D6> d_d6;  // sixth embedded argument
    bslalg::ConstructorProxy<D7> d_d7;  // seventh embedded argument
    bslalg::ConstructorProxy<D8> d_d8;  // eighth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0F8(const bdefi_Vfunc0F8<F, D1, D2, D3, D4, D5, D6, D7, D8>&);
    bdefi_Vfunc0F8<F, D1, D2, D3, D4, D5, D6, D7, D8>& operator=(
                   const bdefi_Vfunc0F8<F, D1, D2, D3, D4, D5, D6, D7, D8>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0F8();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the sequence of eight argument values
        // specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc0F8(F                 procedure,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   const D6&         embeddedArg6,
                   const D7&         embeddedArg7,
                   const D8&         embeddedArg8,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // eight additional trailing arguments and the corresponding
        // arguments' specified 'embeddedArg1', 'embeddedArg2, 'embeddedArg3',
        // 'embeddedArg4', 'embeddedArg5', 'embeddedArg6', 'embeddedArg7', and
        // 'embeddedArg8' values.  Use the specified 'basicAllocator' to
        // supply memory.  Note that any value returned from 'procedure' will
        // be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0F9
                        // ====================

template <class  F, class D1, class D2, class D3, class D4,
          class D5, class D6, class D7, class D8, class D9>
class bdefi_Vfunc0F9 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds a pure procedure (i.e., free function,
    // static member function, or functor) taking nine additional trailing
    // arguments, and those arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<F>  d_f;   // function pointer (or functor)
    bslalg::ConstructorProxy<D1> d_d1;  // first embedded argument
    bslalg::ConstructorProxy<D2> d_d2;  // second embedded argument
    bslalg::ConstructorProxy<D3> d_d3;  // third embedded argument
    bslalg::ConstructorProxy<D4> d_d4;  // fourth embedded argument
    bslalg::ConstructorProxy<D5> d_d5;  // fifth embedded argument
    bslalg::ConstructorProxy<D6> d_d6;  // sixth embedded argument
    bslalg::ConstructorProxy<D7> d_d7;  // seventh embedded argument
    bslalg::ConstructorProxy<D8> d_d8;  // eighth embedded argument
    bslalg::ConstructorProxy<D9> d_d9;  // ninth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0F9(const bdefi_Vfunc0F9<F, D1, D2, D3, D4, D5, D6,
                                        D7, D8, D9>&);
    bdefi_Vfunc0F9<F, D1, D2, D3, D4, D5, D6, D7, D8, D9>& operator=(
                   const bdefi_Vfunc0F9<F, D1, D2, D3, D4, D5, D6,
                                        D7, D8, D9>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0F9();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying procedure (free function, static member
        // function, or functor) with the sequence of eight argument values
        // specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc0F9(F                 procedure,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   const D6&         embeddedArg6,
                   const D7&         embeddedArg7,
                   const D8&         embeddedArg8,
                   const D9&         embeddedArg9,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'procedure'
        // (i.e., free function, static member function, or functor) taking
        // nine additional trailing arguments and the corresponding arguments'
        // specified 'embeddedArg1', 'embeddedArg2, 'embeddedArg3',
        // 'embeddedArg4', 'embeddedArg5', 'embeddedArg6', 'embeddedArg7',
        // 'embeddedArg8', and 'embeddedArg9' values.  Use the specified
        // 'basicAllocator' to supply memory.  Note that any value returned
        // from 'procedure' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0C0
                        // ====================

template <class OBJ, class C>
class bdefi_Vfunc0C0 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning void, that holds an object and one of that object's 'const'
    // member functions taking no additional arguments.

    // DATA
    bslalg::ConstructorProxy<OBJ> d_obj;  // object
    C                             d_m;    // const member function pointer

    // NOT IMPLEMENTED
    bdefi_Vfunc0C0(const bdefi_Vfunc0C0<OBJ, C>&);
    bdefi_Vfunc0C0<OBJ, C>& operator=(const bdefi_Vfunc0C0<OBJ, C>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0C0();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with no additional arguments.

  public:
    // CREATORS
    bdefi_Vfunc0C0(const OBJ&        object,
                   C                 method,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking no additional
        // arguments.  Use the specified 'basicAllocator' to supply memory.
        // Note that any value returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0C1
                        // ====================

template <class OBJ, class C, class D1>
class bdefi_Vfunc0C1 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking one additional trailing argument, and the
    // argument's corresponding value.

    // DATA
    bslalg::ConstructorProxy<OBJ> d_obj;  // object
    C                             d_m;    // const member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;   // first embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0C1(const bdefi_Vfunc0C1<OBJ, C, D1>&);
    bdefi_Vfunc0C1<OBJ, C, D1>& operator=(const bdefi_Vfunc0C1<OBJ, C, D1>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0C1();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the trailing argument value specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc0C1(const OBJ&        object,
                   C                 method,
                   const D1&         embeddedArg1,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking one additional
        // trailing argument, and the corresponding argument's specified
        // 'embeddedArg1' value.  Use the specified 'basicAllocator' to supply
        // memory.  Note that any value returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0C2
                        // ====================

template <class OBJ, class C, class D1, class D2>
class bdefi_Vfunc0C2 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking two additional trailing arguments, and the
    // arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<OBJ> d_obj;  // object
    C                             d_m;    // const member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;   // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;   // second embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0C2(const bdefi_Vfunc0C2<OBJ, C, D1, D2>&);
    bdefi_Vfunc0C2<OBJ, C, D1, D2>& operator=(
                   const bdefi_Vfunc0C2<OBJ, C, D1, D2>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0C2();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of two trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0C2(const OBJ&        object,
                   C                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking two additional
        // trailing arguments, and the corresponding arguments' specified
        // 'embeddedArg1' and 'embeddedArg2' values.  Use the specified
        // 'basicAllocator' to supply memory.  Note that any value returned
        // from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0C3
                        // ====================

template <class OBJ, class C, class D1, class D2, class D3>
class bdefi_Vfunc0C3 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking three additional trailing arguments, and the
    // arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<OBJ> d_obj;  // object
    C                             d_m;    // const member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;   // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;   // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;   // third embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0C3(const bdefi_Vfunc0C3<OBJ, C, D1, D2, D3>&);
    bdefi_Vfunc0C3<OBJ, C, D1, D2, D3>& operator=(
                   const bdefi_Vfunc0C3<OBJ, C, D1, D2, D3>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0C3();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of three trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0C3(const OBJ&        object,
                   C                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking three additional
        // trailing arguments, and the corresponding arguments' specified
        // 'embeddedArg1', 'embeddedArg2', and 'embeddedArg3' values.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0C4
                        // ====================

template <class OBJ, class C, class D1, class D2, class D3,
          class D4>
class bdefi_Vfunc0C4 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking four additional trailing arguments, and the
    // arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<OBJ> d_obj;  // object
    C                             d_m;    // const member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;   // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;   // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;   // third embedded argument
    bslalg::ConstructorProxy<D4>  d_d4;   // fourth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0C4(const bdefi_Vfunc0C4<OBJ, C, D1, D2, D3, D4>&);
    bdefi_Vfunc0C4<OBJ, C, D1, D2, D3, D4>& operator=(
                   const bdefi_Vfunc0C4<OBJ, C, D1, D2, D3, D4>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0C4();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of four trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0C4(const OBJ&        object,
                   C                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking four additional
        // trailing arguments, and the corresponding arguments' specified
        // 'embeddedArg1', 'embeddedArg2', 'embeddedArg3', and 'embeddedArg4'
        // values.  Use the specified 'basicAllocator' to supply memory.  Note
        // that any value returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0C5
                        // ====================

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5>
class bdefi_Vfunc0C5 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking five additional trailing arguments, and the
    // arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<OBJ> d_obj;  // object
    C                             d_m;    // const member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;   // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;   // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;   // third embedded argument
    bslalg::ConstructorProxy<D4>  d_d4;   // fourth embedded argument
    bslalg::ConstructorProxy<D5>  d_d5;   // fifth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0C5(const bdefi_Vfunc0C5<OBJ, C, D1, D2, D3, D4, D5>&);
    bdefi_Vfunc0C5<OBJ, C, D1, D2, D3, D4, D5>& operator=(
                   const bdefi_Vfunc0C5<OBJ, C, D1, D2, D3, D4, D5>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0C5();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of five trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0C5(const OBJ&        object,
                   C                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking five additional
        // trailing arguments, and the corresponding arguments' specified
        // 'embeddedArg1', 'embeddedArg2', 'embeddedArg3', 'embeddedArg4', and
        // 'embeddedArg5' values.  Use the specified 'basicAllocator' to
        // supply memory.  Note that any value returned from 'method' will be
        // ignored.
};

                        // ====================
                        // class bdefi_Vfunc0C6
                        // ====================

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6>
class bdefi_Vfunc0C6 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking six additional trailing arguments, and the
    // arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<OBJ> d_obj;  // object
    C                             d_m;    // const member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;   // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;   // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;   // third embedded argument
    bslalg::ConstructorProxy<D4>  d_d4;   // fourth embedded argument
    bslalg::ConstructorProxy<D5>  d_d5;   // fifth embedded argument
    bslalg::ConstructorProxy<D6>  d_d6;   // sixth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0C6(const bdefi_Vfunc0C6<OBJ, C, D1, D2, D3, D4, D5, D6>&);
    bdefi_Vfunc0C6<OBJ, C, D1, D2, D3, D4, D5, D6>& operator=(
                   const bdefi_Vfunc0C6<OBJ, C, D1, D2, D3, D4, D5, D6>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0C6();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of six trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0C6(const OBJ&        object,
                   C                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   const D6&         embeddedArg6,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking six additional
        // trailing arguments, and the corresponding arguments' specified
        // 'embeddedArg1', 'embeddedArg2', 'embeddedArg3', 'embeddedArg4',
        // 'embeddedArg5', and 'embeddedArg6' values.  Use the specified
        // 'basicAllocator' to supply memory.  Note that any value returned
        // from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0C7
                        // ====================

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7>
class bdefi_Vfunc0C7 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking seven additional trailing arguments, and the
    // arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<OBJ> d_obj;  // object
    C                             d_m;    // const member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;   // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;   // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;   // third embedded argument
    bslalg::ConstructorProxy<D4>  d_d4;   // fourth embedded argument
    bslalg::ConstructorProxy<D5>  d_d5;   // fifth embedded argument
    bslalg::ConstructorProxy<D6>  d_d6;   // sixth embedded argument
    bslalg::ConstructorProxy<D7>  d_d7;   // seventh embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0C7(const bdefi_Vfunc0C7<OBJ, C, D1, D2, D3, D4, D5, D6, D7>&);
    bdefi_Vfunc0C7<OBJ, C, D1, D2, D3, D4, D5, D6, D7>& operator=(
                   const bdefi_Vfunc0C7<OBJ, C, D1, D2, D3, D4, D5, D6, D7>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0C7();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of seven trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0C7(const OBJ&        object,
                   C                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   const D6&         embeddedArg6,
                   const D7&         embeddedArg7,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking seven additional
        // trailing arguments, and the corresponding arguments' specified
        // 'embeddedArg1', 'embeddedArg2', 'embeddedArg3', 'embeddedArg4',
        // 'embeddedArg5', 'embeddedArg6', and 'embeddedArg7' values.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0C8
                        // ====================

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8>
class bdefi_Vfunc0C8 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking eight additional trailing arguments, and the
    // arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<OBJ> d_obj;  // object
    C                             d_m;    // const member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;   // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;   // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;   // third embedded argument
    bslalg::ConstructorProxy<D4>  d_d4;   // fourth embedded argument
    bslalg::ConstructorProxy<D5>  d_d5;   // fifth embedded argument
    bslalg::ConstructorProxy<D6>  d_d6;   // sixth embedded argument
    bslalg::ConstructorProxy<D7>  d_d7;   // seventh embedded argument
    bslalg::ConstructorProxy<D8>  d_d8;   // eighth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0C8(const bdefi_Vfunc0C8<OBJ, C, D1, D2, D3, D4, D5,
                                        D6, D7, D8>&);
    bdefi_Vfunc0C8<OBJ, C, D1, D2, D3, D4, D5, D6, D7, D8>& operator=(
                   const bdefi_Vfunc0C8<OBJ, C, D1, D2, D3, D4, D5,
                                        D6, D7, D8>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0C8();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of eight trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0C8(const OBJ&        object,
                   C                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   const D6&         embeddedArg6,
                   const D7&         embeddedArg7,
                   const D8&         embeddedArg8,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking eight additional
        // trailing arguments, and the corresponding arguments' specified
        // 'embeddedArg1', 'embeddedArg2', 'embeddedArg3', 'embeddedArg4',
        // 'embeddedArg5', 'embeddedArg6', 'embeddedArg7' and 'embeddedArg8'
        // values.  Use the specified 'basicAllocator' to supply memory.  Note
        // that any value returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0C9
                        // ====================

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8, class D9>
class bdefi_Vfunc0C9 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds an object and one of that object's 'const'
    // member functions taking nine additional trailing arguments, and the
    // arguments' corresponding values.

    // DATA
    bslalg::ConstructorProxy<OBJ> d_obj;  // object
    C                             d_m;    // const member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;   // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;   // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;   // third embedded argument
    bslalg::ConstructorProxy<D4>  d_d4;   // fourth embedded argument
    bslalg::ConstructorProxy<D5>  d_d5;   // fifth embedded argument
    bslalg::ConstructorProxy<D6>  d_d6;   // sixth embedded argument
    bslalg::ConstructorProxy<D7>  d_d7;   // seventh embedded argument
    bslalg::ConstructorProxy<D8>  d_d8;   // eighth embedded argument
    bslalg::ConstructorProxy<D9>  d_d9;   // ninth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0C9(const bdefi_Vfunc0C9<OBJ, C, D1, D2, D3, D4, D5,
                                        D6, D7, D8, D9>&);
    bdefi_Vfunc0C9<OBJ, C, D1, D2, D3, D4, D5, D6, D7, D8, D9>& operator=(
                   const bdefi_Vfunc0C9<OBJ, C, D1, D2, D3, D4, D5,
                                        D6, D7, D8, D9>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0C9();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of eight trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0C9(const OBJ&        object,
                   C                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   const D6&         embeddedArg6,
                   const D7&         embeddedArg7,
                   const D8&         embeddedArg8,
                   const D9&         embeddedArg9,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object' and
        // the specified 'const' 'method' of 'object' taking nine additional
        // trailing arguments, and the corresponding arguments' specified
        // 'embeddedArg1', 'embeddedArg2', 'embeddedArg3', 'embeddedArg4',
        // 'embeddedArg5', 'embeddedArg6', 'embeddedArg7', 'embeddedArg8', and
        // 'embeddedArg9' values.  Use the specified 'basicAllocator' to
        // supply memory.  Note that any value returned from 'method' will be
        // ignored.
};

                        // ====================
                        // class bdefi_Vfunc0M0
                        // ====================

template <class OBJ, class M>
class bdefi_Vfunc0M0 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking no additional
    // arguments.

    // DATA
    OBJ *d_obj_p;  // object
    M    d_m;      // member function pointer

    // NOT IMPLEMENTED
    bdefi_Vfunc0M0(const bdefi_Vfunc0M0<OBJ, M>&);
    bdefi_Vfunc0M0<OBJ, M>& operator=(const bdefi_Vfunc0M0<OBJ, M>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0M0();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying member function on the embedded object pointer
        // with no additional arguments.

  public:
    // CREATORS
    bdefi_Vfunc0M0(OBJ              *object,
                   M                 method,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' taking no additional arguments.  Use the specified
        // 'basicAllocator' to supply memory.  Note that any value returned
        // from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0M1
                        // ====================

template <class OBJ, class M, class D1>
class bdefi_Vfunc0M1 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking one
    // additional trailing argument, and that argument's corresponding value.

    // DATA
    OBJ                          *d_obj_p;  // object
    M                             d_m;      // member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;     // first embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0M1(const bdefi_Vfunc0M1<OBJ, M, D1>&);
    bdefi_Vfunc0M1<OBJ, M, D1>& operator=(const bdefi_Vfunc0M1<OBJ, M, D1>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0M1();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying member function on the embedded object pointer
        // with the additional trailing argument value specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0M1(OBJ              *object,
                   M                 method,
                   const D1&         embeddedArg1,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' taking one additional trailing argument, and the
        // corresponding argument's specified 'embeddedArg1' value.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0M2
                        // ====================

template <class OBJ, class M, class D1, class D2>
class bdefi_Vfunc0M2 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking two
    // additional trailing arguments, and those arguments' corresponding
    // values.

    // DATA
    OBJ                          *d_obj_p;  // object
    M                             d_m;      // member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;     // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;     // second embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0M2(const bdefi_Vfunc0M2<OBJ, M, D1, D2>&);
    bdefi_Vfunc0M2<OBJ, M, D1, D2>& operator=(
                   const bdefi_Vfunc0M2<OBJ, M, D1, D2>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0M2();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying member function on the embedded object pointer
        // with the sequence of two argument values specified at construction.

  public:
    // CREATORS
    bdefi_Vfunc0M2(OBJ              *object,
                   M                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' taking two additional trailing arguments, and the
        // corresponding arguments' specified 'embeddedArg1' and
        // 'embeddedArg2' values.  Use the specified 'basicAllocator' to
        // supply memory.  Note that any value returned from 'method' will be
        // ignored.
};
                        // ====================
                        // class bdefi_Vfunc0M3
                        // ====================

template <class OBJ, class M, class D1, class D2, class D3>
class bdefi_Vfunc0M3 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking three
    // additional trailing arguments, and those arguments' corresponding
    // values.

    // DATA
    OBJ                          *d_obj_p;  // object
    M                             d_m;      // member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;     // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;     // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;     // third embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0M3(const bdefi_Vfunc0M3<OBJ, M, D1, D2, D3>&);
    bdefi_Vfunc0M3<OBJ, M, D1, D2, D3>& operator=(
                   const bdefi_Vfunc0M3<OBJ, M, D1, D2, D3>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0M3();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying member function on the embedded object pointer
        // with the sequence of three argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0M3(OBJ              *object,
                   M                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning void, using the specified object address
        // and the specified ('const' or non-'const') 'method' of 'object'
        // taking three additional trailing arguments, and the corresponding
        // arguments' specified 'embeddedArg1', 'embeddedArg2', and
        // 'embeddedArg3' values.  Use the specified 'basicAllocator' to
        // supply memory.  Note that any value returned from 'method' will be
        // ignored.
};

                        // ====================
                        // class bdefi_Vfunc0M4
                        // ====================

template <class OBJ, class M, class D1, class D2, class D3,
          class D4>
class bdefi_Vfunc0M4 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking four
    // additional trailing arguments, and those arguments' corresponding
    // values.

    // DATA
    OBJ                          *d_obj_p;  // object
    M                             d_m;      // member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;     // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;     // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;     // third embedded argument
    bslalg::ConstructorProxy<D4>  d_d4;     // fourth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0M4(const bdefi_Vfunc0M4<OBJ, M, D1, D2, D3, D4>&);
    bdefi_Vfunc0M4<OBJ, M, D1, D2, D3, D4>& operator=(
                   const bdefi_Vfunc0M4<OBJ, M, D1, D2, D3, D4>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0M4();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of four trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0M4(OBJ              *object,
                   M                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' 'method' taking four additional trailing arguments, and
        // the corresponding arguments' specified 'embeddedArg1',
        // 'embeddedArg2', 'embeddedArg3', and 'embeddedArg4' values.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0M5
                        // ====================

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5>
class bdefi_Vfunc0M5 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking five
    // additional trailing arguments, and those arguments' corresponding
    // values.

    // DATA
    OBJ                          *d_obj_p;  // object
    M                             d_m;      // member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;     // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;     // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;     // third embedded argument
    bslalg::ConstructorProxy<D4>  d_d4;     // fourth embedded argument
    bslalg::ConstructorProxy<D5>  d_d5;     // fifth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0M5(const bdefi_Vfunc0M5<OBJ, M, D1, D2, D3, D4, D5>&);
    bdefi_Vfunc0M5<OBJ, M, D1, D2, D3, D4, D5>& operator=(
                   const bdefi_Vfunc0M5<OBJ, M, D1, D2, D3, D4, D5>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0M5();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of five trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0M5(OBJ              *object,
                   M                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' taking five additional trailing arguments, and the
        // corresponding arguments' specified 'embeddedArg1', 'embeddedArg2',
        // 'embeddedArg3', 'embeddedArg4', and 'embeddedArg5' values.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0M6
                        // ====================

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6>
class bdefi_Vfunc0M6 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking six
    // additional trailing arguments, and those arguments' corresponding
    // values.

    // DATA
    OBJ                          *d_obj_p;  // object
    M                             d_m;      // member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;     // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;     // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;     // third embedded argument
    bslalg::ConstructorProxy<D4>  d_d4;     // fourth embedded argument
    bslalg::ConstructorProxy<D5>  d_d5;     // fifth embedded argument
    bslalg::ConstructorProxy<D6>  d_d6;     // sixth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0M6(const bdefi_Vfunc0M6<OBJ, M, D1, D2, D3, D4, D5, D6>&);
    bdefi_Vfunc0M6<OBJ, M, D1, D2, D3, D4, D5, D6>& operator=(
                   const bdefi_Vfunc0M6<OBJ, M, D1, D2, D3, D4, D5, D6>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0M6();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of six trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0M6(OBJ              *object,
                   M                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   const D6&         embeddedArg6,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' taking six additional trailing arguments, and the
        // corresponding arguments' specified 'embeddedArg1', 'embeddedArg2',
        // 'embeddedArg3', 'embeddedArg4', 'embeddedArg5', and 'embeddedArg6'
        // values.  Use the specified 'basicAllocator' to supply memory.  Note
        // that any value returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0M7
                        // ====================

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7>
class bdefi_Vfunc0M7 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking seven
    // additional trailing arguments, and those arguments' corresponding
    // values.

    // DATA
    OBJ                          *d_obj_p;  // object
    M                             d_m;      // member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;     // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;     // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;     // third embedded argument
    bslalg::ConstructorProxy<D4>  d_d4;     // fourth embedded argument
    bslalg::ConstructorProxy<D5>  d_d5;     // fifth embedded argument
    bslalg::ConstructorProxy<D6>  d_d6;     // sixth embedded argument
    bslalg::ConstructorProxy<D7>  d_d7;     // seventh embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0M7(const bdefi_Vfunc0M7<OBJ, M, D1, D2, D3, D4, D5, D6, D7>&);
    bdefi_Vfunc0M7<OBJ, M, D1, D2, D3, D4, D5, D6, D7>& operator=(
                   const bdefi_Vfunc0M7<OBJ, M, D1, D2, D3, D4, D5, D6, D7>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0M7();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of seven trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0M7(OBJ              *object,
                   M                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   const D6&         embeddedArg6,
                   const D7&         embeddedArg7,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' 'method' taking seven additional trailing arguments, and
        // the corresponding arguments' specified 'embeddedArg1',
        // 'embeddedArg2', 'embeddedArg3', 'embeddedArg4', 'embeddedArg5',
        // 'embeddedArg6', and 'embeddedArg7' values.  Use the specified
        // 'basicAllocator' to supply memory.  Note that any value returned
        // from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0M8
                        // ====================

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8>
class bdefi_Vfunc0M8 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking eight
    // additional trailing arguments, and those arguments' corresponding
    // values.

    // DATA
    OBJ                          *d_obj_p;  // object
    M                             d_m;      // member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;     // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;     // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;     // third embedded argument
    bslalg::ConstructorProxy<D4>  d_d4;     // fourth embedded argument
    bslalg::ConstructorProxy<D5>  d_d5;     // fifth embedded argument
    bslalg::ConstructorProxy<D6>  d_d6;     // sixth embedded argument
    bslalg::ConstructorProxy<D7>  d_d7;     // seventh embedded argument
    bslalg::ConstructorProxy<D8>  d_d8;     // eighth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0M8(const bdefi_Vfunc0M8<OBJ, M, D1, D2, D3, D4, D5,
                                        D6, D7, D8>&);
    bdefi_Vfunc0M8<OBJ, M, D1, D2, D3, D4, D5, D6, D7, D8>& operator=(
                   const bdefi_Vfunc0M8<OBJ, M, D1, D2, D3, D4, D5,
                                        D6, D7, D8>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0M8();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of eight trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0M8(OBJ              *object,
                   M                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   const D6&         embeddedArg6,
                   const D7&         embeddedArg7,
                   const D8&         embeddedArg8,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' 'method' taking eight additional trailing arguments, and
        // the corresponding arguments' specified 'embeddedArg1',
        // 'embeddedArg2', 'embeddedArg3', 'embeddedArg4', 'embeddedArg5',
        // 'embeddedArg6', 'embeddedArg7', and 'embeddedArg8' values.  Use the
        // specified 'basicAllocator' to supply memory.  Note that any value
        // returned from 'method' will be ignored.
};

                        // ====================
                        // class bdefi_Vfunc0M9
                        // ====================

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8, class D9>
class bdefi_Vfunc0M9 : public bdefr_Vfunc0 {
    // This class defines the representation for a function object (functor),
    // characterized by a function-call operator taking no arguments and
    // returning 'void', that holds the address of an object and one of that
    // object's ('const' or non-'const') member functions taking eight
    // additional trailing arguments, and those arguments' corresponding
    // values.

    // DATA
    OBJ                          *d_obj_p;  // object
    M                             d_m;      // member function pointer
    bslalg::ConstructorProxy<D1>  d_d1;     // first embedded argument
    bslalg::ConstructorProxy<D2>  d_d2;     // second embedded argument
    bslalg::ConstructorProxy<D3>  d_d3;     // third embedded argument
    bslalg::ConstructorProxy<D4>  d_d4;     // fourth embedded argument
    bslalg::ConstructorProxy<D5>  d_d5;     // fifth embedded argument
    bslalg::ConstructorProxy<D6>  d_d6;     // sixth embedded argument
    bslalg::ConstructorProxy<D7>  d_d7;     // seventh embedded argument
    bslalg::ConstructorProxy<D8>  d_d8;     // eighth embedded argument
    bslalg::ConstructorProxy<D9>  d_d9;     // ninth embedded argument

    // NOT IMPLEMENTED
    bdefi_Vfunc0M9(const bdefi_Vfunc0M9<OBJ, M, D1, D2, D3, D4, D5,
                                        D6, D7, D8, D9>&);
    bdefi_Vfunc0M9<OBJ, M, D1, D2, D3, D4, D5, D6, D7, D8, D9>& operator=(
                   const bdefi_Vfunc0M9<OBJ, M, D1, D2, D3, D4, D5,
                                        D6, D7, D8, D9>&);

  private:
    // PRIVATE CREATORS
    ~bdefi_Vfunc0M9();
        // Destroy this functor.  Note that this destructor can be invoked only
        // indirectly, via the static 'deleteObject' method of the base class.

    // PRIVATE ACCESSORS
    void execute() const;
        // Invoke the underlying 'const' member function on the embedded object
        // with the sequence of eight trailing argument values specified at
        // construction.

  public:
    // CREATORS
    bdefi_Vfunc0M9(OBJ              *object,
                   M                 method,
                   const D1&         embeddedArg1,
                   const D2&         embeddedArg2,
                   const D3&         embeddedArg3,
                   const D4&         embeddedArg4,
                   const D5&         embeddedArg5,
                   const D6&         embeddedArg6,
                   const D7&         embeddedArg7,
                   const D8&         embeddedArg8,
                   const D9&         embeddedArg9,
                   bslma::Allocator *basicAllocator);
        // Create a representation for a function object (functor) taking no
        // arguments and returning 'void', using the specified 'object'
        // address and the specified ('const' or non-'const') 'method' of
        // 'object' 'method' taking nine additional trailing arguments, and
        // the corresponding arguments' specified 'embeddedArg1',
        // 'embeddedArg2', 'embeddedArg3', 'embeddedArg4', 'embeddedArg5',
        // 'embeddedArg6', 'embeddedArg7', 'embeddedArg8', and 'embeddedArg9'
        // values.  Use the specified 'basicAllocator' to supply memory.  Note
        // that any value returned from 'method' will be ignored.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------
                        // class bdefi_Vfunc0Null
                        // ----------------------

inline
bdefi_Vfunc0Null::bdefi_Vfunc0Null(bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
{
}

inline
void bdefi_Vfunc0Null::execute() const
{
}

                        // --------------------
                        // class bdefi_Vfunc0F0
                        // --------------------

template <class F>
inline
bdefi_Vfunc0F0<F>::bdefi_Vfunc0F0(F                 procedure,
                                  bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_f(procedure, basicAllocator)
{
}

template <class F>
inline
bdefi_Vfunc0F0<F>::~bdefi_Vfunc0F0()
{
}

template <class F>
inline
void bdefi_Vfunc0F0<F>::execute() const
{
    const F& f = d_f.object();  // for readability
    f();
}

                        // --------------------
                        // class bdefi_Vfunc0F1
                        // --------------------

template <class F, class D1>
inline
bdefi_Vfunc0F1<F, D1>::bdefi_Vfunc0F1(F                 procedure,
                                      const D1&         embeddedArg1,
                                      bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_f(procedure, basicAllocator)
, d_d1(embeddedArg1, basicAllocator)
{
}

template <class F, class D1>
inline
bdefi_Vfunc0F1<F, D1>::~bdefi_Vfunc0F1()
{
}

template <class F, class D1>
inline
void bdefi_Vfunc0F1<F, D1>::execute() const
{
    const F& f = d_f.object();  // for readability
    f(d_d1.object());
}

                        // --------------------
                        // class bdefi_Vfunc0F2
                        // --------------------

template <class F, class D1, class D2>
inline
bdefi_Vfunc0F2<F, D1, D2>::bdefi_Vfunc0F2(F                 procedure,
                                          const D1&         embeddedArg1,
                                          const D2&         embeddedArg2,
                                          bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_f(procedure, basicAllocator)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
{
}

template <class F, class D1, class D2>
inline
bdefi_Vfunc0F2<F, D1, D2>::~bdefi_Vfunc0F2()
{
}

template <class F, class D1, class D2>
inline
void bdefi_Vfunc0F2<F, D1, D2>::execute() const
{
    const F& f = d_f.object();  // for readability
    f(d_d1.object(), d_d2.object());
}

                        // --------------------
                        // class bdefi_Vfunc0F3
                        // --------------------

template <class F, class D1, class D2, class D3>
inline
bdefi_Vfunc0F3<F, D1, D2, D3>::bdefi_Vfunc0F3(F                 procedure,
                                              const D1&         embeddedArg1,
                                              const D2&         embeddedArg2,
                                              const D3&         embeddedArg3,
                                              bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_f(procedure, basicAllocator)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
{
}

template <class F, class D1, class D2, class D3>
inline
bdefi_Vfunc0F3<F, D1, D2, D3>::~bdefi_Vfunc0F3()
{
}

template <class F, class D1, class D2, class D3>
inline
void bdefi_Vfunc0F3<F, D1, D2, D3>::execute() const
{
    const F& f = d_f.object();  // for readability
    f(d_d1.object(), d_d2.object(), d_d3.object());
}

                        // --------------------
                        // class bdefi_Vfunc0F4
                        // --------------------

template <class F, class D1, class D2, class D3, class D4>
inline
bdefi_Vfunc0F4<F, D1, D2, D3, D4>::bdefi_Vfunc0F4(
                                             F                 procedure,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_f(procedure, basicAllocator)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
{
}

template <class F, class D1, class D2, class D3, class D4>
inline
bdefi_Vfunc0F4<F, D1, D2, D3, D4>::~bdefi_Vfunc0F4()
{
}

template <class F, class D1, class D2, class D3, class D4>
inline
void bdefi_Vfunc0F4<F, D1, D2, D3, D4>::execute() const
{
    const F& f = d_f.object();  // for readability
    f(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object());
}

                        // --------------------
                        // class bdefi_Vfunc0F5
                        // --------------------

template <class F, class D1, class D2, class D3, class D4,
          class D5>
inline
bdefi_Vfunc0F5<F, D1, D2, D3, D4, D5>::bdefi_Vfunc0F5(
                                             F                 procedure,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_f(procedure, basicAllocator)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
{
}

template <class F, class D1, class D2, class D3, class D4,
          class D5>
inline
bdefi_Vfunc0F5<F, D1, D2, D3, D4, D5>::~bdefi_Vfunc0F5()
{
}

template <class F, class D1, class D2, class D3, class D4, class D5>
inline
void bdefi_Vfunc0F5<F, D1, D2, D3, D4, D5>::execute() const
{
    const F& f = d_f.object();  // for readability
    f(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
      d_d5.object());
}

                        // --------------------
                        // class bdefi_Vfunc0F6
                        // --------------------

template <class F, class D1, class D2, class D3, class D4,
          class D5, class D6>
inline
bdefi_Vfunc0F6<F, D1, D2, D3, D4, D5, D6>::bdefi_Vfunc0F6(
                                             F                 procedure,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             const D6&         embeddedArg6,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_f(procedure, basicAllocator)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
, d_d6(embeddedArg6, basicAllocator)
{
}

template <class F, class D1, class D2, class D3, class D4,
          class D5, class D6>
inline
bdefi_Vfunc0F6<F, D1, D2, D3, D4, D5, D6>::~bdefi_Vfunc0F6()
{
}

template <class F, class D1, class D2, class D3,
          class D4, class D5, class D6>
inline
void bdefi_Vfunc0F6<F, D1, D2, D3, D4, D5, D6>::execute() const
{
    const F& f = d_f.object();  // for readability
    f(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
      d_d5.object(), d_d6.object());
}

                        // --------------------
                        // class bdefi_Vfunc0F7
                        // --------------------

template <class F, class D1, class D2, class D3, class D4,
          class D5, class D6, class D7>
inline
bdefi_Vfunc0F7<F, D1, D2, D3, D4, D5, D6, D7>::bdefi_Vfunc0F7(
                                             F                 procedure,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             const D6&         embeddedArg6,
                                             const D7&         embeddedArg7,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_f(procedure, basicAllocator)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
, d_d6(embeddedArg6, basicAllocator)
, d_d7(embeddedArg7, basicAllocator)
{
}

template <class F, class D1, class D2, class D3, class D4,
          class D5, class D6, class D7>
inline
bdefi_Vfunc0F7<F, D1, D2, D3, D4, D5, D6, D7>::~bdefi_Vfunc0F7()
{
}

template <class F, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7>
inline
void bdefi_Vfunc0F7<F, D1, D2, D3, D4, D5, D6, D7>::execute() const
{
    const F& f = d_f.object();  // for readability
    f(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
      d_d5.object(), d_d6.object(), d_d7.object());
}

                        // --------------------
                        // class bdefi_Vfunc0F8
                        // --------------------

template <class F, class D1, class D2, class D3, class D4,
          class D5, class D6, class D7, class D8>
inline
bdefi_Vfunc0F8<F, D1, D2, D3, D4, D5, D6, D7, D8>::bdefi_Vfunc0F8(
                                             F                 procedure,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             const D6&         embeddedArg6,
                                             const D7&         embeddedArg7,
                                             const D8&         embeddedArg8,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_f(procedure, basicAllocator)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
, d_d6(embeddedArg6, basicAllocator)
, d_d7(embeddedArg7, basicAllocator)
, d_d8(embeddedArg8, basicAllocator)
{
}

template <class F, class D1, class D2, class D3, class D4,
          class D5, class D6, class D7, class D8>
inline
bdefi_Vfunc0F8<F, D1, D2, D3, D4, D5, D6, D7, D8>::~bdefi_Vfunc0F8()
{
}

template <class F, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8>
inline
void bdefi_Vfunc0F8<F, D1, D2, D3, D4, D5, D6, D7, D8>::execute() const
{
    const F& f = d_f.object();  // for readability
    f(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
      d_d5.object(), d_d6.object(), d_d7.object(), d_d8.object());
}

                        // --------------------
                        // class bdefi_Vfunc0F9
                        // --------------------

template <class F, class D1, class D2, class D3, class D4,
          class D5, class D6, class D7, class D8, class D9>
inline
bdefi_Vfunc0F9<F, D1, D2, D3, D4, D5, D6, D7, D8, D9>::bdefi_Vfunc0F9(
                                             F                 procedure,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             const D6&         embeddedArg6,
                                             const D7&         embeddedArg7,
                                             const D8&         embeddedArg8,
                                             const D9&         embeddedArg9,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_f(procedure, basicAllocator)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
, d_d6(embeddedArg6, basicAllocator)
, d_d7(embeddedArg7, basicAllocator)
, d_d8(embeddedArg8, basicAllocator)
, d_d9(embeddedArg9, basicAllocator)
{
}

template <class F, class D1, class D2, class D3, class D4,
          class D5, class D6, class D7, class D8, class D9>
inline
bdefi_Vfunc0F9<F, D1, D2, D3, D4, D5, D6, D7, D8, D9>::~bdefi_Vfunc0F9()
{
}

template <class F, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8, class D9>
inline
void bdefi_Vfunc0F9<F, D1, D2, D3, D4, D5, D6, D7, D8, D9>::execute() const
{
    const F& f = d_f.object();  // for readability
    f(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
      d_d5.object(), d_d6.object(), d_d7.object(), d_d8.object(),
      d_d9.object());
}

                        // --------------------
                        // class bdefi_Vfunc0C0
                        // --------------------

template <class OBJ, class C>
inline
bdefi_Vfunc0C0<OBJ, C>::bdefi_Vfunc0C0(const OBJ&        object,
                                       C                 method,
                                       bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
{
}

template <class OBJ, class C>
inline
bdefi_Vfunc0C0<OBJ, C>::~bdefi_Vfunc0C0()
{
}

template <class OBJ, class C>
inline
void bdefi_Vfunc0C0<OBJ, C>::execute() const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)();
}

                        // --------------------
                        // class bdefi_Vfunc0C1
                        // --------------------

template <class OBJ, class C, class D1>
inline
bdefi_Vfunc0C1<OBJ, C, D1>::bdefi_Vfunc0C1(const OBJ&        object,
                                           C                 method,
                                           const D1&         embeddedArg1,
                                           bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
{
}

template <class OBJ, class C, class D1>
inline
bdefi_Vfunc0C1<OBJ, C, D1>::~bdefi_Vfunc0C1()
{
}

template <class OBJ, class C, class D1>
inline
void bdefi_Vfunc0C1<OBJ, C, D1>::execute() const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(d_d1.object());
}

                        // --------------------
                        // class bdefi_Vfunc0C2
                        // --------------------

template <class OBJ, class C, class D1, class D2>
inline
bdefi_Vfunc0C2<OBJ, C, D1, D2>::bdefi_Vfunc0C2(
                                             const OBJ&        object,
                                             C                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
{
}

template <class OBJ, class C, class D1, class D2>
inline
bdefi_Vfunc0C2<OBJ, C, D1, D2>::~bdefi_Vfunc0C2()
{
}

template <class OBJ, class C, class D1, class D2>
inline
void bdefi_Vfunc0C2<OBJ, C, D1, D2>::execute() const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(d_d1.object(), d_d2.object());
}

                        // --------------------
                        // class bdefi_Vfunc0C3
                        // --------------------

template <class OBJ, class C, class D1, class D2, class D3>
inline
bdefi_Vfunc0C3<OBJ, C, D1, D2, D3>::bdefi_Vfunc0C3(
                                             const OBJ&        object,
                                             C                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
{
}

template <class OBJ, class C, class D1, class D2, class D3>
inline
bdefi_Vfunc0C3<OBJ, C, D1, D2, D3>::~bdefi_Vfunc0C3()
{
}

template <class OBJ, class C, class D1, class D2, class D3>
inline
void bdefi_Vfunc0C3<OBJ, C, D1, D2, D3>::execute() const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(d_d1.object(), d_d2.object(), d_d3.object());
}

                        // --------------------
                        // class bdefi_Vfunc0C4
                        // --------------------

template <class OBJ, class C, class D1, class D2, class D3,
          class D4>
inline
bdefi_Vfunc0C4<OBJ, C, D1, D2, D3, D4>::bdefi_Vfunc0C4(
                                             const OBJ&        object,
                                             C                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
{
}

template <class OBJ, class C, class D1, class D2, class D3,
          class D4>
inline
bdefi_Vfunc0C4<OBJ, C, D1, D2, D3, D4>::~bdefi_Vfunc0C4()
{
}

template <class OBJ, class C, class D1, class D2, class D3, class D4>
inline
void bdefi_Vfunc0C4<OBJ, C, D1, D2, D3, D4>::execute() const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object());
}

                        // --------------------
                        // class bdefi_Vfunc0C5
                        // --------------------

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5>
inline
bdefi_Vfunc0C5<OBJ, C, D1, D2, D3, D4, D5>::bdefi_Vfunc0C5(
                                             const OBJ&        object,
                                             C                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
{
}

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5>
inline
bdefi_Vfunc0C5<OBJ, C, D1, D2, D3, D4, D5>::~bdefi_Vfunc0C5()
{
}

template <class OBJ, class C, class D1, class D2, class D3, class D4, class D5>
inline
void bdefi_Vfunc0C5<OBJ, C, D1, D2, D3, D4, D5>::execute() const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
               d_d5.object());
}

                        // --------------------
                        // class bdefi_Vfunc0C6
                        // --------------------

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6>
inline
bdefi_Vfunc0C6<OBJ, C, D1, D2, D3, D4, D5, D6>::bdefi_Vfunc0C6(
                                             const OBJ&        object,
                                             C                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             const D6&         embeddedArg6,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
, d_d6(embeddedArg6, basicAllocator)
{
}

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6>
inline
bdefi_Vfunc0C6<OBJ, C, D1, D2, D3, D4, D5, D6>::~bdefi_Vfunc0C6()
{
}

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6>
inline
void bdefi_Vfunc0C6<OBJ, C, D1, D2, D3, D4, D5, D6>::execute() const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
               d_d5.object(), d_d6.object());
}

                        // --------------------
                        // class bdefi_Vfunc0C7
                        // --------------------

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7>
inline
bdefi_Vfunc0C7<OBJ, C, D1, D2, D3, D4, D5, D6, D7>::bdefi_Vfunc0C7(
                                             const OBJ&        object,
                                             C                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             const D6&         embeddedArg6,
                                             const D7&         embeddedArg7,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
, d_d6(embeddedArg6, basicAllocator)
, d_d7(embeddedArg7, basicAllocator)
{
}

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7>
inline
bdefi_Vfunc0C7<OBJ, C, D1, D2, D3, D4, D5, D6, D7>::~bdefi_Vfunc0C7()
{
}

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7>
inline
void bdefi_Vfunc0C7<OBJ, C, D1, D2, D3, D4, D5, D6, D7>::execute() const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
               d_d5.object(), d_d6.object(),
               d_d7.object());
}

                        // --------------------
                        // class bdefi_Vfunc0C8
                        // --------------------

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8>
inline
bdefi_Vfunc0C8<OBJ, C, D1, D2, D3, D4, D5, D6, D7, D8>::bdefi_Vfunc0C8(
                                             const OBJ&        object,
                                             C                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             const D6&         embeddedArg6,
                                             const D7&         embeddedArg7,
                                             const D8&         embeddedArg8,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
, d_d6(embeddedArg6, basicAllocator)
, d_d7(embeddedArg7, basicAllocator)
, d_d8(embeddedArg8, basicAllocator)
{
}

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8>
inline
bdefi_Vfunc0C8<OBJ, C, D1, D2, D3, D4, D5, D6, D7, D8>::~bdefi_Vfunc0C8()
{
}

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8>
inline
void bdefi_Vfunc0C8<OBJ, C, D1, D2, D3, D4, D5, D6, D7, D8>::execute() const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
               d_d5.object(), d_d6.object(), d_d7.object(), d_d8.object());
}

                        // --------------------
                        // class bdefi_Vfunc0C9
                        // --------------------

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8, class D9>
inline
bdefi_Vfunc0C9<OBJ, C, D1, D2, D3, D4, D5, D6, D7, D8, D9>::bdefi_Vfunc0C9(
                                             const OBJ&        object,
                                             C                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             const D6&         embeddedArg6,
                                             const D7&         embeddedArg7,
                                             const D8&         embeddedArg8,
                                             const D9&         embeddedArg9,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj(object, basicAllocator)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
, d_d6(embeddedArg6, basicAllocator)
, d_d7(embeddedArg7, basicAllocator)
, d_d8(embeddedArg8, basicAllocator)
, d_d9(embeddedArg9, basicAllocator)
{
}

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8, class D9>
inline
bdefi_Vfunc0C9<OBJ, C, D1, D2, D3, D4, D5, D6, D7, D8, D9>::~bdefi_Vfunc0C9()
{
}

template <class OBJ, class C, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8, class D9>
inline
void
bdefi_Vfunc0C9<OBJ, C, D1, D2, D3, D4, D5, D6, D7, D8, D9>::execute() const
{
    const OBJ& obj = d_obj.object();  // for readability
    (obj.*d_m)(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
               d_d5.object(), d_d6.object(), d_d7.object(), d_d8.object(),
               d_d9.object());
}

                        // --------------------
                        // class bdefi_Vfunc0M0
                        // --------------------

template <class OBJ, class M>
inline
bdefi_Vfunc0M0<OBJ, M>::bdefi_Vfunc0M0(OBJ              *object,
                                       M                 method,
                                       bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj_p(object)
, d_m(method)
{
}

template <class OBJ, class M>
inline
bdefi_Vfunc0M0<OBJ, M>::~bdefi_Vfunc0M0()
{
}

template <class OBJ, class M>
inline
void bdefi_Vfunc0M0<OBJ, M>::execute() const
{
    (d_obj_p->*d_m)();
}

                        // --------------------
                        // class bdefi_Vfunc0M1
                        // --------------------

template <class OBJ, class M, class D1>
inline
bdefi_Vfunc0M1<OBJ, M, D1>::bdefi_Vfunc0M1(OBJ              *object,
                                           M                 method,
                                           const D1&         embeddedArg1,
                                           bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
{
}

template <class OBJ, class M, class D1>
inline
bdefi_Vfunc0M1<OBJ, M, D1>::~bdefi_Vfunc0M1()
{
}

template <class OBJ, class M, class D1>
inline
void bdefi_Vfunc0M1<OBJ, M, D1>::execute() const
{
    (d_obj_p->*d_m)(d_d1.object());
}

                        // --------------------
                        // class bdefi_Vfunc0M2
                        // --------------------

template <class OBJ, class M, class D1, class D2>
inline
bdefi_Vfunc0M2<OBJ, M, D1, D2>::bdefi_Vfunc0M2(
                                             OBJ              *object,
                                             M                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
{
}

template <class OBJ, class M, class D1, class D2>
inline
bdefi_Vfunc0M2<OBJ, M, D1, D2>::~bdefi_Vfunc0M2()
{
}

template <class OBJ, class M, class D1, class D2>
inline
void bdefi_Vfunc0M2<OBJ, M, D1, D2>::execute() const
{
    (d_obj_p->*d_m)(d_d1.object(), d_d2.object());
}

                        // --------------------
                        // class bdefi_Vfunc0M3
                        // --------------------

template <class OBJ, class M, class D1, class D2, class D3>
inline
bdefi_Vfunc0M3<OBJ, M, D1, D2, D3>::bdefi_Vfunc0M3(
                                             OBJ              *object,
                                             M                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
{
}

template <class OBJ, class M, class D1, class D2, class D3>
inline
bdefi_Vfunc0M3<OBJ, M, D1, D2, D3>::~bdefi_Vfunc0M3()
{
}

template <class OBJ, class M, class D1, class D2, class D3>
inline
void bdefi_Vfunc0M3<OBJ, M, D1, D2, D3>::execute() const
{
    (d_obj_p->*d_m)(d_d1.object(), d_d2.object(), d_d3.object());
}

                        // --------------------
                        // class bdefi_Vfunc0M4
                        // --------------------

template <class OBJ, class M, class D1, class D2, class D3,
          class D4>
inline
bdefi_Vfunc0M4<OBJ, M, D1, D2, D3, D4>::bdefi_Vfunc0M4(
                                             OBJ              *object,
                                             M                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
{
}

template <class OBJ, class M, class D1, class D2, class D3,
          class D4>
inline
bdefi_Vfunc0M4<OBJ, M, D1, D2, D3, D4>::~bdefi_Vfunc0M4()
{
}

template <class OBJ, class M, class D1, class D2, class D3, class D4>
inline
void bdefi_Vfunc0M4<OBJ, M, D1, D2, D3, D4>::execute() const
{
    (d_obj_p->*d_m)(d_d1.object(), d_d2.object(), d_d3.object(),
                    d_d4.object());
}

                        // --------------------
                        // class bdefi_Vfunc0M5
                        // --------------------

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5>
inline
bdefi_Vfunc0M5<OBJ, M, D1, D2, D3, D4, D5>::bdefi_Vfunc0M5(
                                             OBJ              *object,
                                             M                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
{
}

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5>
inline
bdefi_Vfunc0M5<OBJ, M, D1, D2, D3, D4, D5>::~bdefi_Vfunc0M5()
{
}

template <class OBJ, class M, class D1, class D2, class D3, class D4, class D5>
inline
void bdefi_Vfunc0M5<OBJ, M, D1, D2, D3, D4, D5>::execute() const
{
    (d_obj_p->*d_m)(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
                    d_d5.object());
}

                        // --------------------
                        // class bdefi_Vfunc0M6
                        // --------------------

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6>
inline
bdefi_Vfunc0M6<OBJ, M, D1, D2, D3, D4, D5, D6>::bdefi_Vfunc0M6(
                                             OBJ              *object,
                                             M                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             const D6&         embeddedArg6,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
, d_d6(embeddedArg6, basicAllocator)
{
}

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6>
inline
bdefi_Vfunc0M6<OBJ, M, D1, D2, D3, D4, D5, D6>::~bdefi_Vfunc0M6()
{
}

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6>
inline
void bdefi_Vfunc0M6<OBJ, M, D1, D2, D3, D4, D5, D6>::execute() const
{
    (d_obj_p->*d_m)(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
                    d_d5.object(), d_d6.object());
}

                        // --------------------
                        // class bdefi_Vfunc0M7
                        // --------------------

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7>
inline
bdefi_Vfunc0M7<OBJ, M, D1, D2, D3, D4, D5, D6, D7>::bdefi_Vfunc0M7(
                                             OBJ              *object,
                                             M                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             const D6&         embeddedArg6,
                                             const D7&         embeddedArg7,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
, d_d6(embeddedArg6, basicAllocator)
, d_d7(embeddedArg7, basicAllocator)
{
}

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7>
inline
bdefi_Vfunc0M7<OBJ, M, D1, D2, D3, D4, D5, D6, D7>::~bdefi_Vfunc0M7()
{
}

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7>
inline
void bdefi_Vfunc0M7<OBJ, M, D1, D2, D3, D4, D5, D6, D7>::execute() const
{
    (d_obj_p->*d_m)(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
                    d_d5.object(), d_d6.object(), d_d7.object());
}

                        // --------------------
                        // class bdefi_Vfunc0M8
                        // --------------------

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8>
inline
bdefi_Vfunc0M8<OBJ, M, D1, D2, D3, D4, D5, D6, D7, D8>::bdefi_Vfunc0M8(
                                             OBJ              *object,
                                             M                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             const D6&         embeddedArg6,
                                             const D7&         embeddedArg7,
                                             const D8&         embeddedArg8,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
, d_d6(embeddedArg6, basicAllocator)
, d_d7(embeddedArg7, basicAllocator)
, d_d8(embeddedArg8, basicAllocator)
{
}

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8>
inline
bdefi_Vfunc0M8<OBJ, M, D1, D2, D3, D4, D5, D6, D7, D8>::~bdefi_Vfunc0M8()
{
}

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8>
inline
void bdefi_Vfunc0M8<OBJ, M, D1, D2, D3, D4, D5, D6, D7, D8>::execute() const
{
    (d_obj_p->*d_m)(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
                   d_d5.object(), d_d6.object(), d_d7.object(), d_d8.object());
}

                        // --------------------
                        // class bdefi_Vfunc0M9
                        // --------------------

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8, class D9>
inline
bdefi_Vfunc0M9<OBJ, M, D1, D2, D3, D4, D5, D6, D7, D8, D9>::bdefi_Vfunc0M9(
                                             OBJ              *object,
                                             M                 method,
                                             const D1&         embeddedArg1,
                                             const D2&         embeddedArg2,
                                             const D3&         embeddedArg3,
                                             const D4&         embeddedArg4,
                                             const D5&         embeddedArg5,
                                             const D6&         embeddedArg6,
                                             const D7&         embeddedArg7,
                                             const D8&         embeddedArg8,
                                             const D9&         embeddedArg9,
                                             bslma::Allocator *basicAllocator)
: bdefr_Vfunc0(basicAllocator)
, d_obj_p(object)
, d_m(method)
, d_d1(embeddedArg1, basicAllocator)
, d_d2(embeddedArg2, basicAllocator)
, d_d3(embeddedArg3, basicAllocator)
, d_d4(embeddedArg4, basicAllocator)
, d_d5(embeddedArg5, basicAllocator)
, d_d6(embeddedArg6, basicAllocator)
, d_d7(embeddedArg7, basicAllocator)
, d_d8(embeddedArg8, basicAllocator)
, d_d9(embeddedArg9, basicAllocator)
{
}

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8, class D9>
inline
bdefi_Vfunc0M9<OBJ, M, D1, D2, D3, D4, D5, D6, D7, D8, D9>::~bdefi_Vfunc0M9()
{
}

template <class OBJ, class M, class D1, class D2, class D3,
          class D4, class D5, class D6, class D7, class D8, class D9>
inline
void
bdefi_Vfunc0M9<OBJ, M, D1, D2, D3, D4, D5, D6, D7, D8, D9>::execute() const
{
    (d_obj_p->*d_m)(d_d1.object(), d_d2.object(), d_d3.object(), d_d4.object(),
                    d_d5.object(), d_d6.object(), d_d7.object(), d_d8.object(),
                    d_d9.object());
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
