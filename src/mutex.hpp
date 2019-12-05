#pragma once

#include <windows.h>
#include <string>
#include <dumbnose/waiter.hpp>
#include <cassert>


namespace dumbnose
{


class mutex
{
	friend class waiter;

public:
	mutex(){
		mutex_ = CreateMutex(NULL,false,NULL);
		if(NULL==mutex_)
			throw GetLastError();
	}

	explicit mutex(std::wstring name){
		mutex_ = CreateMutexW(NULL,false,name.c_str());
		if(NULL==mutex_)
			throw GetLastError();
	}

	~mutex(){
		assert(NULL!=mutex_);
		CloseHandle(mutex_);
	}

	void acquire(unsigned int wait_time=INFINITE) const {
		assert(NULL!=mutex_);

		DWORD result = WaitForSingleObject(mutex_,wait_time);
		if((WAIT_OBJECT_0!=result) && (WAIT_ABANDONED!=result)) 
			throw GetLastError();
	}

	void release() const {
		assert(NULL!=mutex_);

		ReleaseMutex(mutex_);
	}

private:
	HANDLE handle() const {
		return mutex_;
	}

	mutable HANDLE mutex_;
};


} // namespace dumbnose