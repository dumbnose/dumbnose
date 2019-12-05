#pragma once

#include <dumbnose\safe_map.hpp>
#include <cassert>

#pragma warning(disable:4786)

namespace dumbnose 
{


template<class T>
class memory_tracking
{
public:
	static void * operator new(size_t size)
	{
		void * raw = ::operator new(size);
		mem_list_.insert(std::make_pair(raw,size));

		return raw;
	}

	static void operator delete(void* raw)
	{
		if(NULL == raw) return;

		mem_list_t::iterator it = mem_list_.find(raw);
		assert(it!=mem_list_.end());

		mem_list_.erase(it);
		::operator delete(raw);
	}

protected:
	typedef safe_map<void*,size_t> mem_list_t;
	static mem_list_t mem_list_;
};

template <class T> typename memory_tracking<T>::mem_list_t memory_tracking<T>::mem_list_;


} // namespace dumbnose