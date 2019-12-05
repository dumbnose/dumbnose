#pragma once

namespace dumbnose {


template<class type_t>
class fixed_allocator 
{
public:
	typedef type_t value_type;
	typedef type_t *pointer;
	typedef type_t& reference;
	typedef const type_t* const_pointer;
	typedef const type_t& const_reference;

	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	// constructors
	fixed_allocator<type_t>(pointer memory, size_t size) : storage_(memory), size_(size) {}

	pointer allocate(size_type count)
	{
		assert(count<=size_);
		if(count>size_) throw std::length_error(__FUNCTION__ ": Length exceeds fixed buffer size");

		return storage_;
	}

	pointer allocate(size_type _Count, const void* hint)
	{	// use hint
		assert(count<=size_);
		if(count>size_) throw std::length_error(__FUNCTION__ ": Length exceeds fixed buffer size");

		storage_ = hint;
		return hint;
	}

	pointer address(reference val) const
	{	// return address of mutable val
		return &val;
	}

	const_pointer address(const_reference val) const
	{	// return address of nonmutable val
		return (&val);
	}

	void deallocate(pointer _Ptr, size_type)
	{	
		// noop
	}

	void destroy(pointer _Ptr)
	{
		// noop
	}

	size_type max_size() const 
	{	
		return size_;
	}

	template<class other_t>
	struct rebind
	{	// convert an allocator<_Ty> to an allocator <_Other>
		typedef fixed_allocator<other_t> other;
	};

private:
	// disable default allocator
	fixed_allocator<type_t>()/* : storage_(0){}*/;

private:
	pointer storage_;
	size_t size_;

};


} // namespace dumbnose
