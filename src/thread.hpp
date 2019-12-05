#pragma once

#include <windows.h>

namespace dumbnose {


template <typename thread_impl>
class thread
{
	typedef thread_impl this_t;

public:
	thread() : thread_handle_(0),thread_id_(0){}
	~thread(){if(thread_handle_!=0) CloseHandle(thread_handle_);}

	void run(){
		thread_handle_ = CreateThread(0,0,&thread_starter,this,0,&thread_id_);
		if(thread_handle_==0) throw GetLastError();
	}

protected:
	static DWORD WINAPI thread_starter(void* param)
	{
		this_t* _this = reinterpret_cast<this_t*>(param);
		return _this->start();
	}

	HANDLE thread_handle_;
	DWORD thread_id_;
};


} // namespace dumbnose
