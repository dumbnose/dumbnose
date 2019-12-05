// cstring_wrapper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <dumbnose/cstring_wrapper.hpp>
//using dumbnose::cstring_wrapper;

#include <dumbnose/fixed_string.hpp>
using dumbnose::fixed_wstring;

#include <dumbnose/cstring_wrapper.hpp>
using dumbnose::cstring_wrapper;

int wmain(int argc, wchar_t* argv[])
{
	try {

		FIXED_WSTRING(wstr,L"My name is julio");
		FIXED_STRING(str,"My name is julio");

		std::wcout << L"Wstring: " << wstr << std::endl;
		std::cout << "String: " << str << std::endl;

		wchar_t test_string[] = L"I am long enough to work";
		dumbnose::fixed_wstring test(L"I am long enough to work", dumbnose::fixed_allocator<wchar_t>(test_string, ARRAY_SIZE(test_string)));
		test += L"bar";

		std::wcout << L"Test: " << test << std::endl;

		//cstring_wrapper str("hello");
		//std::cout << str << std::endl;

	} catch(std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}
