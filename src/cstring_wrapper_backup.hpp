#pragma once

#pragma warning(push)
#pragma warning (disable : 4786)


#include <iterator>
#include <string>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <assert.h>


namespace dumbnose {


template<class elem_t,
class traits_t = std::char_traits<elem_t>,
class alloc_t = std::allocator<elem_t> >
class cstring_wrapper_t {

public:
	// types:
	typedef typename std::char_traits<elem_t>		traits_type;
	typedef typename alloc_t						allocator_type;
	typedef typename elem_t							value_type;
	typedef typename size_t							size_type;
	typedef typename ptrdiff_t						difference_type;
	typedef typename elem_t&						reference;
	typedef typename const elem_t&					const_reference;
	typedef typename elem_t*						pointer;
	typedef typename const elem_t*					const_pointer;

	typedef std::basic_string<elem_t> string_type;

	typedef pointer const_iterator;
	typedef pointer iterator;
	typedef pointer reverse_iterator;
	typedef pointer const_reverse_iterator;

	enum { npos = size_type (-1) };

	// construct/copy/destroy:
	explicit cstring_wrapper_t() : str_(L"") {}
	cstring_wrapper_t (const elem_t* s) : str_ (s) { assert (s); }
	~cstring_wrapper_t() {}

	//cstring_wrapper_t& operator= (const cstring_wrapper_t& str){ imp = str.imp; return *this; }
	//cstring_wrapper_t& operator= (const elem_t* s)    { assert (s); imp = s; return *this; }
	//cstring_wrapper_t& operator= (elem_t c)           { imp = c; return *this; }

	// iterators:
	iterator       begin()                { return str_; }
	const_iterator begin() const          { return str_; }
	iterator       end()                  { return str_ + this.size(); }
	const_iterator end() const            { return str_ + this.size(); }
	//reverse_iterator       rbegin()       { return end()-1; }
	//const_reverse_iterator rbegin() const { return end()-1; }
	//reverse_iterator       rend()         { return begin()-1; }
	//const_reverse_iterator rend() const   { return begin()-1; }

	// capacity:
	size_type size() const             { return traits_type.length(str_); }
	size_type length() const           { return traits_type.length(str_); }
	size_type max_size() const         { return traits_type.length(str_); }
	//void resize (size_type n, elem_t c) { imp.resize (n, c); }
	//void resize (size_type n)          { imp.resize (n); }
	size_type capacity() const         { return traits_type.length(str_); }
	//void reserve (size_type n = 0)     { imp.reserve (n); }
	//void clear()                       { imp.erase (base_iter(begin()), base_iter(end())); } // no clear() in MSVC++ 6.0
	bool empty() const                 { return traits_type.length(str_)==0; }

	// element access:
	reference       operator[] (size_type pos)       { return str_[pos]; }
	const_reference operator[] (size_type pos) const { return str_[pos]; }
	reference       at (size_type n)                 { return str_[n]; }
	const_reference at (size_type n) const           { return str_[n]; }


	// modifiers:
	//cstring_wrapper_t& operator+= (const cstring_wrapper_t& str) { imp += (str.imp); return *this; }
	//cstring_wrapper_t& operator+= (const elem_t* s)     { assert (s); imp += (s); return *this; }
	//cstring_wrapper_t& operator+= (elem_t c)            { imp += (c); return *this; }
	//cstring_wrapper_t& append (const cstring_wrapper_t& str)     { imp.append (str.imp); return *this; }
	//cstring_wrapper_t& append (const cstring_wrapper_t& str, size_type pos, size_type n) { imp.append (str.imp, pos, n); return *this; }
	//cstring_wrapper_t& append (const elem_t* s, size_type n) { assert (s); imp.append (s, n); return *this; }
	//cstring_wrapper_t& append (const elem_t* s)              { assert (s); imp.append (s); return *this; }
	//cstring_wrapper_t& append (size_type n, elem_t c)        { imp.append (n, c); return *this; }
	//template<class InputIterator>
	//cstring_wrapper_t& append (InputIterator first, InputIterator last) { imp.append (first, last); return *this; }
	//void push_back (const elem_t c)         { imp += (c); } // // no push_back() in MSVC++ 6.0

