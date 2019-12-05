#pragma once

#include <map>
#include <functional>
#include <cassert>
#include <mutex>

namespace dumbnose {

template<class sender_t, class event_args_t>
class event_source
{
public: 
	using event_handler_t = std::function<void(sender_t source, event_args_t args)>;
	using cookie_t = uint64_t;

	// Register a listener and receive a cookie to revoke later.
	cookie_t register_listener(event_handler_t listener)
	{
		std::unique_lock<std::mutex> lock(lock_);
		cookie_t cookie = ++largest_event_cookie_;
		listeners_[cookie] = listener;

		return cookie;
	}

	void unregister_listener(cookie_t cookie)
	{
		if (cookie == 0) return;

		std::unique_lock<std::mutex> lock(lock_);

		assert((cookie != 0) && (cookie <= largest_event_cookie_));
		assert(listeners_.find(cookie) != listeners_.end());

		listeners_.erase(cookie);
	}

	void raise(sender_t sender, event_args_t args)
	{
		std::unique_lock<std::mutex> lock(lock_);
		std::map<uint64_t, event_handler_t> listeners = listeners_;
		lock.unlock();

		for (auto& listener : listeners)
		{
			listener.second(sender, args);
		}

		event_.notify_all();
	}

	void wait()
	{
		std::unique_lock<std::mutex> lock(lock_);
		event_.wait(lock);
	}

private:
	std::mutex lock_;
	cookie_t largest_event_cookie_ = 0;
	std::map<uint64_t, event_handler_t> listeners_;
	std::condition_variable event_;
};

}
