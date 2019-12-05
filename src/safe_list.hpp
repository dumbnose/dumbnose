#pragma once

#include <list>
#include <dumbnose/critical_section.hpp>
#include <dumbnose/lock.hpp>

namespace dumbnose {

template<typename element_t, typename alloc_t = std::allocator<element_t>,
		 typename list_t=std::list<element_t,alloc_t>,
		 typename lock_t=critical_section, 
		 typename read_lock_holder_t=lock_holder<lock_t>, 
		 typename write_lock_holder_t=read_lock_holder_t 
		>
class safe_list
	: public lock_t
{
public:
	typedef safe_list<element_t,alloc_t,list_t,lock_t,read_lock_holder_t,write_lock_holder_t> this_type;
	typedef list_t						list_type;
	typedef element_t					element_type;

	typedef typename list_t::_Alloc		_Alloc;				
	typedef element_type				_Ty;
	typedef list_type					_Myt;


	typedef typename list_t::allocator_type		allocator_type;
	typedef typename list_t::size_type	size_type;
	/*
	typedef list_t::_Dift				_Dift;
	typedef list_t::difference_type		difference_type;
	typedef list_t::_Tptr				_Tptr;
	typedef list_t::_Ctptr				_Ctptr;
	typedef list_t::pointer				pointer;
	typedef list_t::const_pointer		const_pointer;
	typedef list_t::_Reft				_Reft;
	*/
	typedef typename list_t::reference			reference;
	typedef typename list_t::const_reference		const_reference;
//	typedef list_t::value_type			value_type;

		// CLASS const_iterator
	// class const_iterator;
	// friend class const_iterator;

	/*
	TODO: iterator stuff goes here
	*/
	typedef typename list_t::iterator					iterator;
	typedef typename list_t::reverse_iterator			reverse_iterator;
	typedef typename list_t::const_iterator				const_iterator;
	typedef typename list_t::const_reverse_iterator		const_reverse_iterator;

	typedef	write_lock_holder_t							write_lock_holder;

	/* ---------------------------------------------------------------------------------*\
		CTOR/DTOR
	\* ---------------------------------------------------------------------------------*/
	safe_list()
		: impl_()
		{	// construct empty list
		}

	explicit safe_list(const _Alloc& _Al)
		: impl_(_Al)
		{	// construct empty list, allocator
		}

	explicit safe_list(size_type _Count)
		: impl_(_Count)
		{	// construct list from _Count * _Ty()
		}

	safe_list(size_type _Count, const _Ty& _Val)
		: impl_(_Count, _Val)
		{	// construct list from _Count * _Val
		}

	safe_list(size_type _Count, const _Ty& _Val, const _Alloc& _Al)
		: impl_(_Count, _Val, _Al)
		{	// construct list, allocator from _Count * _Val
		}

	safe_list(const _Myt& _Right)
		: impl_(_Right)
		{	// construct list by copying _Right
		}

	template<class _Iter>
		safe_list(_Iter _First, _Iter _Last)
		: impl_(_First, _Last)
		{	// construct list from [_First, _Last)
		}

	template<class _Iter>
		safe_list(_Iter _First, _Iter _Last, const _Alloc& _Al)
		: impl_(_First, _Last, _Al)
		{	// construct list, allocator from [_First, _Last)
		}

	~safe_list()
		{
		}

	this_type& operator=(const this_type& t)
	{
		write_lock_holder_t holder(*this);

		impl_ = t.impl_; 
		return *this; 
	}

	/* ---------------------------------------------------------------------------------*\
		stack-based lock
	\* ---------------------------------------------------------------------------------*/
	write_lock_holder lock()
	{
		return write_lock_holder_t(*this);
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
	size_type size() // const
		{	// return length of sequence
		write_lock_holder_t holder(*this);
		return impl_.size();
		}

	size_type max_size() // const
		{	// return maximum possible length of sequence
		write_lock_holder_t holder(*this);
		return impl_.max_size();
		}

	bool empty() // const
		{	// test if sequence is empty
		write_lock_holder_t holder(*this);
		return impl_.empty();
		}

	allocator_type get_allocator() // const
		{	// return allocator object for values
		write_lock_holder_t holder(*this);
		return impl_.get_allocator();
		}

	const_reference front() const
		{	// return first element of nonmutable sequence
		write_lock_holder_t holder(*this);
		return impl_.front();
		}

	const_reference back() const
		{	// return last element of nonmutable sequence
		write_lock_holder_t holder(*this);
		return impl_.back();
		}

	/* ---------------------------------------------------------------------------------*\
		modifying methods
	\* ---------------------------------------------------------------------------------*/
	void resize(size_type _Newsize)
		{	// determine new length, padding with _Ty() elements as needed
		write_lock_holder_t holder(*this);
		impl_.resize(_Newsize);
		}

	void resize(size_type _Newsize, _Ty _Val)
		{	// determine new length, padding with _Val elements as needed
		write_lock_holder_t holder(*this);
		impl_.resize(_Newsize, _Val);
		}

	reference front()
		{	// return first element of mutable sequence
		write_lock_holder_t holder(*this);
		return impl_.front();
		}

	reference back()
		{	// return last element of mutable sequence
		write_lock_holder_t holder(*this);
		return impl_.back();
		}

	void push_front(const _Ty& _Val)
		{	// insert element at beginning
		write_lock_holder_t holder(*this);
		return impl_.push_front(_Val);
		}

	void pop_front()
		{	// erase element at beginning
		write_lock_holder_t holder(*this);
		return impl_.pop_front();
		}

	void push_back(const _Ty& _Val)
		{	// insert element at end
		write_lock_holder_t holder(*this);
		return impl_.push_back(_Val);
		}

	void pop_back()
		{	// erase element at end
		write_lock_holder_t holder(*this);
		return impl_.pop_back();
		}

	template<class _Iter>
		void assign(_Iter _First, _Iter _Last)
		{	// assign [_First, _Last)
		write_lock_holder_t holder(*this);
		impl_.assign(_First, _Last);
		}

	void assign(size_type _Count, const _Ty& _Val)
		{	// assign _Count * _Val
		write_lock_holder_t holder(*this);
		impl_.assign(_First, _Last, _Val);
		}

	iterator insert(iterator _Where, const _Ty& _Val)
		{	// insert _Val at _Where
		write_lock_holder_t holder(*this);
		return (impl_.insert(_Where,_Val));
		}

	void insert(iterator _Where, size_type _Count, const _Ty& _Val)
		{	// insert _Count * _Val at _Where
		write_lock_holder_t holder(*this);
		impl.insert(_Where, _Count, _Val);
		}

	template<class _Iter>
		void insert(iterator _Where, _Iter _First, _Iter _Last)
		{	// insert [_First, _Last) at _Where
		write_lock_holder_t holder(*this);
		impl_.insert(_Where, _First, _Last);
		}

	iterator erase(iterator _Where)
		{	// erase element at _Where
		write_lock_holder_t holder(*this);
		return impl_.erase(_Where);
		}

	iterator erase(iterator _First, iterator _Last)
		{	// erase [_First, _Last)
		write_lock_holder_t holder(*this);
		return impl_.erase(_First,_Last);
		}

	void clear()
		{	// erase all
		write_lock_holder_t holder(*this);
		impl_.clear();
		}

	void swap(_Myt& _Right)
		{	// exchange contents with _Right
		write_lock_holder_t holder(*this);
		impl_.swap(_Right);
		}

	void splice(iterator _Where, _Myt& _Right)
		{	// splice all of _Right at _Where
		write_lock_holder_t holder(*this);
		impl_.splice(_Where,_Right);
		}

	void splice(iterator _Where, _Myt& _Right, iterator _First)
		{	// splice _Right [_First, _First + 1) at _Where
		write_lock_holder_t holder(*this);
		impl_.splice(_Where,_Right,_First);
		}

	void splice(iterator _Where,
		_Myt& _Right, iterator _First, iterator _Last)
		{	// splice _Right [_First, _Last) at _Where
		write_lock_holder_t holder(*this);
		impl_.splice(_Where,_Right,_First,_Last);
		}

	void remove(const _Ty& _Val)
		{	// erase each element matching _Val
		write_lock_holder_t holder(*this);
		impl_.remove(_Val);
		}

	template<class _Pr1>
		void remove_if(_Pr1 _Pred)
		{	// erase each element satisfying _Pr1
		write_lock_holder_t holder(*this);
		impl_.remove(_Val);
		}

	void unique()
		{	// erase each element matching previous
		write_lock_holder_t holder(*this);
		impl_.unique();
		}

	template<class _Pr2>
		void unique(_Pr2 _Pred)
		{	// erase each element satisfying _Pred with previous
		write_lock_holder_t holder(*this);
		impl_.unique(_Pred);
		}

	void merge(_Myt& _Right)
		{	// merge in elements from _Right, both ordered by operator<
		write_lock_holder_t holder(*this);
		impl_.merge(_Right);
		}

	template<class _Pr3>
		void merge(_Myt& _Right, _Pr3 _Pred)
		{	// merge in elements from _Right, both ordered by _Pred
		write_lock_holder_t holder(*this);
		impl_.merge(_Right, _Pred);
		}

	void sort()
		{	// order sequence, using operator<
		write_lock_holder_t holder(*this);
		impl_.sort();
		}

	template<class _Pr3>
		void sort(_Pr3 _Pred)
		{	// order sequence, using _Pred
		write_lock_holder_t holder(*this);
		impl_.sort(_Pred);
		}

	void reverse()
		{	// reverse sequence
		write_lock_holder_t holder(*this);
		impl_.reverse();
		}

protected:
	list_t	impl_;
};


} // namespace dumbnose
