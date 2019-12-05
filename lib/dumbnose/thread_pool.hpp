#pragma once

#include <dumbnose/mutex.hpp>
#include <dumbnose/semaphore.hpp>
#include <dumbnose/event.hpp>
#include <dumbnose/waiter.hpp>
#include <dumbnose/safe_list.hpp>
#include <dumbnose/cmdline_options.hpp>
#include <boost/shared_ptr.hpp>
#include <dumbnose/lock.hpp>
#include <limits>


namespace dumbnose {


template<class work_item_t>
class thread_pool
{
	typedef boost::shared_ptr<work_item_t> work_item_ptr;

public:
	thread_pool():die_(true,false),work_item_sem_(0,std::numeric_limits<long>::max()) {}
	~thread_pool(){release_threads();}

	void release_threads(){die_.set();}

	work_item_ptr get_work_item()
	{
		while(true)
		{
			if(1==waiter::wait_any(die_,work_item_sem_)) {
				// The die_ event was set, return with getting a work item 
				return work_item_ptr();
			}

			//
			// The semaphore was upped because a job is waiting, try to get it.  
			// NOTE:  Double check to ensure there is a work item queued.
			//
			HOLD_LOCK(work_item_queue_);

			// handle empty queue case
			if(work_item_queue_.empty()) continue;

			// retrieve the item at the front of the list
			work_item_ptr work_item = work_item_queue_.front();
			work_item_queue_.pop_front();

			return work_item;
		}
	}

	void add_work_item(work_item_ptr work_item)
	{
		work_item_queue_.push_back(work_item);

		// Up the semaphore by one, so a thread will get
		// woken up.
		work_item_sem_.release();
	}

protected:
	// Member variables
	semaphore work_item_sem_;
	event die_;
	safe_list<work_item_ptr> work_item_queue_;
};

} // namespace dumbnose
