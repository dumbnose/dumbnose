#pragma once


#include <dumbnose/critical_section.hpp>
#include <dumbnose/semaphore.hpp>
#include <list>

namespace dumbnose {

template<class job_t>
class job_queue
{
public:
	job_queue() : sem_(0,1000) {}

	void add_job(job_t& job)
	{
		HOLD_LOCK(list_lock_);
		job_list_.push_back(job);
		sem_.release();
	}

	job_t get_job()
	{
		sem_.acquire();
		HOLD_LOCK(list_lock_);
		job_t job = job_list_.front();
		job_list_.pop_front();

		return job;
	}

private:
	dumbnose::semaphore sem_;
	dumbnose::critical_section list_lock_;
	std::list<job_t> job_list_;
};


} // namespace dumbnose
