#pragma once


#include<dumbnose/windows_exception.hpp>


namespace dumbnose {


class windows_handle
{
public:

	//
	// various forms of assignment
	//
	windows_handle() : handle_(0)
	{
	}

	windows_handle(const HANDLE handle) : handle_(handle)
	{
	}

	windows_handle(const windows_handle& other) : handle_(0)
	{
		*this = other;
	}

	windows_handle& operator=(HANDLE handle)
	{
		close();			// make sure the old one is closed
		handle_ = handle;

		return *this;
	}

	windows_handle& operator=(const windows_handle& other) 
	{

		HANDLE new_handle;

		if(!DuplicateHandle(GetCurrentProcess(),other.handle_,GetCurrentProcess(),&new_handle,0,FALSE,DUPLICATE_SAME_ACCESS)) {
			throw windows_exception("Could not duplicate handle",GetLastError());
		}

		return *this = new_handle;
	}

	//
	// various forms of destruction
	//
	~windows_handle()
	{
		try {
			close();
		} catch(std::exception&) {
			// don't let an exception propogate out of the destructor 
		}
	}

	void close()
	{
		if(*this) {
			if(!CloseHandle(handle_)) throw windows_exception("Could not close handle",GetLastError());
			handle_ = NULL;
		}
	}

	//
	// comparisons
	//
	bool operator==(const HANDLE other) const
	{
		return this->handle_ == other;
	}

	bool operator==(const windows_handle& other) const
	{
		return this->handle_ == other.handle_;
	}

	bool operator!=(const HANDLE other) const
	{
		return !this->operator==(other);
	}

	bool operator!=(const windows_handle& other) const
	{
		return !this->operator==(other);
	}

	//
	// casts
	//
	operator bool() const
	{
		return (handle_!=NULL) && (handle_!=INVALID_HANDLE_VALUE);
	}

	HANDLE handle() const
	{
		return handle_;
	}

	HANDLE& handle_ref()
	{
		return handle_;
	}


private:
	HANDLE handle_;
};



} // namespace dumbnose
