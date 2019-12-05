#pragma once

#include <dumbnose/waiter.hpp>


namespace dumbnose {


class waitable_timer {
	friend class waiter;

public:
	waitable_timer() {
		timer_ = CreateWaitableTimer(0,FALSE,0);
		if(timer_==NULL) throw windows_exception("Could not create waitable timer");
	}

	void set_hours(unsigned int hours) {
		LARGE_INTEGER due;
		due.QuadPart=-hours_to_100_nanosecs(hours);
		if(!SetWaitableTimer(timer_,&due,0,0,0,0)) throw windows_exception("Could not set waitable timer");
	}

	void wait(unsigned int wait_time=INFINITE) const {
		DWORD status = WaitForSingleObject(timer_,wait_time);
		if(status!=WAIT_OBJECT_0) throw windows_exception("Wait for timer failed");
	}

private:
	HANDLE handle() const {return timer_;}
	HANDLE timer_;

	inline static long long secs_to_100_nanosecs(long long secs)
	{
		return secs*10000000;
	}

	inline static long long minutes_to_secs(long long minutes)
	{
		return minutes*60;
	}

	inline static long long hours_to_minutes(long long hours)
	{
		return hours*60;
	}

	inline static long long hours_to_100_nanosecs(long long hours)
	{
		return secs_to_100_nanosecs(minutes_to_secs(hours_to_minutes(hours)));
	}
};


} // namespace dumbnose