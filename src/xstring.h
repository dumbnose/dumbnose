// Copyright (c) 2001 John Panzer
// Permission is granted to use this code without restriction as
// long as this copyright notice appears in all source files.
// Portions of interface Copyright (c) 1997-1999
//    Silicon Graphics Computer Systems, Inc.

#ifndef XSTRING_H_INCLUDED
#define XSTRING_H_INCLUDED

#include <ctype.h>
#include <stddef.h>
#include <iterator>
#include <stdexcept>
#include <memory>
#include <algorithm>
#include <string> // For char_traits
#include <functional>

#include "xs_defs.h"

XS_NAMESPACE(xstr)

//----------------------------------------------------
// Helper classes:

// Helper templates that wrap char_traits functions within
// STL predicate functors.  This lets the string class use
// standard algorithms for much of its work.

template <class Traits>
struct eq_traits
  : public std::binary_function<typename Traits::char_type,
                                typename Traits::char_type,
                                bool> {
    bool operator()(const typename Traits::char_type& x,
                  const typename Traits::char_type& y) const { 
        return Traits::eq(x, y); 
    }
};

template <class Traits>
struct not_within_traits
  : public std::unary_function<typename Traits::char_type, bool>
{
  typedef typename Traits::char_type CharT;
  const CharT* _first;
  const CharT* _last;

  not_within_traits(const typename Traits::char_type* f,
                     const typename Traits::char_type* l)
    : _first(f), _last(l) {}

  bool operator()(const typename Traits::char_type& x) const {
    return find_if((CharT*)_first, (CharT*)_last,
                   bind1st(eq_traits<Traits>(), x)) == (CharT*)_last;
  }
};


// Helper template IsInteger: Does compile time type
// checking to tell if template type is an integer
// type (query is true_type) or something else
// (query is false_type).  This is used for compile
// time dispatching of otherwise ambiguous overloads.
                        
struct true_type {};
struct false_type {};

template <class T>
struct IsInteger {typedef false_type query;};

template <> struct IsInteger<char> {typedef true_type query;};
template <> struct IsInteger<unsigned char> {typedef true_type query;};
template <> struct IsInteger<short> {typedef true_type query;};
template <> struct IsInteger<unsigned short> {typedef true_type query;};
template <> struct IsInteger<int> {typedef true_type query;};
template <> struct IsInteger<unsigned> {typedef true_type query;};
template <> struct IsInteger<long> {typedef true_type query;};
template <> struct IsInteger<unsigned long> {typedef true_type query;};


//----------------------------------------------------

// Wrapper that adds extended string interface on top of
// a minimal string implementation. Impl must
// provide a minimal random access string container interface.

template <class Impl>
class xstring : public Impl {
public:
    // Standard typedefs, taken from implementation:
    typedef typename Impl::value_type value_type;
    typedef typename Impl::traits_type traits_type;

    typedef typename Impl::pointer pointer;
    typedef typename Impl::const_pointer const_pointer;
    typedef typename Impl::reference reference;
    typedef typename Impl::const_reference const_reference;
    typedef typename Impl::size_type size_type;
    typedef typename Impl::difference_type difference_type;

    typedef typename Impl::const_iterator const_iterator;
    typedef typename Impl::iterator iterator;

    typedef typename Impl::reverse_iterator reverse_iterator;
    typedef typename Impl::const_reverse_iterator const_reverse_iterator;

    typedef typename Impl::allocator_type allocator_type;

    // --------- Constructors ----------

    xstring(allocator_type const &allocator = allocator_type()) 
        : Impl(allocator) {
        Impl::element_initialize(0,0);
    }

    xstring(xstring const &s) : Impl(s) {}

    // Conversion from basic_string:
    xstring(std::basic_string<value_type> const &s) {
        Impl::range_initialize(s.begin(), s.end());
    }

    xstring(xstring const &s, 
            typename Impl::size_type pos, 
            typename Impl::size_type n = npos,
            const typename Impl::allocator_type& a = allocator_type()) 
        : Impl(a) {
        if (pos > s.size()) 
            throw std::out_of_range("Out of range");
        Impl::range_initialize(s.begin() + pos,
                          s.begin() + pos + std::min(n,s.size() - pos));
    }

    xstring(const value_type* s, size_type n=(size_type)(-1),
            const allocator_type& a = _default_allocator);

