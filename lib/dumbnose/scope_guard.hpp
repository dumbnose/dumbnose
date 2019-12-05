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

#include "preprocessor.hpp"
//
//	Macro for simplifying the case where you don't need to dismiss
//
#define ON_BLOCK_EXIT auto ANONYMOUS_VARIABLE(scopeGuard) = dumbnose::make_guard

namespace dumbnose {

//
//	Base class for Specific ScopeGuards
//
    template<class lambda_t>
	class scope_guard {
	public:
        explicit scope_guard(lambda_t fun) throw() /*noexcept*/ 
            : fun_(fun) {}

        // destructor will execute the functor, if not dismissed
        ~scope_guard() throw() /*noexcept*/
        {
            if (!dismissed_)
            {
                doit();
            }
        }

        void dismiss() throw() /*noexcept*/ 
		{    
            dismissed_ = true;
        }

        // Throwing during processing is completely unsafe, because if two guards in the same method both throw,
        // they will nest, causing abnormal process temrination.  So, just disallow it.
        void doit() throw() /*noexcept*/
        {
            dismiss();  // functor should only every try to run once
            fun_();
        }

    private:
        lambda_t fun_;
        bool dismissed_ = false;
    };

    template<class lambda_t>
    scope_guard<lambda_t> make_guard(lambda_t fun) throw() /*noexcept*/
    {
        return scope_guard<lambda_t>(fun);
    }

}  // namespace dumbnose
