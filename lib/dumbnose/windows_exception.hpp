#pragma once


#include <exception>
#include <string>
#include <sstream>


namespace dumbnose {

//
// Simple wrapper class that accepts a win32 error # and an exception description.
// Provides formatted version of what() that incorporates the error # and description.
//
class windows_exception : public std::exception {
public:
	windows_exception(const char* desc, unsigned long num = GetLastError())
		: std::exception(desc), num_(num)
	{
		std::stringstream message;
		message << "Error " << num_ << ": " << desc << '\n' << windows_error();
		desc_ = message.str();
	}

	virtual const char* what() const {
		return desc_.c_str();
	}

	virtual unsigned long num() const {
		return num_;
	}

	std::string windows_error() {

		char* message = 0;
		DWORD dw = GetLastError();

		DWORD result = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, num_, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL);
		if (result == 0 || message == 0) return "";

		std::string win_err(message);
		LocalFree(message);

		return win_err;
	}

private:
	unsigned long num_;
	std::string desc_;
};


} // namespace dumbnose
