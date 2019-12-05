#pragma once

#include <functional>
#include <dumbnose/string_utils.hpp>


namespace dumbnose {


template<typename char_t>
struct iless : public std::binary_function<char_t, char_t, bool>
{
	bool operator()(const wchar_t& left, const wchar_t& right) const
	{
		return (toupper_t<char_t>(left) < toupper_t<char_t>(right));
	}
};


} // namespace dumbnose
