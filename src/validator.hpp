#pragma once

#include <exception>

namespace dumbnose {


template<typename type>
class validator
{
public:
	static void validate(const type& value)
	{
	}
};


template<typename type, type lower, type upper>
class range_validator
{
public:
	static void validate(const type& value)
	{
		if(value<lower || value>upper) {
			throw std::out_of_range("value out of range");
		}
	}
};


} // namespace dumbnose
