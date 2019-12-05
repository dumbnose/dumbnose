#pragma once

#include <windows.h>
#include <string>
#include <cassert>
#include <dumbnose/waiter.hpp>


namespace dumbnose
{


class event
{
	friend class waiter;

public:
	event(bool manual_reset, bool initial_state){
		event_ = CreateEvent(NULL,manual_reset, initial_state,NULL);
		if(NULL==event_)
			throw GetLastError();
	}

	event(bool manual_reset, bool initial_state, std::wstring name){
		event_ = CreateEventW(NULL,manual_reset, initial_state,name.c_str());
		if(NULL==event_)
			throw GetLastError();
	}

	~event(){
		assert(NULL!=event_);
		CloseHandle(event_);
	}

	void wait(unsigned int wait_time=INFINITE) const {
		assert(NULL!=event_);

		DWORD result = WaitForSingleObject(event_,wait_time);
		if(WAIT_OBJECT_0!=result) 
			throw windows_exception("Could not wait for event");
	}

	void set() {
		assert(NULL!=event_);

		SetEvent(event_);
	};

	void pulse() {
		assert(NULL!=event_);

		PulseEvent(event_);
	};

	void reset() {
		assert(NULL!=event_);

		ResetEvent(event_);
	}

private:
	HANDLE handle() const {
		return event_;
	}

	mutable HANDLE event_;
};


} // namespace dumbnose