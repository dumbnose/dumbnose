#pragma once

//
//  fixed_string
//
//	Defines strings based on std::basic_string<> that use a fixed string
//	buffer.  Useful for leveraging the std::basic_string<> interface without
//	having to use dynamic memory.  
//
//	@todo:	It would be dangerous to try to append to the string to make it 
//	longer than the supplied buffer.  The macros define the strings to be const.
//	If someone manually defines one, they will need to be careful.  Could add
//	checking later.
//

#include <string>
#include <dumbnose/preprocessor.hpp>
#include <dumbnose/memory.hpp>


#define FIXED_STRING(var_name, string_val) \
	char CONCATENATE(var_name,_array)[] = string_val;		\
	const dumbnose::fixed_string var_name(string_val, dumbnose::fixed_allocator<char>(CONCATENATE(##var_name,_array), ARRAY_SIZE(string_val)));

#define FIXED_WSTRING(var_name, string_val) \
	wchar_t CONCATENATE(var_name,_array)[] = string_val;		\
	const dumbnose::fixed_wstring var_name(string_val, dumbnose::fixed_allocator<wchar_t>(CONCATENATE(##var_name,_array), ARRAY_SIZE(string_val)));

namespace dumbnose {

// convenient typedefs
typedef std::basic_string<char,std::char_traits<char>, dumbnose::fixed_allocator<char> > fixed_string;
typedef std::basic_string<wchar_t,std::char_traits<wchar_t>, dumbnose::fixed_allocator<wchar_t> > fixed_wstring;

} // namespace dumbnose

