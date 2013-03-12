// bael_attributecontext.h                                            -*-C++-*-
#ifndef INCLUDED_BAEL_ATTRIBUTECONTEXT
#define INCLUDED_BAEL_ATTRIBUTECONTEXT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a container for storing attributes and caching results.
//
//@CLASSES:
//         bael_AttributeContext: thread-local list of attribute containers
//  bael_AttributeContextProctor: proctor for deleting and attribute context
//
//@SEE_ALSO: bael_attributeset
//
//@AUTHOR: Gang Chen (gchen20), Mike Verschell (hverschell)
//
//@DESCRIPTION: This component provides a mechanism to store attributes in
// thread local storage, and to evaluate rules associated with a given
// category using those stored attributes.  The 'bael_AttributeContext' class
// is statically initialized (using the 'initialize()' method) with a
// 'bael_CategoryManager' object containing a 'bael_RuleSet' the represents
// the currently installed (global) logging rules for the process.  Clients
// can obtain the context for the current thread by calling the static
// 'getContext()' method.  Attributes can be added or removed from an
// attribute context using the 'addAttributes()' and removeAttributes()'
// methods.  Finally the 'bael_AttributeContext' provides methods (used
// primarily by other components in the 'bael' package') to determine the
// affect of the current logging rules on the logging thresholds of a
// category.  The 'hasRelevantActiveRules()' method, returns 'true' if there
// is at least on relevant and active rule (in the global set of rules) that
// might modify the logging thresholds of the supplied 'category'.  A rule is
// "relevant" if the rule's pattern matches the category's name, and a rule is
// "active" if all the predicates defined for that rule are satisfied by the
// current thread's attributes (i.e., 'bael_Rule::evaluate()' returns 'true'
// for the collection of attributes maintained for the current thread by the
// thread's 'bael_AttributeContext' object).  The 'determineThresholdLevels()'
// method returns the logging threshold levels for a category, factoring in
// any active rules that apply to the category that might override the
// category's thresholds.  Note that, in practice, 'initialize()' is called by
// the singleton 'bael_LoggerManager' object when the logger manager is
// initialized.
//
///Usage
///-----
// This section illustrates the use of 'bael_AttributeContext'.
//
///Managing Attributes
///- - - - - - - - - -
// First we will define a thread function that will create and install two
// attributes.  Note that we will use the 'AttributeSet' implementation of the
// 'bael_AttributeContainer' protocol defined in the component documentation
// for 'bael_attributecontainer'; the 'bael' package provides a similar class
// in the 'bael_defaultattributecontainer' component.
//..
//  extern "C" void *thread1(void*)
//  {
//..
// Inside this thread function, we create an attribute set to hold our
// attribute values; then we create two 'bael_Attribute' objects and add them
// to that set:
//..
//      AttributeSet attributes;
//      bael_Attribute a1("uuid", 4044457);
//      bael_Attribute a2("name", "Gang Chen");
//      attributes.insert(a1);
//      attributes.insert(a2);
//..
// Next we obtain a reference to the current threads attribute context using
// the 'getContext()' class method:
//..
//      bael_AttributeContext *context = bael_AttributeContext::getContext();
//      assert(context);
//      assert(context == bael_AttributeContext::lookupContext());
//..
// We can add our attribute container, 'attributes', to the current context
// using the 'addAttributes()' method.  We store the returned iterator so that
// we can remove 'attributes'  before it goes out of scope and is destroyed:
//..
//      bael_AttributeContext::iterator it =
//                                         context->addAttributes(&attributes);
//      assert(context->hasAttribute(a1));
//      assert(context->hasAttribute(a2));
//..
// We then call the 'removeAttributes()' method to remove the attributes from
// the attribute context:
//..
//      context->removeAttributes(it);
//      assert(false == context->hasAttribute(a1));
//      assert(false == context->hasAttribute(a2));
//..
// This completes the first thread function:
//..
//      return 0;
//  }
//..
// The second thread function will simply verify that there is no currently
// available attribute context.  Note that attribute contexts are created and
// managed by individual threads using thread-specific storage, and that
// attribute contexts created by one thread are not visible in any other
// threads:
//..
//  extern "C" void *thread2(void*)
//  {
//      assert(0 == bael_AttributeContext::lookupContext());
//      return 0;
//  }
//..
///Calling 'hasRelevantActiveRules' and 'determineThresholdLevels'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to call the 'hasRelevantActiveRules()'
// and 'determineThresholdLevels()' methods.  These methods are used
// (primarily by other components in the 'bael' package) to determine the
// affect of the current logging rules on the logging thresholds of a
// category.  Note that a rule is "relevant" if the rule's pattern matches
// the category's name, and a rule is "active" if 'bael_Rule::evaluate()'
// returns 'true' for the collection of attributes maintained for the current
// thread by the thread's 'bael_AttributeContext' object.
//
// We start by creating a 'bael_CategoryManager' and use it to initialize the
// static data members of 'bael_AttributeContext'.  Note that, in practice,
// this initialization *should* *not* be performed by clients of the 'bael'
// package':  The 'bael_LoggerManager' singleton will initialize the
// 'bael_AttributeContext' with the when the singleton is created.
//..
//  bael_CategoryManager categoryManager;
//  bael_AttributeContext::initialize(&categoryManager);  // this is normally
//                                                        // performed by the
//                                                        // bael_LoggerManager
//..
// Next we add a category to the category manager.  Each created category
// has a name and the logging threshold levels for that category.  The logging
// threshold levels indicate the minimum severity for logged message that
// will trigger the relevant action.  The four thresholds are the "record
// level" (messages logged with a higher severity than this threshold should
// be added to the current logger's record buffer), the "passthrough level"
// (messages logged with a severity higher than this threshold should be
// published immediately), the "trigger level" (messages logged with a higher
// severity than this threshold should trigger the publication of the entire
// contents of the current logger's record buffer), and the "trigger-all
// level" (messages logged with a higher severity than this threshold should
// trigger the publication of every logger's record buffer), respectively.
// Note that, clients are generally most interested in the "passthrough"
// threshold level.  Also note that a higher number indicates a lower
// severity.
//..
//  const bael_Category *cat1 =
//             categoryManager.addCategory("MyCategory", 128, 96, 64, 32);
//..
// Next we obtain the context for the current thread.
//..
//  bael_AttributeContext *context = bael_AttributeContext::getContext();
//..
// We call 'hasRelevantActiveRules()' on 'cat1'.  This will be 'false'
// because we haven't supplied any rules.
//..
// assert(false == context->hasRelevantActiveRules(cat1));
//..
// We call 'determineThresholdLevels()' on 'cat1'.  This will simply return
// the logging threshold levels we defined for 'cat1' when it was created
// because no rules have been defined that might modify those thresholds:
//..
//  bael_ThresholdAggregate cat1ThresholdLevels(0, 0, 0, 0);
//  context->determineThresholdLevels(&cat1ThresholdLevels, cat1);
//  assert(128 == cat1ThresholdLevels.recordLevel());
//  assert(96  == cat1ThresholdLevels.passLevel());
//  assert(64  == cat1ThresholdLevels.triggerLevel());
//  assert(32  == cat1ThresholdLevels.triggerAllLevel());
//..
// Next we create a rule that will apply to those categories whose names match
// the pattern "My*", where '*' is a wild-card value.  The rule defines a
// set of thresholds levels that may override the threshold levels of those
// categories whose name matches the rule's pattern:
//..
//  bael_Rule myRule("My*", 120, 110, 70, 40);
//  categoryManager.addRule(myRule);
//..
// Now we call 'hasRelevantActiveRules()' again for 'cat1', but this time the
// method returns 'true' because the rule we just added is both "relevant" to
// 'cat1', and "active".  'myRule' is "relevant" to 'cat1', because the name of
// 'cat1' ("MyCategory") matches the pattern for 'myRule' ("My*") (i.e.,
// 'myRule' applies to 'cat1').  'myRule' is also "active" because all the
// predicates defined for the rule are satisfied by the current thread (in
// this case the rule has no predicates, so the rule is always "active").
// Note that, we will discuss the meaning of "active" and the use of
// predicates later in this example.
//..
//  assert(true == context->hasRelevantActiveRules(cat1));
//..
// Next we call 'determineThresholdLevels()' for 'cat1'.  The
// 'determineThresholdLevels()' method compares the threshold levels defined
// for category with those of any active rules that apply to that category,
// and determines the minimum severity (i.e., the maximum numerical value),
// for each respective threshold amongst those values.
//..
//  bael_ThresholdAggregate thresholdLevels(0, 0, 0, 0);
//  context->determineThresholdLevels(&thresholdLevels, cat1);
//  assert(128 == thresholdLevels.recordLevel());
//  assert(110 == thresholdLevels.passLevel());
//  assert(70  == thresholdLevels.triggerLevel());
//  assert(40  == thresholdLevels.triggerAllLevel());
//..
// In this case the "passthrough", "trigger", and "trigger-all" threshold
// levels defined by 'myRule' (110, 70, and 40) are greater (i.e., define a
// lower severity) than those respective values defined for 'cat1' (96, 64,
// and 32), so those values override the values defined for 'cat1'.  On the
// other hand the "record" threshold level for 'cat1' (128) is greater than
// the value defined by 'myRule' (120), so the threshold level for defined for
// 'cat1' is  returned.  In effect, 'myRule' has lowered the severity at which
// messages logged in the "MyCategory" category will be published immediately,
// trigger the publication of the current logger's record buffer, and trigger
// the publication of every logger's record buffer.
//
// Next we modify 'myRule', adding a predicate indicating that the rule should
// only apply if the attribute context for the current thread contains the
// attribute '("uuid", 3938908)':
//..
//  categoryManager.removeRule(myRule);
//  bael_Predicate predicate("uuid", 3938908);
//  myRule.addPredicate(predicate);
//  categorymanager.addRule(myRule);
//..
// When we again call 'hasRelevantActiveRules()' for 'cat1', it now returns
// 'false'.  The rule, 'myRule', still applies to 'cat1' (i.e., it is still
// "relevant" to 'cat1') but the predicates defined by 'myRule' are no longer
// satisfied by the current thread, i.e., the current threads attribute
// context does not contain an attribute matching '("uuid", 3938908)'.
//..
//  assert(false == context->hasRelevantActiveRules(cat1));
//..
// Next we call 'determineThresholdLevels()' on 'cat1' and find that it
// returns the threshold levels we defined for 'cat1' when we created it:
//..
//  context->determineThresholdLevels(&thresholdLevels, cat1);
//  assert(thresholdLevels  == cat1ThresholdLevels);
//..
// Finally, we add an attribute to the current thread's attribute
// context (as we did in the first example, "Managing Attributes").  Note that
// we keep an iterator referring to the added attributes so that we can remove
// them before 'attributest' goes out of scope and is destroyed.  Also note
// that the class 'AttributeSet' is defined in the component documentation for
// 'bael_attributecontainer'.
//..
//  AttributeSet attributes;
//  attributes.insert(bael_Attribute("uuid", 3938908));
//  bael_AttributeContext::iterator it = context->addAttributes(&attributes);
//..
// The following call to 'hasRelevantActive()' will return 'true' for 'cat1'
// because there is at least one rule, 'myRule', that is both "relevant"
// (i.e., its pattern matches the category name of 'cat1') and "active" (i.e.,
// all of the predicates defined for 'myRule' are satisfied by the attributes
// held by this thread's attribute context).
//..
//  assert(true == context->hasRelevantActiveRules(cat1));
//..
// Now when we call 'determineThresholdLevels()', it will again return the
// maximum threshold level from 'cat1' an 'myRule'.
//..
//  context->determineThresholdLevels(&thresholdLevels, cat1);
//  assert(128 == thresholdLevels.recordLevel());
//  assert(110 == thresholdLevels.passLevel());
//  assert(70  == thresholdLevels.triggerLevel());
//  assert(40  == thresholdLevels.triggerAllLevel());
//..
// We must be careful to remove 'attributes' from the attribute context
// before it goes out of scope and is destroyed.  Note that the 'bael'
// package supplies a component, 'bael_scopedattributes' for adding, and
// automatically removing, attributes from the current thread's attribute
// context.
//..
// context->removeAttributes(it);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_ATTRIBUTECONTAINERLIST
#include <bael_attributecontainerlist.h>
#endif

