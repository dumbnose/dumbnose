#pragma once

/*	----------------------------------------------------------------------	*\

	Thread-safe wrapper for STL-compliant map containers.

	@author	John Sheehan

	@todo	Need to add external locking capabilities.  These need to be
			used during copy and swap operations to protect the second
			safe_map being operated on.

			These can also be used by external operations that need to
			make sure the container's contents don't change during
			iteration.
\*	-----------------------------------------------------------------------	*/

#include <map>
#include <dumbnose/critical_section.hpp>
#include <dumbnose/lock.hpp>

namespace dumbnose {


template<typename key_t, typename value_t, typename cmp_t=std::less<key_t>,typename alloc_t=std::allocator<std::pair<key_t,value_t> >,
		 typename map_t=std::map<key_t,value_t,cmp_t,alloc_t>, typename lock_t=critical_section, 
		 typename read_lock_holder_t=lock_holder<lock_t>, typename write_lock_holder_t=read_lock_holder_t >
class safe_map : public lock_t
{
public:
	typedef safe_map<key_t,value_t,cmp_t,alloc_t,map_t,lock_t,read_lock_holder_t,write_lock_holder_t> this_type;
	typedef map_t								map_type;
	typedef key_t								key_type;
	typedef value_t								mapped_type;
	typedef cmp_t								key_compare;
	typedef typename map_t::value_type			value_type;
	typedef typename map_t::value_compare		value_compare;
	typedef typename map_t::allocator_type		allocator_type;
	typedef typename map_t::size_type			size_type;
	typedef typename map_t::difference_type		difference_type;
	typedef typename map_t::pointer				pointer;
	typedef typename map_t::const_pointer		const_pointer;
	typedef typename map_t::reference			reference;
	typedef typename map_t::const_reference		const_reference;
	typedef typename map_t::iterator			iterator;
	typedef typename map_t::const_iterator		const_iterator;
	typedef typename map_t::reverse_iterator	reverse_iterator;
	typedef typename map_t::const_reverse_iterator	const_reverse_iterator;
	typedef typename map_t::value_type			value_type;

	key_compare key_comp()		{ return impl_.key_compare(); }
	value_compare value_comp()	{ return impl_.value_compare(); }

	safe_map()
	{	// construct empty map from defaults
	}

	explicit safe_map(const key_compare& cmp)
		: impl_(cmp)
	{	// construct empty map from comparator
	}

	safe_map(const key_compare& cmp, const allocator_type& alloc)
		: impl_(cmp, alloc)
	{	// construct empty map from comparator and allocator
	}

	template<class _Iter>
		safe_map(_Iter first, _Iter last)
		: impl_(first,last)
	{	// construct map from [first, last), defaults
	}

	template<class _Iter>
		safe_map(_Iter first, _Iter last,
		const key_compare& cmp)
		: impl_(first,last,cmp)
	{	// construct map from [first, last), comparator
	}

	template<class _Iter>
		safe_map(_Iter first, _Iter last,
		const key_compare& cmp, const allocator_type& alloc)
		: impl_(first,last,cmp,alloc)
	{	// construct map from [first, last), comparator, and allocator
	}

	//
	// @todo	Decide whether this should be defined, since it isn't
	//			really thread-safe
	//

	//mapped_type& operator[](const key_type& key_val)
	//{
	//	write_lock_holder_t holder(*this);
	//	return impl_[key_val];
	//}

	this_type& operator=(const this_type& t)
	{
		write_lock_holder_t holder(*this);

		impl_ = t.impl_; 
		return *this; 
	}

	bool operator==(const this_type& t) const
	{ 
		read_lock_holder_t holder(*this);

		return impl_ == t.impl_; 
	}

	/* ---------------------------------------------------------------------------------*\
		non-const iterators
	\* ---------------------------------------------------------------------------------*/

	iterator begin() { 
		read_lock_holder_t holder(*this);

		return impl_.begin(); 
	}

	iterator end() { 
		read_lock_holder_t holder(*this);

		return impl_.end(); 
	}

	reverse_iterator rbegin() { 
		read_lock_holder_t holder(*this);

		return impl_.rbegin(); 
	}

	reverse_iterator rend() { 
		read_lock_holder_t holder(*this);

		return impl_.rend(); 
	}

	/* ---------------------------------------------------------------------------------*\
		const iterators
	\* ---------------------------------------------------------------------------------*/

	const_iterator begin() const { 
		read_lock_holder_t holder(*this);

		return impl_.begin(); 
	}

	const_iterator end() const { 
		read_lock_holder_t holder(*this);

		return impl_.end(); 
	}

	const_reverse_iterator rbegin() const { 
		read_lock_holder_t holder(*this);

		return impl_.rbegin(); 
	}

	const_reverse_iterator rend() const { 
		read_lock_holder_t holder(*this);

		return impl_.rend(); 
	}


	/* ---------------------------------------------------------------------------------*\
		non-modifying methods
	\* ---------------------------------------------------------------------------------*/

	bool empty() const { 
		read_lock_holder_t holder(*this);

		return impl_.empty(); 
	}

