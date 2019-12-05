#pragma once

extern "C" _CRTIMP errno_t __cdecl rand_s ( __out unsigned int *_RandomValue);

#include <stdlib.h>
#include <time.h>

namespace dumbnose {

class rand_util
{
public:
	static unsigned int random() 
	{ 
		unsigned int val;
		rand_s(&val);
		return val;
	}

	static unsigned int rand_range(unsigned int min, unsigned int max)
	{
		unsigned int range = max - min + 1;
		unsigned int num = random() % range;
		return num + min;
	}
};

} // namespace dumbnose 

