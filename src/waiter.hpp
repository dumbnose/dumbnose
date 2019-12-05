#pragma once

#include <dumbnose/null_type.hpp>

namespace dumbnose {

//
// 
//
class waiter
{
public:

	template<typename lock1_t, typename lock2_t>
	static unsigned short signal_and_wait(lock1_t& lock1, lock2_t& lock2)
	{
		return static_cast<unsigned short>(SignalObjectAndWait(lock1.handle(),lock2.handle(),INFINITE,false));
	}

	template<typename lock_t>
	static unsigned short wait(lock_t& lock)
	{
		HANDLE handles[1];
		handles[0] = lock.handle();

		return wait_any_impl(1,handles);
	}

	template<typename lock1_t, typename lock2_t>
	static unsigned short wait_any(lock1_t& lock1, lock2_t& lock2)
	{
		HANDLE handles[2];
		handles[0] = lock1.handle();
		handles[1] = lock2.handle();

		return wait_any_impl(2,handles);
	}

	template<typename lock1_t, typename lock2_t, typename lock3_t>
	static unsigned short wait_any(lock1_t& lock1, lock2_t& lock2, lock3_t& lock3)
	{
		HANDLE handles[3];
		handles[0] = lock1.handle();
		handles[1] = lock2.handle();
		handles[2] = lock3.handle();

		return wait_any_impl(3,handles);
	}

private:
	static unsigned short wait_any_impl(unsigned short count, HANDLE handles[])
	{

		DWORD result = WaitForMultipleObjects(count,handles,false,INFINITE);
		if(result==WAIT_FAILED) throw windows_exception("wait_any failed");

		if(result>=WAIT_ABANDONED_0) return static_cast<unsigned short>(result-WAIT_ABANDONED_0+1);
		return static_cast<unsigned short>(result-WAIT_OBJECT_0+1);
	}

};


} // namespace dumbnose