	//cstring_wrapper_t& assign (const cstring_wrapper_t& str)   { imp.assign (str.imp); return *this; }
	//cstring_wrapper_t& assign (const cstring_wrapper_t& str, size_type pos, size_type n)
	//{ imp.assign (str.imp, pos, n); return *this; }
	//cstring_wrapper_t& assign (const elem_t* s, size_type n) { assert (s); imp.assign (s, n); return *this; }
	//cstring_wrapper_t& assign (const elem_t* s)              { assert (s); imp.assign (s); return *this; }
	//cstring_wrapper_t& assign (size_type n, elem_t c)        { imp.assign (n, c); return *this; }
	//template<class InputIterator>
	//cstring_wrapper_t& assign (InputIterator first, InputIterator last)
	//{ imp.assign (first, last); return *this; }

	//cstring_wrapper_t& insert (size_type pos1, const cstring_wrapper_t& str)
	//{ imp.insert (pos1, str.imp); return *this; }
	//cstring_wrapper_t& insert (size_type pos1, const cstring_wrapper_t& str, size_type pos2, size_type n)
	//{ imp.insert (pos1, str.imp, pos2, n); return *this; }
	//cstring_wrapper_t& insert (size_type pos, const elem_t* s, size_type n)
	//{ assert (s); imp.insert (pos, s, n); return *this; }
	//cstring_wrapper_t& insert (size_type pos, const elem_t* s)
	//{ assert (s); imp.insert (pos, s); return *this; }
	//cstring_wrapper_t& insert (size_type pos, size_type n, elem_t c)
	//{ imp.insert (pos, n, c); return *this; }
	//iterator insert (iterator p, elem_t c)
	//{ return imp.insert (base_iter (p), c); }
	//void insert (iterator p, size_type n, elem_t c)
	//{ imp.insert (base_iter (p), n, c); }
	//template<class InputIterator>
	//void insert (iterator p, InputIterator first, InputIterator last)
	//{ imp.insert (base_iter (p), first, last); }

	//cstring_wrapper_t& erase (size_type pos = 0, size_type n = npos)
	//{ imp.erase (pos, n); return *this; }
	//iterator erase (iterator position)
	//{ return imp.erase (base_iter (position)); }
	//iterator erase (iterator first, iterator last)
	//{ return imp.erase (base_iter (first), base_iter (last)); }
	//cstring_wrapper_t& replace (size_type pos1, size_type n1, const cstring_wrapper_t& str)
	//{ imp.replace (pos1, n1, str.imp); return *this; }
	//cstring_wrapper_t& replace (size_type pos1, size_type n1, const cstring_wrapper_t& str, size_type pos2, size_type n2)
	//{ imp.replace (pos1, n1, str.imp, pos2, n2); return *this; }
	//cstring_wrapper_t& replace (size_type pos, size_type n1, const elem_t* s, size_type n2)
	//{ assert (s); imp.replace (pos, n1, s, n2); return *this; }
	//cstring_wrapper_t& replace (size_type pos, size_type n1, const elem_t* s)
	//{ assert (s); imp.replace (pos, n1, s); return *this; }
	//cstring_wrapper_t& replace (size_type pos, size_type n1, size_type n2, elem_t c)
	//{ imp.replace (pos, n1, n2, c); return *this; }
	//cstring_wrapper_t& replace (iterator i1, iterator i2, const cstring_wrapper_t& str)
	//{ imp.replace (base_iter (i1), base_iter (i2), str.imp); return *this; }
	//cstring_wrapper_t& replace (iterator i1, iterator i2, const elem_t* s, size_type n)
	//{ assert (s); imp.replace (base_iter (i1), base_iter (i2), s, n); return *this; }
	//cstring_wrapper_t& replace (iterator i1, iterator i2, const elem_t* s)
	//{ assert (s); imp.replace (base_iter (i1), base_iter (i2), s); return *this; }
	//cstring_wrapper_t& replace (iterator i1, iterator i2, size_type n, elem_t c)
	//{ imp.replace (base_iter (i1), base_iter (i2), n, c); return *this; }
	//template<class InputIterator>
	//cstring_wrapper_t& replace (iterator i1, iterator i2,InputIterator j1, InputIterator j2)
	//{ imp.replace (base_iter (i1), base_iter (i2), j1, j2); return *this; }
	//size_type copy (elem_t* s, size_type n, size_type pos = 0) const
	//{ assert (s); return imp.copy (s, n, pos); }


