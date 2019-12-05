#pragma once

#include <windows.h>

namespace dumbnose
{


class critical_section
{
public:
	critical_section(){
		InitializeCriticalSection(&cs_);
	}

	~critical_section(){
		DeleteCriticalSection(&cs_);
	}

	void acquire() const {
		EnterCriticalSection(&cs_);
	}

	void release() const {
		LeaveCriticalSection(&cs_);
	}

private:
	mutable CRITICAL_SECTION cs_;
};


} // namespace dumbnose