    xstring(size_type n, value_type c, 
               const allocator_type& a = _default_allocator) 
        : Impl(a) {
        Impl::element_initialize(n,c);
    }

    template <class PossibleInputIter>
    xstring(PossibleInputIter f, PossibleInputIter l, 
               const allocator_type& a = _default_allocator) 
        : Impl(a) {
        typedef typename IsInteger<PossibleInputIter>::query is_integer;
        assign_dispatch(f, l, is_integer());
    }

private: // Dispatched implementation of assignment
    template <class Integer>
    void assign_dispatch(Integer n, Integer x, true_type) {
        Impl::element_initialize(n,(value_type)x);
    }

    template <class InputIter>
    void assign_dispatch(InputIter f, InputIter l, false_type) {
        Impl::range_initialize(f, l);
    }
public:

    // Conversion to basic_string<value_type>.  This will only
    // work if everyone is using the default char_traits.
    operator std::basic_string<value_type>() {
        return std::basic_string<value_type>(this->c_str());
    }

    size_type length() const {return size();}
 
    reverse_iterator rbegin()             
        { return reverse_iterator(end()); }
    reverse_iterator rend()               
        { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const 
        { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const 
        { return const_reverse_iterator(begin()); }

    void resize(size_type n, value_type c = value_type()) {
      if (n <= size())
          erase(begin() + n, end());
      else
          append(n - size(), c);
    }

    void push_back(value_type c) {
        insert(end(),c);
    }

    void pop_back() {
        erase(end()-1, end());
    }

    // Assignment

    template <class PossibleInputIter>
    xstring &assign(PossibleInputIter first, PossibleInputIter last) {
        clear();
        typedef typename IsInteger<PossibleInputIter>::query is_integer;
        assign_dispatch(first, last, is_integer());
        return *this;
    }

    xstring &operator=(const xstring &s) {
        if (&s != this)
            assign(s.begin(), s.end());
        return *this;
    }

    xstring& operator=(const value_type* s) { 
        assign_dispatch(s, s + traits_type::size(s), false_type());
        return *this;
    }

    xstring& operator=(value_type c) { 
        assign_dispatch(static_cast<size_type>(1), 
                        static_cast<size_type>(c), 
                        true_type());
        return *this;
    }

    // Append

    xstring& operator+=(const xstring& s) { return append(s); }
    xstring& operator+=(const value_type* s) { return append(s); }
    xstring& operator+=(value_type c) { push_back(c); return *this; }

    template <class InputIter>
    xstring &append(InputIter first, InputIter last) {
        insert(begin()+size(), first, last);
        return *this;
    }

    xstring& append(const xstring& s) 
    { return append(s.begin(), s.end()); }

    xstring& append(const xstring& s,
                    size_type pos, size_type n) {
        if (pos > s.size())
            throw std::out_of_range("Out of range");
        return append(s.begin() + pos,
                  s.begin() + pos + std::min(n, s.size() - pos));
    }

    xstring& append(const value_type* s, size_type n) 
    { return append(s, s+n); }

    xstring& append(const value_type* s) 
    { return append(s, s + traits_type::length(s)); }

    xstring& append(size_type n, value_type c)
    { return insert(size(), n, c); }

    // Insertion

    // Need to define these just so we can overload
    // the function in this class.
    iterator insert(iterator p, value_type c) {
        insert<value_type*>(p,&c,1+&c);
        return ++p;
    }

    template <class InputIter>
    void insert(iterator pos, InputIter first, InputIter last) {
        Impl::insert(pos,first,last);
    }    

    xstring& insert(size_type pos, const xstring& s) {
        if (pos > size())
            throw std::out_of_range("Out of range");
        if (size() > max_size() - s.size())
            throw std::length_error("Length error");
        insert(begin() + pos, s.begin(), s.end());
        return *this;
    }

    xstring& insert(size_type pos, const xstring& s,
                    size_type beg, size_type n) {
        if (pos > size() || beg > s.size())
            throw std::out_of_range("Out of range");
        size_type len = std::min(n, s.size() - beg);             
        if (size() > max_size() - len)
            throw std::out_of_range("Out of range");
        insert(begin() + pos,
               s.begin() + beg, s.begin() + beg + len);
        return *this;
    }

    xstring& insert(size_type pos, const value_type* s, size_type n) {
        if (pos > size())
            throw std::out_of_range("Out of range");
        if (size() > max_size() - n)
            throw std::out_of_range("Out of range");
        insert(begin() + pos, s, s + n);
        return *this;
    }

    xstring& insert(size_type pos, const value_type* s) {
        if (pos > size())
            throw std::out_of_range("Out of range");
        size_type len = traits_type::length(s);
        if (size() > max_size() - len)
            throw std::out_of_range("Out of range");
        insert(begin() + pos, s, s + len);
        return *this;
    }

    xstring& insert(size_type pos, size_type n, value_type c) {
        if (pos > size())
            throw std::out_of_range("Out of range");
        if (size() > max_size() - n)
            throw std::out_of_range("Out of range");
        // TODO: Replace with less brain-dead implementation.
        for (size_type i=0;i<n;++i)
           insert(begin()+pos+i, c);
        return *this;
    }

    xstring& insert(iterator p, size_type n, value_type c) {
        return insert(p-begin(), n, c);
    }

    // Erase
    iterator erase(iterator first, iterator last) {
        return Impl::erase(first,last);
    }

    xstring& erase(size_type pos = 0, size_type n = npos) {
        if (pos > size())
            throw std::out_of_range("Out of range");
        erase(begin() + pos,
              begin() + std::min(n, size() - pos));
        return *this;
    }
    iterator erase(iterator pos) {return erase(pos,pos+1);}

    // Replace
    xstring& replace(size_type pos, size_type n, 
                        const xstring& s) {
        if (pos > size())
            throw std::out_of_range("Out of range");
        const size_type len = std::min(n, size() - pos);
        if (size() - len >= max_size() - s.size())
            throw std::length_error("Too long");
        return replace(begin() + pos, begin() + pos + len, 
                       s.begin(), s.end());
    }

    xstring& replace(size_type pos1, size_type n1,
                     const xstring& s,
                     size_type pos2, size_type n2) {
        if (pos1 > size() || pos2 > s.size())
            throw std::out_of_range("Out of range");
        const size_type len1 = std::min(n1, size() - pos1);
        const size_type len2 = std::min(n2, s.size() - pos2);
        if (size() - len1 >= max_size() - len2)
            throw std::length_error("Too long");
        return replace(begin() + pos1, begin() + pos1 + len1,
                       s.begin() + pos2, s.begin() + pos2 + len2);
    }

    xstring& replace(size_type pos, size_type n1,
                            const value_type* s, size_type n2) {
        if (pos > size())
            throw std::out_of_range("Out of range");
        const size_type len = std::min(n1, size() - pos);
        if (n2 > max_size() || size() - len >= max_size() - n2)
            throw std::length_error("Too long");
        return replace(begin() + pos, begin() + pos + len,
                       s, s + n2);
    }

    xstring& replace(size_type pos, size_type n1,
                     const value_type* s) {
        if (pos > size())
            throw std::out_of_range("Out of range");
        const size_type len = std::min(n1, size() - pos);
        const size_type n2 = traits_type::length(s);
        if (n2 > max_size() || size() - len >= max_size() - n2)
            throw std::length_error("Too long");
        return replace(begin() + pos, begin() + pos + len,
                       s, s + traits_type::length(s));
    }

    xstring& replace(size_type pos, size_type n1,
                     size_type n2, value_type c) {
        if (pos > size())
            throw std::out_of_range("Out of range");
        const size_type len = std::min(n1, size() - pos);
        if (n2 > max_size() || size() - len >= max_size() - n2)
            throw std::length_error("Too long");
        return replace(begin() + pos, begin() + pos + len, n2, c);
    }

    xstring& replace(iterator first, iterator last, 
                     const xstring& s) 
    { return replace(first, last, s.begin(), s.end()); }

    xstring& replace(iterator first, iterator last,
                     const value_type* s, size_type n) 
    { return replace(first, last, s, s + n); }

    xstring& replace(iterator first, iterator last,
                     const value_type* s) {
        return replace(first, last, s, s + traits_type::length(s));
    }

    xstring& replace(iterator first, iterator last, 
                     size_type n, value_type c);
    

    template <class InputIter>
    xstring& replace (iterator first, iterator last, size_type n, 
                      value_type c);

//#ifdef STL_MEMBER_TEMPLATES
    template <class ParamType>
    xstring& replace(iterator first, iterator last,
                       ParamType x, ParamType y) {
        typedef typename IsInteger<ParamType>::query is_integer;
        return replace_dispatch(first, last, x, y,  is_integer());
    }
//#endif /* STL_MEMBER_TEMPLATES */

//#ifdef STL_MEMBER_ TEMPLATES
private:
  template <class IntegerType>
  xstring& replace_dispatch(iterator first, iterator last,
                                    IntegerType n, IntegerType x,
                                    true_type) {
      return replace(first, last, (size_type) n, (value_type) x);
  }

  template <class InputIter>
  xstring& replace_dispatch(iterator first, iterator last,
                            InputIter f, InputIter l,
                            false_type) {
      typedef typename std::iterator_traits<InputIter>::iterator_category Category;
      return replace(first, last, f, l, Category());
  }

private:
  template <class InputIter>
  xstring& replace(iterator first, iterator last,
                   InputIter f, InputIter l, std::input_iterator_tag);

  template <class ForwardIter>
  xstring& replace(iterator first, iterator last,
                   ForwardIter f, ForwardIter l, 
                   std::forward_iterator_tag);
public:

//#endif /* STL_MEMBER_TEMPLATES */

// Other modifier member functions.

    size_type copy(value_type* s, size_type n, size_type pos = 0) const {
      if (pos > size())
          throw std::out_of_range("Out of range");
      const size_type len = std::min(n, size() - pos);
      traits_type::copy(s, begin() + pos, len);
      return len;
    }


    // Misc

    const_reference operator[](size_type n) const { 
        return *(begin()+n);
    }
    reference operator[](size_type n) {
        return *(begin()+n);
    }
    const_reference at(size_type n) const {
        if (n >= size())
            throw std::out_of_range("Out of range");
        return *(begin()+n);
    }
    reference at(size_type n) {
        if (n >= size())
            throw std::out_of_range("Out of range");
        return *(begin()+n);
    }

    // find

    size_type find(const xstring& s, size_type pos = 0) const 
    { return find(s.begin(), pos, s.size()); }

    size_type find(const value_type* s, size_type pos = 0) const 
    { return find(s, pos, traits_type::length(s)); }

    size_type find(const value_type* s, size_type pos, size_type n) const;

    size_type find(value_type c, size_type pos = 0) const;

    // rfind
    size_type rfind(const xstring& s, size_type pos = npos) const 
    { return rfind(s.begin(), pos, s.size()); }

    size_type rfind(const value_type* s, size_type pos = npos) const 
    { return rfind(s, pos, traits_type::length(s)); }

    size_type rfind(const value_type* s, size_type pos, size_type n) const;
    size_type rfind(value_type c, size_type pos = npos) const;

    // find_first_of
    size_type find_first_of(const xstring& s, size_type pos = 0) const 
    { return find_first_of(s.begin(), pos, s.size()); }

    size_type find_first_of(const value_type* s, size_type pos = 0) const 
    { return find_first_of(s, pos, traits_type::length(s)); }

    size_type find_first_of(const value_type* s, size_type pos, 
                          size_type n) const;

    size_type find_first_of(value_type c, size_type pos = 0) const 
    { return find(c, pos); }

    // find_last_of

    size_type find_last_of(const xstring& s,
                         size_type pos = npos) const
    { return find_last_of(s.begin(), pos, s.size()); }

    size_type find_last_of(const value_type* s, size_type pos = npos) const 
    { return find_last_of(s, pos, traits_type::length(s)); }

    size_type find_last_of(const value_type* s, size_type pos, 
                         size_type n) const;

    size_type find_last_of(value_type c, size_type pos = npos) const {
    return rfind(c, pos);
    }

    // find_first_not_of

    size_type find_first_not_of(const xstring& s, 
                              size_type pos = 0) const 
    { return find_first_not_of(s.begin(), pos, s.size()); }

    size_type find_first_not_of(const value_type* s, size_type pos = 0) const 
    { return find_first_not_of(s, pos, traits_type::length(s)); }

    size_type find_first_not_of(const value_type* s, size_type pos,
                              size_type n) const;

    size_type find_first_not_of(value_type c, size_type pos = 0) const;

    // find_last_not_of

    size_type find_last_not_of(const xstring& s, 
                             size_type pos = npos) const
    { return find_last_not_of(s.begin(), pos, s.size()); }

    size_type find_last_not_of(const value_type* s, size_type pos = npos) const
    { return find_last_not_of(s, pos, traits_type::length(s)); }

    size_type find_last_not_of(const value_type* s, size_type pos,
                             size_type n) const;

    size_type find_last_not_of(value_type c, size_type pos = npos) const;

    // Substring.

    xstring substr(size_type pos = 0, size_type n = npos) const {
        if (pos > size())
          throw std::out_of_range("Out of range");
        return xstring(begin() + pos, 
                       begin() + pos + std::min(n, size() - pos));
    }

    int compare(xstring const &s) const {
        return m_compare(begin(),end(),s.begin(),s.end());
    }

private:
    // Default allocator instance used for initialization
    static const allocator_type _default_allocator;

public:
  // Helper function for compare and relational operators
  static int m_compare(const value_type* f1, const value_type* l1,
                        const value_type* f2, const value_type* l2) {
    const ptrdiff_t n1 = l1 - f1;
    const ptrdiff_t n2 = l2 - f2;
    const int cmp = traits_type::compare(f1, f2, std::min(n1, n2));
    return cmp != 0 ? cmp : (n1 < n2 ? -1 : (n1 > n2 ? 1 : 0));
  }
};

// -------- Out of line definitions

// Definition of a default allocator instance to use
// in initializing the base class (saves instantiating
// a default allocator object each time)
template <class Impl>
const typename xstring<Impl>::allocator_type 
      xstring<Impl>::_default_allocator;


template <class Impl>
//inline
xstring<Impl>
  ::xstring(const value_type* s, size_type n,
            const allocator_type& a) 
    : Impl(a) {
    Impl::range_initialize(s, (n!=(size_type)-1) ? s+n : s+traits_type::length(s));
}

//#ifdef STL_MEMBER_TEMPLATES

template <class Impl>
template <class InputIter>
xstring<Impl>&
xstring<Impl>
  ::replace(iterator first, iterator last, InputIter f, InputIter l,
            std::input_iterator_tag) 
{
  for ( ; first != last && f != l; ++first, ++f)
    traits_type::assign(*first, *f);

  if (f == l)
    erase(first, last);
  else
    insert(last, f, l);
  return *this;
}

template <class Impl>
template <class ForwardIter>
xstring<Impl>&
xstring<Impl>
  ::replace(iterator first, iterator last,
            ForwardIter f, ForwardIter l,
            std::forward_iterator_tag) 
{
  difference_type n = 0;
  std::distance(f, l, n);
  const difference_type len = last - first;
  if (len >= n) {
    std::copy(f, l, first);
    erase(first + n, last);
  }
  else {
    ForwardIter m = f;
    std::advance(m, len);
    std::copy(f, m, first);
    insert(last, m, l);
  }
  return *this;
}

//#endif /* STL_MEMBER_TEMPLATES */


template <class Impl>
typename xstring<Impl>::size_type
xstring<Impl>
  ::find(const typename Impl::value_type* s, size_type pos, size_type n) const 
{
  if (pos + n > size())
    return npos;
  else {
    const const_iterator result =
      search(begin() + pos, end(), 
             s, s + n, eq_traits<traits_type>());
    return result != end() ? result - begin() : npos;
  }
}

template <class Impl>
typename xstring<Impl>::size_type
xstring<Impl>
  ::find(typename Impl::value_type c, size_type pos) const 
{
  if (pos >= size())
    return npos;
  else {
    const const_iterator result =
      find_if(begin() + pos, end(),
              bind2nd(eq_traits<traits_type>(), c));
    return result != end() ? result - begin() : npos;
  }
}    

template <class Impl>
typename xstring<Impl>::size_type
xstring<Impl>
  ::rfind(const typename Impl::value_type* s, size_type pos, size_type n) const 
{
  const size_t len = size();

  if (n > len)
    return npos;
  else if (n == 0)
    return std::min(len, pos);
  else {
    const const_iterator last = begin() + std::min(len - n, pos) + n;
    const const_iterator result = std::find_end(begin(), last,
                                           s, s + n,
                                           eq_traits<traits_type>());
    return result != last ? result - begin() : npos;
  }
}

template <class Impl>
typename xstring<Impl>::size_type
xstring<Impl>
  ::rfind(typename Impl::value_type c, size_type pos) const 
{
  const size_type len = size();

  if (len < 1)
    return npos;
  else {
    const const_iterator last = begin() + std::min(len - 1, pos) + 1;
    const_reverse_iterator rresult =
      find_if(const_reverse_iterator(last), rend(),
              bind2nd(eq_traits<traits_type>(), c));
    return rresult != rend() ? (rresult.base() - 1) - begin() : npos;
  }
}

template <class Impl>
typename xstring<Impl>::size_type
xstring<Impl>
  ::find_first_of(const typename Impl::value_type* s, size_type pos, size_type n) const
{
  if (pos >= size())
    return npos;
  else {
    const_iterator result = std::find_first_of(begin() + pos, end(),
                                                   s, s + n,
                                                   eq_traits<traits_type>());
    return result != end() ? result - begin() : npos;
  }
}


template <class Impl>
typename xstring<Impl>::size_type
xstring<Impl>
  ::find_last_of(const typename Impl::value_type* s, size_type pos, size_type n) const
{
  const size_type len = size();

  if (len < 1)
    return npos;
  else {
    const const_iterator last = begin() + std::min(len - 1, pos) + 1;
    const const_reverse_iterator rresult =
      std::find_first_of(const_reverse_iterator(last), rend(),
                           s, s + n,
                           eq_traits<traits_type>());
    return rresult != rend() ? (rresult.base() - 1) - begin() : npos;
  }
}


template <class Impl>
typename xstring<Impl>::size_type
xstring<Impl>
  ::find_first_not_of(const typename Impl::value_type* s, size_type pos, size_type n) const
{
  if (pos > size())
    return npos;
  else {
    const_iterator result = find_if(begin() + pos, end(),
                                not_within_traits<traits_type>(s, s + n));
    return result != end() ? result - begin() : npos;
  }
}

template <class Impl>
typename xstring<Impl>::size_type
xstring<Impl>
  ::find_first_not_of(typename Impl::value_type c, size_type pos) const
{
  if (pos > size())
    return npos;
  else {
    const_iterator result
      = find_if(begin() + pos, end(),
                not1(bind2nd(eq_traits<traits_type>(), c)));
    return result != end() ? result - begin() : npos;
  }
}    

template <class Impl>
typename xstring<Impl>::size_type
xstring<Impl>
  ::find_last_not_of(const typename Impl::value_type* s, size_type pos, size_type n) const 
{

  const size_type len = size();

  if (len < 1)
    return npos;
  else {
    const const_iterator last = begin() + std::min(len - 1, pos) + 1;
    const const_reverse_iterator rresult =
      find_if(const_reverse_iterator(last), rend(),
              not_within_traits<traits_type>(s, s + n));
    return rresult != rend() ? (rresult.base() - 1) - begin() : npos;
  }
}

template <class Impl>
typename xstring<Impl>::size_type
xstring<Impl>
  ::find_last_not_of(typename Impl::value_type c, size_type pos) const 
{
  const size_type len = size();

  if (len < 1)
    return npos;
  else {
    const const_iterator last = begin() + std::min(len - 1, pos) + 1;
    const_reverse_iterator rresult =
      find_if(const_reverse_iterator(last), rend(),
              not1(bind2nd(eq_traits<traits_type>(), c)));
    return rresult != rend() ? (rresult.base() - 1) - begin() : npos;
  }
}



// ------------------------------------------------------------
// Non-member functions.

// Operator+

template <class Impl>
inline xstring<Impl>
operator+(const xstring<Impl>& x,
          const xstring<Impl>& y)
{
  typedef xstring<Impl> Str;
  Str result(x);
  result.append(y);
  return result;
}

template <class Impl>
inline xstring<Impl>
operator+(const typename Impl::value_type* s,
          const xstring<Impl>& y) {
  typedef xstring<Impl> Str;
  Str result(s);
  result.append(y);
  return result;
}

template <class Impl>
inline xstring<Impl>
operator+(typename Impl::value_type c,
          const xstring<Impl>& y) {
  typedef xstring<Impl> Str;
  Str result(&c, 1);
  result.append(y);
  return result;
}

template <class Impl>
inline xstring<Impl>
operator+(const xstring<Impl>& x,
          const typename Impl::value_type * s) {
  typedef xstring<Impl> Str;
  Str result(x);
  result.append(s);
  return result;
}

template <class Impl>
inline xstring<Impl>
operator+(const xstring<Impl>& x,
          const typename Impl::value_type c) {
  typedef xstring<Impl> Str;
  Str result(x);
  result.push_back(c);
  return result;
}

// Operator== and operator!=

template <class Impl>
inline bool
operator==(const xstring<Impl>& x,
           const xstring<Impl>& y) {
  typedef typename Impl::traits_type Traits;
  return x.size() == y.size() &&
         Traits::compare(x.data(), y.data(), x.size()) == 0;
}

template <class Impl>
inline bool
operator==(const typename Impl::value_type* s,
           const xstring<Impl>& y) {
  typedef typename Impl::traits_type Traits;
  size_t n = Traits::length(s);
  return n == y.size() && Traits::compare(s, y.data(), n) == 0;
}

template <class Impl>
inline bool
operator==(const xstring<Impl>& x,
           const typename Impl::value_type* s) {
  typedef typename Impl::traits_type Traits;
  size_t n = Traits::length(s);
  return x.size() == n && Traits::compare(x.data(), s, n) == 0;
}

//#ifdef STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class Impl>
inline bool
operator!=(const xstring<Impl>& x,
           const xstring<Impl>& y) {
  return !(x == y);
}

template <class Impl>
inline bool
operator!=(const typename Impl::value_type* s,
           const xstring<Impl>& y) {
  return !(s == y);
}

template <class Impl>
inline bool
operator!=(const xstring<Impl>& x,
           const typename Impl::value_type* s) {
  return !(x == s);
}

//#endif /* STL_FUNCTION_TMPL_PARTIAL_ORDER */

// Operator< (and also >, <=, and >=).

template <class Impl>
inline bool
operator<(const xstring<Impl>& x,
          const xstring<Impl>& y) {
  return xstring<Impl>
    ::m_compare(x.begin(), x.end(), y.begin(), y.end()) < 0;
}

template <class Impl>
inline bool
operator<(const typename Impl::value_type* s,
          const xstring<Impl>& y) {
  typedef typename Impl::traits_type Traits;
  size_t n = Traits::length(s);
  return xstring<Impl>
    ::m_compare(s, s + n, y.begin(), y.end()) < 0;
}

template <class Impl>
inline bool
operator<(const xstring<Impl>& x,
          const typename Impl::value_type* s) {
  typedef typename Impl::traits_type Traits;
  size_t n = Traits::length(s);
  return xstring<Impl>
    ::m_compare(x.begin(), x.end(), s, s + n) < 0;
}

//#ifdef STL_FUNCTION_TMPL_PARTIAL_ORDER

template <class Impl>
inline bool
operator>(const xstring<Impl>& x,
          const xstring<Impl>& y) {
  return y < x;
}

template <class Impl>
inline bool
operator>(const typename Impl::value_type* s,
          const xstring<Impl>& y) {
  return y < s;
}

template <class Impl>
inline bool
operator>(const xstring<Impl>& x,
          const typename Impl::value_type* s) {
  return s < x;
}

template <class Impl>
inline bool
operator<=(const xstring<Impl>& x,
           const xstring<Impl>& y) {
  return !(y < x);
}

template <class Impl>
inline bool
operator<=(const typename Impl::value_type* s,
           const xstring<Impl>& y) {
  return !(y < s);
}

template <class Impl>
inline bool
operator<=(const xstring<Impl>& x,
           const typename Impl::value_type* s) {
  return !(s < x);
}

template <class Impl>
inline bool
operator>=(const xstring<Impl>& x,
           const xstring<Impl>& y) {
  return !(x < y);
}

template <class Impl>
inline bool
operator>=(const typename Impl::value_type* s,
           const xstring<Impl>& y) {
  return !(s < y);
}

template <class Impl>
inline bool
operator>=(const xstring<Impl>& x,
           const typename Impl::value_type* s) {
  return !(x < s);
}

//#endif STL_FUNCTION_TMPL_PARTIAL_ORDER 


template <class Impl>
std::ostream& operator<<(std::ostream& os, xstring<Impl> const &xs) {
    return os << xs.c_str();
} 

template <class Impl>
std::istream& operator>>(std::istream& os, xstring<Impl> &xs) {
    std::basic_string<xstring<Impl>::value_type> bs;
    is >> bs;
    xs.assign(bs.begin(), bs.end());
    return is;
} 

template <class Impl>
std::wostream& operator<<(std::wostream& os, xstring<Impl> const &xs) {
    return os << xs.c_str();
} 

template <class Impl>
std::wistream& operator>>(std::wistream& os, xstring<Impl> &xs) {
    std::basic_string<xstring<Impl>::value_type> bs;
    is >> bs;
    xs.assign(bs.begin(), bs.end());
    return is;
} 


XS_END_NAMESPACE;

/*
 * Copyright (c) 1997-1999
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */ 

#endif