	void swap (cstring_wrapper_t& str)     { std::swap(str_,str.str_); }
	friend inline void swap (cstring_wrapper_t& lhs, cstring_wrapper_t& rhs)     { lhs.swap (rhs); }

	// additional swap functions to exchange string contents with std::string (std::basic_string);
	//void swap (string_type& str) { imp.swap (str); }
	//friend inline void swap (cstring_wrapper_t& lhs, string_type& rhs) { lhs.imp.swap (rhs); }
	//friend inline void swap (string_type& lhs, cstring_wrapper_t& rhs) { lhs.swap (rhs.imp); }

	// string operations:
	const elem_t* c_str() const { return str_; } 
	const elem_t* data() const  { return str_; }

	allocator_type get_allocator() const { return allocator(); }

	//size_type find (const cstring_wrapper_t& str, size_type pos = 0) const
	//{ return imp.find (str.imp, pos); }
	//size_type find (const elem_t* s, size_type pos, size_type n) const
	//{ assert (s); return imp.find (s, pos, n); }
	//size_type find (const elem_t* s, size_type pos = 0) const
	//{ assert (s); return imp.find (s, pos); }
	//size_type find (elem_t c, size_type pos = 0) const
	//{ return imp.find (c, pos); }
	//size_type rfind (const cstring_wrapper_t& str, size_type pos = npos) const
	//{ return imp.rfind (str.imp, pos); }
	//size_type rfind (const elem_t* s, size_type pos, size_type n) const
	//{ assert (s); return imp.rfind (s, pos, n); }
	//size_type rfind (const elem_t* s, size_type pos = npos) const
	//{ assert (s); return imp.rfind (s, pos); }
	//size_type rfind (elem_t c, size_type pos = npos) const
	//{ return imp.rfind (c, pos); }

	//size_type find_first_of (const cstring_wrapper_t& str, size_type pos = 0) const
	//{ return imp.find_first_of (str.imp, pos); }
	//size_type find_first_of (const elem_t* s, size_type pos, size_type n) const
	//{ assert (s); return imp.find_first_of (s, pos, n); }
	//size_type find_first_of (const elem_t* s, size_type pos = 0) const
	//{ assert (s); return imp.find_first_of (s, pos); }
	//size_type find_first_of (elem_t c, size_type pos = 0) const
	//{ return imp.find_first_of (c, pos); }

	//size_type find_last_of (const cstring_wrapper_t& str, size_type pos = npos) const
	//{ return imp.find_last_of (str.imp, pos); }
	//size_type find_last_of (const elem_t* s, size_type pos, size_type n) const
	//{ assert (s); return imp.find_last_of (s, pos, n); }
	//size_type find_last_of (const elem_t* s, size_type pos = npos) const
	//{ assert (s); return imp.find_last_of (s, pos); }
	//size_type find_last_of (elem_t c, size_type pos = npos) const
	//{ return imp.find_last_of (c, pos); }

	//size_type find_first_not_of (const cstring_wrapper_t& str, size_type pos = 0) const
	//{ return imp.find_first_not_of (str.imp, pos); }
	//size_type find_first_not_of (const elem_t* s, size_type pos, size_type n) const
	//{ assert (s); return imp.find_first_not_of (s, pos, n); }
	//size_type find_first_not_of (const elem_t* s, size_type pos = 0) const
	//{ assert (s); return imp.find_first_not_of (s, pos); }
	//size_type find_first_not_of (elem_t c, size_type pos = 0) const
	//{ return imp.find_first_not_of (c, pos); }