	size_type size() const { 
		read_lock_holder_t holder(*this);

		return impl_.size(); 
	}

	size_type count(const key_type& key) const { 
		read_lock_holder_t holder(*this);

		return impl_.count(key); 
	}

	const_iterator find(const key_type& key) const { 
		read_lock_holder_t holder(*this);

		return impl_.find(key); 
	}

	iterator find(const key_type& key) { 
		read_lock_holder_t holder(*this);

		return impl_.find(key); 
	}

	const_iterator lower_bound(const key_type& key) const { 
		read_lock_holder_t holder(*this);

		return impl_.lower_bound(key); 
	}

	iterator lower_bound(const key_type& key) { 
		read_lock_holder_t holder(*this);

		return impl_.lower_bound(key); 
	}

	const_iterator upper_bound(const key_type& key) const { 
		read_lock_holder_t holder(*this);

		return impl_.upper_bound(key); 
	}

	iterator upper_bound(const key_type& key) { 
		read_lock_holder_t holder(*this);

		return impl_.upper_bound(key); 
	}

	std::pair<const_iterator,const_iterator> equal_range(const key_type& key) const { 
		read_lock_holder_t holder(*this);

		return impl_.equal_range(key); 
	}

	std::pair<iterator,iterator> equal_range(const key_type& key) { 
		read_lock_holder_t holder(*this);

		return impl_.equal_range(key); 
	}

	size_type capacity() const { 
		read_lock_holder_t holder(*this);

		return impl_.capacity(); 
	}

	float fill_ratio() const { 
		read_lock_holder_t holder(*this);

		return impl_.fill_ratio(); 
	}

	alloc_t get_allocator() const { 
		read_lock_holder_t holder(*this);

		return impl_.get_allocator();
	}

	/* ---------------------------------------------------------------------------------*\
		modifying methods
	\* ---------------------------------------------------------------------------------*/

	void resize(size_type s) { 
		write_lock_holder_t holder(*this);

		impl_.resize(s); 
	}

	void swap(this_type& t) { 
		write_lock_holder_t holder(*this);

		impl_.swap(t.impl_); 
	}

	std::pair<iterator,bool> insert(const_reference k) {
		write_lock_holder_t holder(*this);

		return impl_.insert(k);
	}


	size_type insert(const value_type* first, const value_type* bound) {
		write_lock_holder_t holder(*this);

		return impl_.insert(first,bound); 
	}

	size_type insert(const_iterator first, const_iterator bound) {
		write_lock_holder_t holder(*this);

		return impl_.insertMaybe(first,bound); 
	}

	size_type erase(const key_t& t) { 
		write_lock_holder_t holder(*this);

		return impl_.erase(t); 
	}

	iterator erase(iterator it) {
		write_lock_holder_t holder(*this);

		return impl_.erase(it);
	}

	iterator erase(iterator it, iterator bound) { 
		write_lock_holder_t holder(*this);

		return impl_.erase(it,bound); 
	}

	void clear() { 
		write_lock_holder_t holder(*this);

		impl_.clear();
	}

private:
	map_t impl_;
};

template<typename key_t,typename value_t,typename cmp_t,typename alloc_t,typename map_t,
		 typename lock_t,typename read_lock_holder_t,typename write_lock_holder_t> inline
void swap(safe_map<key_t,value_t,cmp_t,alloc_t,map_t,lock_t,read_lock_holder_t,write_lock_holder_t>& left,
		  safe_map<key_t,value_t,cmp_t,alloc_t,map_t,lock_t,read_lock_holder_t,write_lock_holder_t>& right)
{
	left.swap(right);
}

//template<typename key_t, typename value_t, typename lock_t=critical_section, 
//		 template<typename> class read_lock_holder_t=lock_holder, 
//		 template<typename> class write_lock_holder_t=lock_holder>
//class safe_map_traits
//{
//public:
//	typedef safe_map<key_t, value_t,std::less<key_t>,std::allocator<std::pair<key_t,value_t> >, 
//					 std::map<key_t,value_t,std::less<key_t>, std::allocator<std::pair<const key_t,value_t> > >, 
//					 lock_t, read_lock_holder_t<lock_t>, write_lock_holder_t<lock_t> >
//			map_type;
//
//};
//
//template<typename key_t, typename value_t, typename lock_t=critical_section, 
//		 template<typename> class read_lock_holder_t=lock_holder, 
//		 template<typename> class write_lock_holder_t=lock_holder>
//class safe_hash_map_traits
//{
//public:
//	typedef safe_map<key_t, value_t,stdext::hash_compare<key_t,std::less<key_t> >,std::allocator<std::pair<const key_t,value_t> >, 
//					 stdext::hash_map<key_t,value_t,stdext::hash_compare<key_t,std::less<key_t> >, 
//					 std::allocator<std::pair<key_t,value_t> > >, lock_t, read_lock_holder_t<lock_t>, 
//					 write_lock_holder_t<lock_t> >					
//			map_type;
//
//};


} // namespace dumbnose