#ifndef INCLUDED_BAEL_RULESET
#include <bael_ruleset.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bael_Category;
class bael_CategoryManager;

             // ===============================================
             // class bael_AttributeContext_RuleEvaluationCache
             // ===============================================

class bael_AttributeContext_RuleEvaluationCache {
    // This is an implementation type of 'bael_AttributeContext' and should
    // not be used by clients of this package.  A rule evaluation cache is a
    // mechanism for evaluating and caching whether a rule is active.  A rule
    // is considered active if all of its predicates are satisfied by the
    // collection of attributes held in a 'bael_AttributeContainerList' object
    // (i.e., 'bael_Rule::evaluate()' returns 'true' for the
    // 'bael_AttributeContainerList' object).  The rules this cache evaluates
    // are contained in a 'bael_RuleSet' object.  The 'bael_RuleSet::MaskType'
    // type is a bit-mask for a rule set, where each bit is a boolean value
    // associated with the rule at the corresponding index in a rule set.
    // Clients can determine, using the 'isDataAvailable()' method, if a
    // particular set of rules (described using a bit mask) have already been
    // evaluated; they can access the current cache of rule evaluations using
    // the 'knownActiveRules()' method; finally they can update the cache of
    // rule evaluations using the 'update()' method.  Note that clients should
    // use the 'isDataAvailable()' method prior to using 'knownActiveRules()'
    // in order to ensure the relevant rules have been evaluated and
    // that those evaluations are up-to-date.

