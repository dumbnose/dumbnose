#pragma once


#include <limits>


namespace dumbnose {


template<size_t num_bits_>
class bitset {
public:
	// @todo:  use mpl to optimize later
	typedef unsigned long long storage_t;
	static const size_t invalid_bit_ = -1;

	bitset(storage_t val = 0) : bits_(val) {
	}

	storage_t bits() {
		return bits_;
	}

	void set_bits(storage_t val) {
		bits_ &= val;
	}

	size_t count() {
		storage_t temp = bits_;
		int count = 0;

		while(temp!=0) {
			if(temp & 1) ++count;
			temp >>= 1;
		}

		return count;
	}

	size_t first_set() {
		storage_t temp = bits_;
		int bit = 0;

		while(temp!=0) {
			if(temp & 1) ++count;
			temp >>= 1;
		}

		return count;
	}

private:
	storage_t bits_;
};


} // namespace dumbnose
