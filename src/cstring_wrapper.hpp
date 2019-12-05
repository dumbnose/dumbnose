#pragma once

#include <string> // For char_traits
#include "xs_defs.h" // Utility macros


namespace dumbnose {


// Fixed size character buffer class.
/// This class implements the minimal string interface 
/// and is intended to be used as a base class for 
/// xstring<>, which provides the full string interface.
template <size_t max_size_t, 
          class char_t = wchar_t, 
          class traits_t = std::char_traits<char_t> > 
class cstring_wrapper_t {
public: 
    // STL container interface typedefs:
    typedef char_t value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef const value_type* const_iterator;
    typedef value_type* iterator;
    typedef std::reverse_iterator<const_iterator> 
            const_reverse_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator; 
    typedef std::allocator<char_t> allocator_type;

    // String interface typedefs:
    typedef traits_t traits_type;
    static const size_type npos;

    // Constructors:
    cstring_wrapper_t(allocator_type const &a=_allocator) {
        _end = _buffer;
    }

    cstring_wrapper_t(cstring_wrapper_t const &s) {
        range_initialize(s.begin(), s.end());
    }

    // Copy operator:
    cstring_wrapper_t &operator=(cstring_wrapper_t const &s) {
        if (this!=&s)
            range_initialize(s.begin(), s.end());
    }

    // Insert:
    template <class InputIter>
    void insert(iterator pos, InputIter first, InputIter last) {
        if (first!=last) { 
            size_type xtra = std::distance(first,last);
            if (pos != _end) {
                if (_end-_buffer + xtra >= max_size_t)
                    throw std::out_of_range("cstring_wrapper_t"); 
                traits_t::move(pos+xtra,pos,_end-pos);
            }
            std::copy(first,last,pos);
            _end += xtra; // Expand end point
        }
    }

    // Erase:
    iterator erase(iterator first, iterator last) {
        if (first != last) {
          traits_t::move(first, last, (_end - last) + 1);
          const iterator new_finish = _end - (last - first);
          _end = new_finish;
        }
        return first;        
    }

    /// (Replace can be defined in terms of erase and insert,
    /// so the minimal interface can skip it.)

    // STL container member function interface:
    const allocator_type &get_allocator() const {
        return _allocator;
    }
    
    iterator begin() {return _buffer;}
    const_iterator begin() const {return _buffer;}
    iterator end() {return _end;}
    const_iterator end() const {return _end;}

    void clear() {_end = _buffer;}
    size_t max_size() const { return max_size_t; }
    bool empty() const { return (_end == _buffer); }    
    size_type size() const {return _end - _buffer;}

    void swap(cstring_wrapper_t& s) {
        /// Not as cheap as some swaps, but guaranteed
        /// not to throw:
        for(int i=0;i<max_size_t;++i) {
            std::swap(_buffer[i],s._buffer[i]);
        }
        int len = _end - _buffer;
        _end = s.size()+_buffer;
        s._end = len+s._buffer;
    }

    // String interface member functions:
    void reserve(size_type n = 0) {
        if (n > max_size_t) 
            throw std::length_error("cstring_wrapper_t");
    } 
    size_type capacity() const { return max_size(); }

    const char_t* c_str() const {
        *_end = char_t(); // Null terminate.
        return _buffer;
    }

    const char_t* data() const {
        return _buffer;
    }

protected: 
    // Internal interface used by xstring wrapper.

    // Initializes with n copies of given element:
    void element_initialize(size_type n, value_type c) {
        if (n > max_size_t) 
            throw std::length_error("cstring_wrapper_t");
        std::uninitialized_fill_n(begin(), n, c);
        _end = _buffer+n;
    }  

    // Initializes with given range of elements:
    template <class InputIter>
    void range_initialize(InputIter first, InputIter last) {
        size_t n = std::distance(first,last);
        if (n >= max_size_t)
            throw std::length_error("cstring_wrapper_t");
        std::uninitialized_copy(first,last,_buffer);
        _end = _buffer+n; 
    }

private: 
    // A fixed size buffer implementation:
    char_t _buffer[max_size_t+1]; // Space for max_size_t elements + null 
    mutable char_t *_end ;  // End of text in _buffer
    static const allocator_type _allocator; // Dummy constant
};

/// Definition of "npos" constant:
template <size_t max_size_t, class char_t, class traits_t>
const typename cstring_wrapper_t<max_size_t,char_t,traits_t>::size_type 
      cstring_wrapper_t<max_size_t,char_t,traits_t>::npos 
      = (cstring_wrapper_t<max_size_t,char_t,traits_t>::size_type) -1;

/// Definition of "_allocator" dummy constant:
template <size_t max_size_t, class char_t, class traits_t>
const typename cstring_wrapper_t<max_size_t,char_t,traits_t>::allocator_type 
      cstring_wrapper_t<max_size_t,char_t,traits_t>::_allocator;

typedef cstring_wrapper_t<50,char> cstring_wrapper;
typedef cstring_wrapper_t<50,wchar_t> cwstring_wrapper;


} // namespace dumbnose