    bael_RuleSet::MaskType d_evalMask;        // set of bits, each of which
                                              // indicates whether the
                                              // corresponding rule has been
                                              // evaluated and cached in
                                              // 'd_resultMask'.

    bael_RuleSet::MaskType d_resultMask;      // set of bits, each of which
                                              // caches the result of
                                              // previous evaluation of the
                                              // corresponding rule, or is 0 if
                                              // the rule has not been
                                              // evaluated

    int                    d_sequenceNumber;  // sequential index provided
                                              // on the most recent call to
                                              // 'update()'; if the provided
                                              // sequence number changes it
                                              // indicates the cache is out of
                                              // date

    // NOT IMPLEMENTED
    bael_AttributeContext_RuleEvaluationCache(
                             const bael_AttributeContext_RuleEvaluationCache&);
    bael_AttributeContext_RuleEvaluationCache& operator=(
                             const bael_AttributeContext_RuleEvaluationCache&);
  public:

    // CREATORS
    bael_AttributeContext_RuleEvaluationCache();
        // Create an empty rule evaluation cache.

    // ~bael_AttributeContext_RuleEvaluationCache();
        // Destroy this rule evaluation cache.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    void clear();
        // Clear any currently cached rule evaluation data.

    bael_RuleSet::MaskType update(
                           int                                sequenceNumber,
                           bael_RuleSet::MaskType             relevantRuleMask,
                           const bael_RuleSet&                rules,
                           const bael_AttributeContainerList& attributes);
        // Update, for the specified 'sequenceNumber', the cache for those
        // rules indicated by the specified 'relevantRuleMask' bit-mask in the
        // specified set of 'rules', by evaluating those rules for the
        // specified 'attributes'; return the bit mask indicating those
        // rules that are known to be active.  If a bit in the returned
        // bit-mask value is set to 1,  the rule at the corresponding index in
        // 'rules' is "active", however if a bit is set to 0, the
        // corresponding rule is either not active *or* has not been
        // evaluated.  This operation does, however, guarantee that all the
        // rules indicated by the 'relevantRuleMask' *will* be evaluated.  A
        // particular rule is considered "active" if all of it's predicates
        // are satisfied by 'attributes' (i.e., if 'bael_Rule::evaluate()'
        // returns 'true' for 'attributes').  The behavior is undefined unless
        // 'rules' is valid and unmodified during this operation (i.e., any
        // lock associated with 'rules' must be locked during this operation)
        // and the supplied 'sequenceNumber' value is associated with only one
        // 'rules' value.