	//size_type find_last_not_of (const cstring_wrapper_t& str, size_type pos = npos) const
	//{ return imp.find_last_not_of (str.imp, pos); }
	//size_type find_last_not_of (const elem_t* s, size_type pos, size_type n) const
	//{ assert (s); return imp.find_last_not_of (s, pos, n); }
	//size_type find_last_not_of (const elem_t* s, size_type pos = npos) const
	//{ assert (s); return imp.find_last_not_of (s, pos); }
	//size_type find_last_not_of (elem_t c, size_type pos = npos) const
	//{ return imp.find_last_not_of (c, pos); }

	//cstring_wrapper_t substr (size_type pos = 0, size_type n = npos) const  {
	//	if (pos <= size()) { // see C++ Standard 21.3.6.7
	//		size_type rlen = n < size()-pos ? n : size()-pos;
	//		return cstring_wrapper_t (c_str() + pos, rlen);
	//	} else {
	//		throw std::out_of_range("cstring_wrapper_t::substr");
	//	}
	//}

	//int compare (const cstring_wrapper_t& str) const
	//{ return imp.compare (str.imp); }
	//int compare (size_type pos1, size_type n1, const cstring_wrapper_t& str) const
	//{ return imp.compare (pos1, n1, str.imp); }
	//int compare (size_type pos1, size_type n1, const cstring_wrapper_t& str, size_type pos2, size_type n2) const
	//{ return imp.compare (pos1, n1, str.imp, pos2, n2); }
	//int compare (const elem_t* s) const
	//{ assert (s); return imp.compare (s); }
	//int compare (size_type pos1, size_type n1, const elem_t* s, size_type n2 = npos) const
	//{ assert (s); return imp.compare (pos1, n1, s, n2); }

	//friend inline cstring_wrapper_t operator+ (const cstring_wrapper_t& lhs, const cstring_wrapper_t& rhs) {
	//	return cstring_wrapper_t (lhs, rhs, false);
	//}
	//friend inline cstring_wrapper_t operator+ (const elem_t* lhs, const cstring_wrapper_t& rhs) {
	//	assert (lhs);
	//	return cstring_wrapper_t (lhs, rhs);
	//}
	//friend inline cstring_wrapper_t operator+ (elem_t lhs, const cstring_wrapper_t& rhs) {
	//	return cstring_wrapper_t (lhs, rhs);
	//}
	//friend inline cstring_wrapper_t operator+ (const cstring_wrapper_t& lhs, const elem_t* rhs) {
	//	assert (rhs);
	//	return cstring_wrapper_t (lhs, rhs);
	//}
	//friend inline cstring_wrapper_t operator+ (const cstring_wrapper_t& lhs, elem_t rhs) {
	//	return cstring_wrapper_t (lhs, rhs);
	//}

	//friend inline bool operator== (const cstring_wrapper_t& lhs, const cstring_wrapper_t& rhs) { return lhs.imp == rhs.imp; }
	//friend inline bool operator== (const elem_t* lhs, const cstring_wrapper_t& rhs) {
	//	assert (lhs);
	//	return lhs == rhs.imp;
	//}
	//friend inline bool operator== (const cstring_wrapper_t& lhs, const elem_t* rhs) {
	//	assert (rhs);
	//	return lhs.imp == rhs;
	//}
	//friend inline bool operator<  (const cstring_wrapper_t& lhs, const cstring_wrapper_t& rhs) { return lhs.imp < rhs.imp; }
	//friend inline bool operator<  (const cstring_wrapper_t& lhs, const elem_t* rhs) {
	//	assert (rhs);
	//	return lhs.imp < rhs;
	//}
	//friend inline bool operator<  (const elem_t* lhs, const cstring_wrapper_t& rhs) {
	//	assert (lhs);
	//	return lhs < rhs.imp;
	//}
	//friend inline bool operator!= (const cstring_wrapper_t& lhs, const cstring_wrapper_t& rhs) { return !(lhs == rhs); }
	//friend inline bool operator!= (const elem_t* lhs,   const cstring_wrapper_t& rhs) { return !(lhs == rhs); }
	//friend inline bool operator!= (const cstring_wrapper_t& lhs, const elem_t* rhs)   { return !(lhs == rhs); }
	//friend inline bool operator>  (const cstring_wrapper_t& lhs, const cstring_wrapper_t& rhs) { return rhs < lhs; }
	//friend inline bool operator>  (const cstring_wrapper_t& lhs, const elem_t* rhs)   { return rhs < lhs; }
	//friend inline bool operator>  (const elem_t* lhs,   const cstring_wrapper_t& rhs) { return rhs < lhs; }
	//friend inline bool operator<= (const cstring_wrapper_t& lhs, const cstring_wrapper_t& rhs) { return !(rhs < lhs); }
	//friend inline bool operator<= (const cstring_wrapper_t& lhs, const elem_t* rhs)   { return !(rhs < lhs); }
	//friend inline bool operator<= (const elem_t* lhs,   const cstring_wrapper_t& rhs) { return !(rhs < lhs); }
	//friend inline bool operator>= (const cstring_wrapper_t& lhs, const cstring_wrapper_t& rhs) { return !(lhs < rhs); }
	//friend inline bool operator>= (const cstring_wrapper_t& lhs, const elem_t* rhs)   { return !(lhs < rhs); }
	//friend inline bool operator>= (const elem_t* lhs,   const cstring_wrapper_t& rhs) { return !(lhs < rhs); }

