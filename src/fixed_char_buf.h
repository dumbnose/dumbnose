// Copyright (c) 2001 John Panzer
// Permission is granted to use this code without restriction as
// long as this copyright notice appears in all source files.
#ifndef FIXED_CHAR_BUF_H_INCLUDED
#define FIXED_CHAR_BUF_H_INCLUDED

#include <string> // For char_traits
#include "xs_defs.h" // Utility macros

XS_NAMESPACE(xstr)

// Fixed size character buffer class.
/// This class implements the minimal string interface 
/// and is intended to be used as a base class for 
/// xstring<>, which provides the full string interface.
template <size_t SIZE, 
          class CharT = char, 
          class Traits = std::char_traits<CharT> > 
class fixed_char_buf {
public: 
    // STL container interface typedefs:
    typedef CharT value_type;
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
    typedef std::allocator<CharT> allocator_type;

    // String interface typedefs:
    typedef Traits traits_type;
    static const size_type npos;

    // Constructors:
    fixed_char_buf(allocator_type const &a=_allocator) {
        _end = _buffer;
    }

    fixed_char_buf(fixed_char_buf const &s) {
        range_initialize(s.begin(), s.end());
    }

    // Copy operator:
    fixed_char_buf &operator=(fixed_char_buf const &s) {
        if (this!=&s)
            range_initialize(s.begin(), s.end());
    }

    // Insert:
    template <class InputIter>
    void insert(iterator pos, InputIter first, InputIter last) {
        if (first!=last) { 
            size_type xtra = std::distance(first,last);
            if (pos != _end) {
                if (_end-_buffer + xtra >= SIZE)
                    throw std::out_of_range("fixed_char_buf"); 
                Traits::move(pos+xtra,pos,_end-pos);
            }
            std::copy(first,last,pos);
            _end += xtra; // Expand end point
        }
    }

    // Erase:
    iterator erase(iterator first, iterator last) {
        if (first != last) {
          Traits::move(first, last, (_end - last) + 1);
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
    size_t max_size() const { return SIZE; }
    bool empty() const { return (_end == _buffer); }    
    size_type size() const {return _end - _buffer;}

    void swap(fixed_char_buf& s) {
        /// Not as cheap as some swaps, but guaranteed
        /// not to throw:
        for(int i=0;i<SIZE;++i) {
            std::swap(_buffer[i],s._buffer[i]);
        }
        int len = _end - _buffer;
        _end = s.size()+_buffer;
        s._end = len+s._buffer;
    }

    // String interface member functions:
    void reserve(size_type n = 0) {
        if (n > SIZE) 
            throw std::length_error("fixed_char_buf");
    } 
    size_type capacity() const { return max_size(); }

    const CharT* c_str() const {
        *_end = CharT(); // Null terminate.
        return _buffer;
    }

    const CharT* data() const {
        return _buffer;
    }

protected: 
    // Internal interface used by xstring wrapper.

    // Initializes with n copies of given element:
    void element_initialize(size_type n, value_type c) {
        if (n > SIZE) 
            throw std::length_error("fixed_char_buf");
        std::uninitialized_fill_n(begin(), n, c);
        _end = _buffer+n;
    }  

    // Initializes with given range of elements:
    template <class InputIter>
    void range_initialize(InputIter first, InputIter last) {
        size_t n = std::distance(first,last);
        if (n >= SIZE)
            throw std::length_error("fixed_char_buf");
        std::uninitialized_copy(first,last,_buffer);
        _end = _buffer+n; 
    }

private: 
    // A fixed size buffer implementation:
    CharT _buffer[SIZE+1]; // Space for SIZE elements + null 
    mutable CharT *_end ;  // End of text in _buffer
    static const allocator_type _allocator; // Dummy constant
};

/// Definition of "npos" constant:
template <size_t SIZE, class CharT, class Traits>
const typename fixed_char_buf<SIZE,CharT,Traits>::size_type 
      fixed_char_buf<SIZE,CharT,Traits>::npos 
      = (typename fixed_char_buf<SIZE,CharT,Traits>::size_type) -1;

/// Definition of "_allocator" dummy constant:
template <size_t SIZE, class CharT, class Traits>
const typename fixed_char_buf<SIZE,CharT,Traits>::allocator_type 
      fixed_char_buf<SIZE,CharT,Traits>::_allocator;

XS_END_NAMESPACE

#endif
