#pragma once

#include <dumbnose/windows_exception.hpp>


namespace dumbnose {

class hires_timer
{
public:
	hires_timer() {
		if(!QueryPerformanceFrequency(&frequency_)) throw windows_exception("QueryPerformanceFrequency() failed");
		reset();
	}

	void reset() {
		start_ = current();
	}

	double elapsed() {
		return ((double)current().QuadPart - start_.QuadPart) / frequency_.QuadPart;
	}

protected:

	LARGE_INTEGER frequency() {
		return frequency_;
	}

	LARGE_INTEGER start() {
		return start_;
	}

	LARGE_INTEGER current() {
		LARGE_INTEGER curr;
		if(!QueryPerformanceCounter(&curr)) throw windows_exception("QueryPerformanceCounter() failed");
		return curr;
	}

private:
	LARGE_INTEGER frequency_;
	LARGE_INTEGER start_;

};

}