	friend inline std::basic_ostream<elem_t, typename string_type::traits_type>&
		operator<< (std::basic_ostream<elem_t, typename string_type::traits_type>& os, const cstring_wrapper_t& str) {
			return os << str.str_;
	}

	friend inline std::basic_istream <elem_t, typename string_type::traits_type>&
		operator>> (std::basic_istream<elem_t, typename string_type::traits_type>& is, cstring_wrapper_t& str) {
			is >> str.str_;
			return is;
	}

	friend inline std::basic_istream<elem_t, typename string_type::traits_type>&
		getline(std::basic_istream<elem_t, typename string_type::traits_type>& is, cstring_wrapper_t& str, elem_t delim) {
			std::getline (is, str.str_, delim);
			return is;
	}

	friend inline std::basic_istream<elem_t, typename string_type::traits_type>&
		getline(std::basic_istream<elem_t, typename string_type::traits_type>& is, cstring_wrapper_t& str) {
			return getline (is, str, is.widen('\n'));
	}

private:
	// private constructors, used only to facilitate RVO for operator+() 

	// the third argument is not used, it disambiguates parsing for older compilers
	//explicit cstring_wrapper_t (const cstring_wrapper_t& lhs, const cstring_wrapper_t& rhs, bool/*not used*/) {
	//	imp.reserve (lhs.length() + rhs.length());
	//	imp.append (lhs.imp).append (rhs.imp);
	//}
	//explicit cstring_wrapper_t (const elem_t* lhs, const cstring_wrapper_t& rhs) {
	//	assert (lhs);
	//	size_type lhsLen = std::char_traits<elem_t>::length (lhs);
	//	imp.reserve (lhsLen + rhs.length());
	//	imp.append (lhs, lhsLen).append (rhs.imp);
	//}
	//explicit cstring_wrapper_t (elem_t lhs, const cstring_wrapper_t& rhs) {
	//	imp.reserve (1 + rhs.length());
	//	imp.append (1, lhs).append (rhs.imp);
	//}
	//explicit cstring_wrapper_t (const cstring_wrapper_t& lhs, const elem_t* rhs) {
	//	assert (rhs);
	//	size_type rhsLen = std::char_traits<elem_t>::length (rhs);
	//	imp.reserve (lhs.length() + rhsLen);
	//	imp.append (lhs.imp).append (rhs, rhsLen);
	//}
	//explicit cstring_wrapper_t (const cstring_wrapper_t& lhs, elem_t rhs) {
	//	imp.reserve (lhs.length() + 1);
	//	imp.append (lhs.imp).append (1, rhs);
	//}

//	typename string_type::iterator base_iter (iterator iter)  { 
//#if 0 // for basic_string<...>::iterator 
//		return iter; 
//#else // for new cstring_wrapper_t::iterator
//		return iter.iterImp; 
//#endif
//	}

	//string_type imp;
	const_pointer str_;

};


typedef cstring_wrapper_t<char> cstring_wrapper;
typedef cstring_wrapper_t<wchar_t> cwstring_wrapper;


} // namespace dumbnose 


#pragma warning(pop)

