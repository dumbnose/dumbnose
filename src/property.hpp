#pragma once

#include "null_type.hpp"
#include "validator.hpp"


namespace dumbnose {


template<typename type>
class property_storage
{
public:
	property_storage(){}
	property_storage(const type& value) : value_(value)
	{
	}

protected:
	type value_;
};


template<typename type>
class read : public property_storage<type>
{
public:
	read(){}
	read(const type& value) : property_storage<type>(value)
	{
	}

	const type& get() const {
		return value_;
	}
};


template<typename type>
class read_write : public read<type>
{
public:
	read_write(){}
	read_write(const type& value) : read<type>(value)
	{
	}

	type& put(const type& value) {
		return value_ = value;
	}

	type& get() {
		return value_;
	}

	const type& get() const {
		return value_;
	}
};


template <typename type, template<typename> class access=read_write>
class property : public access<type>
{
public:
	typedef property<type,access> this_t;

	//
	// default constructor (will only work if contained type is default constructible)
	//
	property(){}

	//
	// assignment handlers
	//
	property(const type& value) 
		: access<type>(value)
	{
	}

	const type& operator=(const type& value) {
		this->put(value);

		return this->get();
	}

	type& operator()(const type& value) {
		return this->put(value);
	}

	//
	// accessors
	//
	operator const type&() {
		return this->get();
	}

	const type& operator()() {
		return this->get();
	}

	//
	// equality
	//
	bool operator==(const type& value) const {
		return value_ == value;
	}

	bool operator==(const this_t& value) const {
		return value_ == value.get();
	}

	//
	// comparator
	//
	bool operator<(const type& value) const {
		return value_ < value;
	}

	bool operator<(const this_t& value) const {
		return value_ < value.get();
	}


	//
	// stream handlers
	//
	std::ostream& operator<<(std::ostream& os) {
		return os << value_;
	}

	std::ostream& operator<<(std::ostream& os) const {
		return os << value_;
	}

	std::istream& operator>>(std::istream& is) {
		type temp; // need a temp value, so we can use ->put() to validate
		is >> temp;
		this->put(temp);

		return is;
	}
};


template <class parent, typename type, template<typename> class access=read_write, class validator_t=validator<type> >
class accessible_property : public property<type,access>
{
	friend parent;

public:
	//
	// default constructor (will only work if contained type is default constructible)
	//
	accessible_property(){}

	//
	// assignment handlers (defer to parent)
	//
	accessible_property(const type& value) 
		: property<type,access>(value)
	{
	}

	type& operator=(const type& value) {
		return this->property<type,access>::operator=(value);
	}
};


//
// stream declarations to make these work with i/o stream's
//
template<typename type, template<typename> class access>
std::ostream& operator<<(std::ostream& os, const property<type,access>& value) {
	return value << os;
}

template<typename type, template<typename> class access>
std::ostream& operator<<(std::ostream& os, property<type,access>& value) {
	return value << os;
}

template<typename type, template<typename> class access>
std::istream& operator>>(std::istream& is, property<type,access>& value) {
	return value >> is;
}




} // namespace dumbnose

/*
, class is_valid=dumbnose::validator<type> 

		// Since it is being set through the constructor, it should be OK to throw after
		// assignment, since the object will never be fully created.
		//
		// However, there is one case where this could be an issue.  If the object has
		// destructive copy semantics, e.g. std::auto_ptr, the object would be destroyed
		// when the exception is thrown.  The alternative is less appealing, as it requires
		// the object to have a default constructor AND it will create an object that will
		// just be thrown away.
		is_valid::validate(value);
*/
