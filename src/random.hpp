#pragma once

#include <stdlib.h>
#include <windows.h>

namespace dumbnose {



class random {
public:
	random() {
		LARGE_INTEGER count;
		QueryPerformanceCounter(&count);
		srand(count.LowPart);
	}

	size_t get_random_number(size_t lower,size_t upper)
	{
		return (rand()%(upper-lower)) + lower;
	}

};


} // namespace dumbnose
