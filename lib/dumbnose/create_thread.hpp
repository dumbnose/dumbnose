//
// Simple wrapper around the Win32 CreateThread call.  Just as with
// CreateThread, you need to ensure you don't hand the function a
// pointer or reference that can potentially go out of scope before
// the function runs.
//
// Also note that the use of bind and function cause non-ref
// arguments to get copied quite a few times.  Therefore, refs or
// shared_ptr's should be used for expensive to copy objects.
//

#pragma once

#include <windows.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>


namespace dumbnose {


template</*boost::function<void(void)>*/class functor_t>
DWORD WINAPI create_thread_indirect(void* param)
{
	// Take ownership of the function object
	boost::scoped_ptr<functor_t> 
		func(static_cast<functor_t*>(param));

	// Call the function
	(*func)();

	return 0;
}


template</*boost::function<void(void)>*/class functor_t>
bool create_thread(functor_t functor, HANDLE* thread = NULL)
{
	functor_t* func = new functor_t(functor);
	//*func = functor;

	HANDLE thr = CreateThread(NULL,0,&create_thread_indirect<functor_t>,func,0,NULL);
	if(NULL==thr) return false;

	if(thread!=NULL) {
		*thread = thr;
	} else {
        CloseHandle(thr);  // the caller doesn't want it, so close it
	}

	return true;
}


} // namespace dumbnose