    // ACCESSORS
    bool isDataAvailable(int                    sequenceNumber,
                         bael_RuleSet::MaskType relevantRulesMask) const;
        // Return true if this cache contains update-to-date cached rule
        // evaluations with the specified 'sequenceNumber' for the set of
        // rules indicated by the specified 'relevantRuleMask' bit mask, and
        // 'false' otherwise.

    bael_RuleSet::MaskType knownActiveRules() const;
        // Return a bit mask indicating those rules, from the set of rules
        // provided in the last call to 'update()', that are known to be
        // active (as of that last call to 'update()').  If a bit in the
        // returned value is set to 1, the rule at the corresponding index is
        // active, however if a bit is set to 0, the corresponding rule is
        // either not active *or* has not been evaluated.  Note that clients
        // should call 'isDataAvailable()' to test if this cache contains
        // up-to-date evaluated rule information for the rules in which they
        // are interested before using the result of this method.

    bael_RuleSet::MaskType evaluatedRules() const;
        // Return a bit mask indicating those rules, from the set of rules
        // provided in the last call to 'update()'  that have been evaluated
        // (as of that last call to 'update()').  If a bit in the returned
        // value is set to 1, the rule at the corresponding index has been
        // evaluated; if a bit is 0, the corresponding rule has not been
        // evaluated.  Note that the sequence number for the rules may be out
        // of date, and in general clients should call 'isDataAvailable()' to
        // test if this cache contains up-to-date rule information.

