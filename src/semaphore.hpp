#include <windows.h>
#include <string>
#include <dumbnose/waiter.hpp>
#include <cassert>


namespace dumbnose
{


class semaphore
{
	friend class waiter;

public:
	semaphore(unsigned long initial_count, unsigned long max_count, std::wstring name){
		semaphore_ = CreateSemaphore(NULL,initial_count, max_count,name.c_str());
		if(NULL==semaphore_)
			throw GetLastError();
	}

	explicit semaphore(unsigned long initial_count, unsigned long max_count){
		semaphore_ = CreateSemaphore(NULL,initial_count, max_count,NULL);
		if(NULL==semaphore_)
			throw GetLastError();
	}

	~semaphore(){
		assert(NULL!=semaphore_);
		CloseHandle(semaphore_);
	}

	void acquire(unsigned int wait_time=INFINITE) const {
		assert(NULL!=semaphore_);

		DWORD result = WaitForSingleObject(semaphore_,wait_time);
		if(WAIT_OBJECT_0!=result) 
			throw GetLastError();
	}

	void release() const {
		assert(NULL!=semaphore_);

		ReleaseSemaphore(semaphore_,1,NULL);
	}

private:
	HANDLE handle() const {
		return semaphore_;
	}

	mutable HANDLE semaphore_;
};


} // namespace dumbnose