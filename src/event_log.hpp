#pragma once


#include "windows_handle.hpp"
#include "windows_exception.hpp"


namespace dumbnose {


class event_log {

public:

	event_log(const std::wstring& event_source_name) {
		log_ = RegisterEventSource(0, event_source_name.c_str());
		if ((log_==0) || (log_== INVALID_HANDLE_VALUE)) throw windows_exception("Could not register event source", GetLastError());
	}

	~event_log() {
		DeregisterEventSource(log_);
	}

	static void output_error(const std::wstring& event_source_name, DWORD error, const std::wstring& text)
	{
			dumbnose::event_log el(event_source_name);
			el.add_error(error,text);
			std::wcerr << text << std::endl;
	}

	static void output_error(const std::wstring& event_source_name, DWORD error, const std::string& text)
	{
			dumbnose::event_log el(event_source_name);
			el.add_error(error,text);
			std::cerr << text << std::endl;
	}

	/*	--------------------------------------------------------------	*\
		Write a message to the event log 
	\*	--------------------------------------------------------------	*/
	void add_error(DWORD error, const std::wstring& text)
	{
		const wchar_t* msg = text.c_str();
		ReportEvent(log_, EVENTLOG_ERROR_TYPE, 0, error, 0, 1, 0, &msg, 0);
	}

	void add_error(DWORD error, const std::string& text)
	{
		const char* msg = text.c_str();
		ReportEventA(log_, EVENTLOG_ERROR_TYPE, 0, error, 0, 1, 0, &msg, 0);
	}

private:
	HANDLE log_;
};


} // namespace dumbnose