    int sequenceNumber() const;
        // Return the non-negative sequence number provided on the last
        // invocation of 'update()' or -1 if evaluate has not been called.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(
                     bsl::ostream&                                    stream,
                     const bael_AttributeContext_RuleEvaluationCache& cache);
        // Write a description of the data members of the specified 'cache'
        // to the specified 'stream' in some single-line human readable
        // format, and return the modifiable 'stream'.

                        // ===========================
                        // class bael_AttributeContext
                        // ===========================

class bael_AttributeContext {
    // This class provides a mechanism for associating attributes with the
    // current thread, and evaluating the logging rules associated with a
    // category using those stored attributes.  The 'bael_AttributeContext'
    // class is statically initialized (using the 'initialize()' method) with
    // a 'bael_CategoryManager' object containing a 'bael_RuleSet' the
    // represents the currently installed logging rules for the process.
    // Clients can obtain the context for the current thread by calling this
    // class's static 'getContext()' method.  The 'addAttributes()' and
    // 'removeAttributes()' methods are used to add and remove collections of
    // attributes from the (thread-local) context object.  Finally the
    // 'bael_AttributeContext' provides methods (used primarily by other
    // components in the 'bael' package') to determine the affect of the
    // current logging rules on the logging thresholds of a category.  The
    // 'hasRelevantActiveRules()' method, returns 'true' if there are any
    // relevant and active rules that might modify the logging thresholds of
    // the supplied category.  A rule is "relevant" if the rule's pattern
    // matches the category's name, and a rule is "active" if all the
    // predicates defined for the rule are satisfied by the current thread's
    // attributes (i.e., 'bael_Rule::evaluate()' returns 'true' for the
    // collection of attributes maintained for the current thread by the
    // thread's 'bael_AttributeContext' object).  The
    // 'determineThresholdLevels()' method returns the logging threshold
    // levels for a category, factoring in any active rules that apply to the
    // category that might override the category's thresholds.  The behavior
    // for the 'hasRelevantActiveRules()' and 'determineThresholdLevels()'
    // methods is undefined unless 'initialize()' has already been called.
    // Note that, in practice, 'initialize()' is called by the singleton
    // 'bael_LoggerManager' object when the logger manager is initialized.

    // PRIVATE TYPES
    typedef bael_AttributeContext_RuleEvaluationCache RuleEvaluationCache;

    // CLASS DATA
    static bael_CategoryManager *s_categoryManager_p;  // holds the rule set,
                                                       // rule sequence number,
                                                       // and rule set mutex

    static bslma::Allocator     *s_globalAllocator_p;  // an allocator for
                                                       // thread local context
                                                       // objects

    // DATA
    bael_AttributeContainerList  d_containerList;      // list of attribute
                                                       // containers

    mutable RuleEvaluationCache  d_ruleCache_p;        // cache of rule
                                                       // evaluations

    bslma::Allocator            *d_allocator_p;        // allocator used to
                                                       // create this object
                                                       // (held but not owned)

    // FRIENDS
    friend class bael_AttributeContextProctor;

    // NOT IMPLEMENTED
    bael_AttributeContext(const bael_AttributeContext& );
    bael_AttributeContext& operator=(const bael_AttributeContext& );

    // PRIVATE CLASS METHODS
    static const bcemt_ThreadUtil::Key& contextKey();
        // Return a reference to the non-modifiable singleton key for the
        // thread local storage in which the 'bael_AttributeContext' objects
        // are stored.  This method will create the key on the first
        // invocation; all subsequent invocations will return the previously
        // created key.  Note that it is more efficient to cache the return
        // value of this method, than to invoke it repeatedly.

