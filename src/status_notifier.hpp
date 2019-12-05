#pragma once
#include <dumbnose\singleton.hpp>
#include <sstream>

namespace dumbnose {


class status_notifier : public dumbnose::singleton<status_notifier>
{
public:

	// operations
	void update_status(std::wstring const & status)
	{
		status_updated_event.raise(*this, status);
	}
	

	void report_error(std::wstring const & message)
	{
		error_occurred_event.raise(*this, message);
	}

	void report_error(std::wstring const & message, uint32_t num)
	{
		std::wstring error_msg = (boost::wformat(L"0x%x: %s") % num % message).str();
		error_occurred_event.raise(*this, error_msg);
	}

	void report_error(std::string const & message)
	{
		auto wide_message = (boost::wformat(L"%S") % message.c_str()).str();
		report_error(wide_message);
	}

	void report_error(std::string const & message, uint32_t num)
	{
		auto wide_message = (boost::wformat(L"%S") % message.c_str()).str();
		report_error(wide_message, num);
	}

	void report_exception(std::exception & error)
	{
		report_error(error.what());
	}

	void trace(std::wstring const & trace_message)
	{
		OutputDebugString(trace_message.c_str());
		trace_history_ << L"\n" << trace_message;
		trace_received_event.raise(*this, trace_message);
	}

	void trace_assert(bool condition, std::wstring const & message)
	{
		assert(condition);
		if (!condition) trace(message);
	}

	std::wstring trace_history() { return trace_history_.str();  }

	// events
	using status_updated_event_t = event_source<status_notifier&, std::wstring const &>;
	status_updated_event_t status_updated_event;

	using error_occurred_event_t = event_source<status_notifier&, std::wstring const &>;
	error_occurred_event_t error_occurred_event;

	using exception_occurred_event_t = event_source<status_notifier&, std::wstring const &>;
	exception_occurred_event_t exception_occurred_event;

	using trace_received_event_t = event_source<status_notifier&, std::wstring const &>;
	trace_received_event_t trace_received_event;

private:
	std::wstringstream trace_history_;
};


}
