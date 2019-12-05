#pragma once

//
//	File: scope_guard.hpp
//
//	Author: John Sheehan
//
//	Description:  
//
//	Implementation of scope_guard(TM), a class that allows actions to be
//	queued up and performed when a particular scope is exited, unless it
//	is dismissed.  This is taken from an article in the December 2000 
//	Experts Forum of the C/C++ Users Journal, Online Edition.  It was 
//	written by Andrei Alexandrescu and Petru Marginean.  
//	
//	Since the actual source had problems, I have reimplemented the 
//	ideas in the article based on code snippets and an understanding of
//	implementation details.
//


//
//	Macro for simplifying the case where you don't need to dismiss
//
#define CONCATENATE_DIRECT(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)

#define ON_BLOCK_EXIT dumbnose::scope_guard ANONYMOUS_VARIABLE(scopeGuard) = dumbnose::make_guard



#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/punctuation/paren.hpp>
#include <boost/preprocessor/repetition.hpp>

#ifndef MAX_SCOPE_GUARD_PARAMS
#define MAX_SCOPE_GUARD_PARAMS 15
#endif

namespace dumbnose {

//
//	Base class for Specific ScopeGuards
//
class scope_guard_impl_base
{
public:
    void Dismiss() const throw()
    {    dismissed_ = true;    }
protected:
    scope_guard_impl_base() : dismissed_(false)
    {}
    scope_guard_impl_base(const scope_guard_impl_base& other)
    : dismissed_(other.dismissed_)
    {    other.Dismiss();    }
    ~scope_guard_impl_base() {} // nonvirtual (see below why)
    mutable bool dismissed_;

private:
    // Disable assignment
    scope_guard_impl_base& operator=(
        const scope_guard_impl_base&);
};
// typedef used for defining scope_guard
typedef const scope_guard_impl_base& scope_guard;

#define BOOST_PP_ITERATION_LIMITS (0, 10)
#define BOOST_PP_FILENAME_1       "test_cpp_iter.hpp"
#include BOOST_PP_ITERATE()

}  // namespace dumbnose
