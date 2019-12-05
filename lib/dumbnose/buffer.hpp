#pragma once

#include <ostream>
#include <iomanip>
#include <vector>


namespace dumbnose {


typedef unsigned char byte;
typedef std::vector<byte> buffer_t;
const int buffer_line_size_ = 16; // bytes per line

} // namespace dumbnose

template<class stream_t>
void output_buffer_line(stream_t& os, dumbnose::buffer_t::const_iterator& it, dumbnose::buffer_t::const_iterator& end)
{
	dumbnose::buffer_t::const_iterator clone_it = it;

	for(int i=0 ; i<dumbnose::buffer_line_size_ ; ++i) {
		if((i%(dumbnose::buffer_line_size_/2))==0)os << "  ";

		if(it!=end) {
			os << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(*it) << ' ';
			++it;
		} else {
			os << "   ";
		}
	}

	os << "  ";

	for(int i=0 ; i<dumbnose::buffer_line_size_ && clone_it!=end ; ++i, ++clone_it) {
		if(isgraph(*clone_it)) {
			os << *clone_it;
		} else {
			os << '.';
		}
	}

	os << std::endl;
}

template<class stream_t>
stream_t& operator<<(stream_t& os, const dumbnose::buffer_t& buffer)
{
	// save the fmt flags
	std::ios_base::fmtflags fmt = os.flags();
		
	os << std::hex << std::uppercase << std::noshowbase << std::right;

	// calculate the # of lines 
	int num_lines = buffer.size() / dumbnose::buffer_line_size_;
	if((buffer.size() % dumbnose::buffer_line_size_)>0) num_lines++;

	dumbnose::buffer_t::const_iterator it=buffer.begin();
	for(int i=0 ; i<num_lines ; ++i) {
		std::cout << std::setw(6) << std::setfill('0') << i*dumbnose::buffer_line_size_ << "  ";
		output_buffer_line(os,it,buffer.end());
	}

	// restore the fmt flags
	os.flags(fmt);

	return os;
}

