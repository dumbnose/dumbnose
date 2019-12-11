#pragma once

#include <map>
#include <functional>
#include <cassert>
#include <mutex>
#include <dumbnose/noncopyable.hpp>

namespace dumbnose {


template<class event_source_t>
class event_source_cookie;

template<class sender_t, class event_args_t>
class event_source
{
public: 
	using this_t = event_source<sender_t, event_args_t>;
	using event_handler_t = std::function<void(sender_t source, event_args_t args)>;
	using cookie_t = event_source_cookie<this_t>;

	// Register a listener and receive a cookie to revoke later.
	cookie_t register_listener(event_handler_t listener)
	{
		std::unique_lock<std::mutex> lock(lock_);
		uint64_t cookieNum = ++largest_event_cookie_;
		listeners_[cookieNum] = listener;

		event_source_cookie<this_t> cookie;
		cookie.initialize(this, cookieNum);

		return cookie;
	}

	void unregister_listener(uint64_t cookie)
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
	uint64_t largest_event_cookie_ = 0;
	std::map<uint64_t, event_handler_t> listeners_;
	std::condition_variable event_;
};


#pragma warning( push )
#pragma warning( disable : 4521 )
#pragma warning( disable : 4522 )
template<class event_source_t>
class event_source_cookie
{
public:
	event_source_cookie() = default;

	~event_source_cookie()
	{
		unregister();
	}

	event_source_cookie(event_source_cookie<event_source_t>& other)
	{
		other = *this;
	}

	event_source_cookie& operator=(event_source_cookie<event_source_t>& other)
	{
		other.cookie_ = cookie_;
		other.event_source_ = event_source_;
		cookie_ = 0;
		event_source_ = nullptr;

		return *this;
	}

	void initialize(event_source_t* event_source, uint64_t cookie)
	{
		event_source_ = event_source;
		cookie_ = cookie;
	}

	void unregister_early()
	{
		unregister();
	}

protected:
	void unregister()
	{
		if (cookie_ != 0) {
			event_source_->unregister_listener(cookie_);
			event_source_ = nullptr;
			cookie_ = 0;
		}
	}

private:
	event_source_t* event_source_ = nullptr;
	uint64_t cookie_ = 0;
};
#pragma warning( pop )


}