    static void removeContext(void *arg);
        // Destroy the 'bael_AttributeContext' object pointed to by the
        // specified 'arg'.  Note that this function is designed to be called
        // by the thread-specific storage facility when a thread exits (and
        // thus should not be called in application code directly).

    // PRIVATE CREATORS
    bael_AttributeContext(bslma::Allocator *globalAllocator = 0);
        // Create a 'bael_AttributeContext' object initially having no
        // attributes.  Optionally specify a 'globalAllocator' used to supply
        // memory.  If 'globalAllocator' is 0, the currently installed global
        // allocator will be used.  Note that the static method 'getContext()'
        // should be used to obtain the address of the attribute context for
        // the current thread.

    ~bael_AttributeContext();
        // Destroy this object.

  public:

    // PUBLIC TYPES
    typedef bael_AttributeContainerList::iterator iterator;

    // CLASS METHODS
    static void initialize(bael_CategoryManager *categoryManager,
                           bslma::Allocator     *globalAllocator = 0);
        // Initialize the static data members of 'bael_AttributeContext' using
        // the specified 'categoryManager'.  Optionally specify a
        // 'globalAllocator' used to supply memory.  If 'globalAllocator' is 0,
        // the currently installed global allocator is used.  Calling this
        // method more than once will log an error message to 'stderr', but
        // will have no other effect.  Note that in practice this method will
        // be called *automatically* when the 'bael_LoggerManager' is
        // initialized -- i.e., it is not intended to be called directly by
        // clients of the 'bael' package.

    static bael_AttributeContext *getContext();
        // Return the address of the current thread's attribute context or,
        // if no such context exists, create one and install it in thread-local
        // storage, and return the address of the newly created context.
        // Note that this method can be invoked safely, even if
        // 'bael_AttributeContext::initialize()' has not been called.

    static bael_AttributeContext *lookupContext();
        // Return the address of the modifiable 'bael_AttributeContext' object
        // installed in local storage for the current thread, or 0 if no
        // attribute context has been created for this thread.  Note that this
        // method can be invoked safely, even if
        // 'bael_AttributeContext::initialize()' has not been called.

    // MANIPULATORS
    iterator addAttributes(const bael_AttributeContainer *attributes);
        // Add the specified 'attributes' to the list of attribute
        // containers maintained by this object.  If 'attributes' is
        // subsequently modified, prior to being removed, the 'clearCache()'
        // method must be invoked.  The behavior is undefined unless
        // 'attributes' remains valid *and* *unmodified* (unless 'clearCache()
        // is called) until this object is destroyed or 'attributes' is
        // removed from this context.  Note that this method can be invoked
        // safely, even if 'bael_AttributeContext::initialize()' has not been
        // called.

    void clearCache();
        // Clear this objects cache of evaluated rules.  Note that this method
        // must be called if a 'bael_AttributeContainer' object supplied to
        // 'addAttributes()' is modified outside of this context.

    void removeAttributes(iterator element);
        // Remove the specified 'element' from the list of attribute
        // containers maintained by this object.  Note that this method can be
        // invoked safely, even if 'bael_AttributeContext::initialize()' has
        // not been called.

    // ACCESSORS
    bool hasRelevantActiveRules(const bael_Category *category) const;
        // Return 'true' if there is at least one rule defined for this process
        // that is both "relevant" to 'category' and "active"; return 'false'
        // otherwise.  A rule is "relevant" to 'category' if the rule's
        // pattern matches 'category->categoryName()', and a rule is "active"
        // if all the predicates defined for that rule are satisfied by the
        // current thread's attributes (i.e., 'bael_Rule::evaluate()' returns
        // 'true' for the collection of attributes maintained by this object).
        // This method operates on the set of rules supplied to the
        // 'initialize()' class method (which, in practice, should be the
        // global set of rules for the process).  The behavior is undefined
        // unless 'bael_AttributeContext::initialize()' has previously been
        // invoked.

    void determineThresholdLevels(bael_ThresholdAggregate *levels,
                                  const bael_Category     *category) const;
        // Populate the specified 'levels' with the threshold levels for the
        // specified 'category'.  This method compares the threshold levels
        // defined by 'category' with those of any active rules that apply to
        // that category, and determines the minimum severity (i.e., the
        // maximum numerical value) for each respective threshold amongst
        // those values.  A rule applies to 'category' if the rule's pattern
        // matches 'category->categoryName()', and a rule is active if all the
        // predicates defined for that rule are satisfied by the current
        // thread's attributes (i.e., 'bael_Rule::evaluate()' returns 'true'
        // for the collection of attributes maintained by this object).  This
        // method operates on the set of rules supplied to the
        // 'initialize()' class method (which, in practice, should be the
        // global set of rules for the process).  The behavior is undefined
        // unless 'bael_AttributeContext::initialize()' has previously been
        // invoked.

