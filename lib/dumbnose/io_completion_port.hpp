#pragma once


#include <boost/noncopyable.hpp>
#include <dumbnose/windows_exception.hpp>
#include <dumbnose/windows_handle.hpp>


namespace dumbnose {


class io_completion_port : private boost::noncopyable {
public:
	io_completion_port(int max_concurrent_threads = 0)
	{
		 port_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE,0,0,max_concurrent_threads);
		 if(!port_) throw windows_exception("Could not create completion port");
	}

	void associate(const windows_handle& device, ULONG_PTR comp_key)
	{
		HANDLE port = CreateIoCompletionPort(device.handle(),port_.handle(),comp_key,0);
		if(port_ != port) throw windows_exception("Could not associate device with completion port");
	}

	void associate(SOCKET socket, ULONG_PTR comp_key) 
	{
		return(associate(reinterpret_cast<HANDLE>(socket), comp_key));
	}

	void post_status(ULONG_PTR comp_key, DWORD num_bytes = 0, OVERLAPPED* overlapped = NULL) 
	{
		BOOL ok = PostQueuedCompletionStatus(port_.handle(), num_bytes, comp_key, overlapped);
		if(!ok) throw windows_exception("Could not post status to completion port");
	}

	void get_status(ULONG_PTR& comp_key, DWORD& num_bytes, OVERLAPPED*& overlapped, DWORD wait_time_ms = INFINITE) 
	{
		BOOL ok = GetQueuedCompletionStatus(port_.handle(), &num_bytes, &comp_key, &overlapped, wait_time_ms);
		if(!ok) throw windows_exception("Could not get status from completion port");
	}

private:
	windows_handle port_;
};


} // namespace dumbnose
