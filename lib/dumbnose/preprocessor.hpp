#pragma once

//
//  preprocessor
//
//	Useful preprocessor macros
//
#define CONCATENATE_DIRECT(s1, s2) s1##s2
#define CONCATENATE(s1, s2) CONCATENATE_DIRECT(s1, s2)
#define ANONYMOUS_VARIABLE(str) CONCATENATE(str, __LINE__)

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