    bool hasAttribute(const bael_Attribute& value) const;
        // Return 'true' if an attribute having specified 'value' exists in
        // any of the attribute containers maintained by this object, and
        // 'false' otherwise.  Note that this method can be invoked safely
        // even if 'bael_AttributeContext::initialize()' has not been called.

    const bael_AttributeContainerList& containers() const;
        // Return a reference to the non-modifiable list of attribute
        // containers maintained by this object.  Note that this method can be
        // invoked safely even if 'bael_AttributeContext::initialize()' has
        // not been called.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bael_AttributeContext& context);
        // Write a description of the data members of the specified 'context'
        // to the specified 'stream' in a single-line human readable format,
        // and return a reference to the modifiable 'stream'.

                     // ==================================
                     // class bael_AttributeContextProctor
                     // ==================================

class bael_AttributeContextProctor {
   // This class implements a proctor that, on its own destruction, will
   // destroy the attribute context of the current thread.  Attribute
   // contexts are stored in thread local memory.  On destruction, objects of
   // this type will deallocate the current threads attribute context (if one
   // has been created), and set the thread local storage pointer to 0.

    // NOT IMPLEMENTED
    bael_AttributeContextProctor(const bael_AttributeContextProctor&);
    bael_AttributeContextProctor& operator=(
                                          const bael_AttributeContextProctor&);
  public:
    // CREATORS
    explicit bael_AttributeContextProctor();
        // Create a 'bael_AttributeContextProctor' object, which will destroy
        // the current attribute context on destruction.

    ~bael_AttributeContextProctor();
        // Destroy this object (as well as the current attribute context).
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

               // -----------------------------------------------
               // class bael_AttributeContext_RuleEvaluationCache
               // -----------------------------------------------

// CREATORS
inline
bael_AttributeContext_RuleEvaluationCache::
bael_AttributeContext_RuleEvaluationCache()
: d_evalMask(0)
, d_resultMask(0)
, d_sequenceNumber(-1)
{
}

// MANIPULATORS
inline
void bael_AttributeContext_RuleEvaluationCache::clear()
{
    d_evalMask       = 0;
    d_resultMask     = 0;
    d_sequenceNumber = -1;
}

// ACCESSORS
inline
bool bael_AttributeContext_RuleEvaluationCache::isDataAvailable(
                             int                    sequenceNumber,
                             bael_RuleSet::MaskType relevantRulesMask) const
{
    return sequenceNumber    == d_sequenceNumber
        && relevantRulesMask == (relevantRulesMask & d_evalMask);
}

inline
bael_RuleSet::MaskType
bael_AttributeContext_RuleEvaluationCache::knownActiveRules() const
{
    return d_resultMask;
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(
                     bsl::ostream&                                    stream,
                     const bael_AttributeContext_RuleEvaluationCache& cache)
{
    return cache.print(stream, 0, -1);
}

                        // ---------------------------
                        // class bael_AttributeContext
                        // ---------------------------

// MANIPULATORS
inline
bael_AttributeContext::iterator
bael_AttributeContext::addAttributes(
                           const bael_AttributeContainer *attributeContainer)
{
    d_ruleCache_p.clear();
    return d_containerList.pushFront(attributeContainer);
}

inline
void bael_AttributeContext::clearCache()
{
    d_ruleCache_p.clear();
}

inline
void bael_AttributeContext::removeAttributes(iterator element)
{
    d_ruleCache_p.clear();
    d_containerList.remove(element);
}

// ACCESSORS
inline
const bael_AttributeContainerList& bael_AttributeContext::containers() const
{
    return d_containerList;
}

inline
bool bael_AttributeContext::hasAttribute(const bael_Attribute& value) const
{
    return d_containerList.hasValue(value);
}

                        // ----------------------------------
                        // class bael_AttributeContextProctor
                        // ----------------------------------

// CREATORS
inline
bael_AttributeContextProctor::bael_AttributeContextProctor()
{
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bael_AttributeContext& context)
{
    return context.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
