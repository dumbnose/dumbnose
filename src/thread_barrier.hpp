#pragma once

#include <dumbnose/mutex.hpp>
#include <dumbnose/event.hpp>
#include <dumbnose/waiter.hpp>
#include <dumbnose/shared_memory.hpp>


namespace dumbnose {


class thread_barrier
{
public:
	thread_barrier(unsigned int threshold) : 
	  event_(true,false),
	  threshold_(threshold),
	  thread_count_(sizeof(unsigned int),L"")
	 {

	 }

	 thread_barrier(unsigned int threshold, std::wstring name) : 
	  lock_(name+L"Mutex"),
	  event_(true,false,name+L"Event"),
	  threshold_(threshold),
	  thread_count_(sizeof(unsigned int),name+L"SharedMem")
	 {
	 }

	 void wait()
	 {
		lock_.acquire();

		(*thread_count())++;
		if(*thread_count()==threshold_) {
			//
			// Wake up all waiters
			//
			*thread_count()=0;
			event_.pulse();
			lock_.release();
		}
		else {
			// Use signal_and_wait to make operation atomic
			dumbnose::waiter::signal_and_wait(lock_,event_);
		}

	 }

private:
	unsigned int* thread_count() {
		return static_cast<unsigned int*>(thread_count_.mem_start());
	}

	mutex			lock_;			// Protects data structures
	event			event_;			// Event that gets signaled when all threads ready
	unsigned int	threshold_;		// Number of required threads
    shared_memory	thread_count_;	// Current number of threads waiting
};


};	// namespace dumbnose