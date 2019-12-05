#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <dumbnose/safe_map.hpp>
#include <dumbnose/windows_handle.hpp>


namespace dumbnose {


template<int block_size_ = 64 * 1024>
class mmap_file
{
public:
	typedef boost::shared_ptr<byte> ptr_t;
	typedef unsigned long node_index_t;
	typedef safe_map<node_index_t,ptr_t> mapped_blocks_t;
	typedef boost::tuples::tuple<node_index_t,ptr_t> allocation_pair_t;
	static const int block_size_ = block_size_;

	mmap_file(const std::wstring& filename)
	{
		size_.QuadPart = block_size_; // if it doesn't exist, we'll create one the size of a single block

		file_ = CreateFile(filename.c_str(),GENERIC_ALL,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
		if(!file_) throw windows_exception("mmap_file:  Could not create file");

		if(GetLastError()==ERROR_ALREADY_EXISTS) { // use existing file size
			if(!GetFileSizeEx(file_.handle(),&size_)) throw windows_exception(__FUNCTION__ ": GetFileSizeEx() failed");
			validate();
		}

		mapping_ = CreateFileMapping(file_.handle(),0,PAGE_READWRITE | SEC_COMMIT,size_.HighPart,size_.LowPart,0);
		if(!mapping_) throw windows_exception(__FUNCTION__ ":  Could not create mapping");
	}

	~mmap_file()
	{
	}

	node_index_t block_count() const {
		assert(size_.HighPart==0);
		return size_.LowPart / block_size_;
	}

	allocation_pair_t allocate_block() 
	{
		HOLD_LOCK(mapped_blocks_);
		node_index_t block_num = block_count();
		mapped_blocks_t::iterator it = map_block(block_num);

		return boost::tuples::make_tuple(block_num,it->second);
	}

	ptr_t get_reference(node_index_t block_num)
	{
		HOLD_LOCK(mapped_blocks_);
		if(block_num >= block_count()) throw std::exception(__FUNCTION__ ": Invalid block num");
		mapped_blocks_t::iterator it = mapped_blocks_.find(block_num);
		if(it==mapped_blocks_.end()) it = map_block(block_num); //need to map it

		return it->second;
	}

protected:

	mapped_blocks_t::iterator map_block(node_index_t block_num)
	{
		LARGE_INTEGER offset;  offset.QuadPart = block_num * block_size_;
		LARGE_INTEGER needed_size; needed_size.QuadPart = offset.QuadPart+block_size_;

		if((needed_size.QuadPart) > size_.QuadPart) {
			extend_file(needed_size);
		}

		byte* mem_loc = reinterpret_cast<byte*>(MapViewOfFile(mapping_.handle(),FILE_MAP_ALL_ACCESS,offset.HighPart,offset.LowPart,block_size_));
		if(mem_loc==0) throw windows_exception(__FUNCTION__ ": MapViewOfFile failed");

		ptr_t ptr(mem_loc,&unmap_address); // When this goes out of scope, it will call UnmapViewOfFile

		std::pair<mapped_blocks_t::iterator,bool> result = mapped_blocks_.insert(std::make_pair(block_num,ptr));
		if(result.second==false) throw std::exception(__FUNCTION__ ":  Duplicate entry discovered");

		return result.first;
	}

	void unmap_block(node_index_t block_num)
	{
		mapped_blocks_t::iterator it = mapped_blocks_.find(block_num);
		if(it==mapped_blocks_.end()) throw std::exception(__FUNCTION__ ": Could not find block to unmap");
		unmap_address(it->second.get());
	}

	// simple wrapper that throws error on failure
	static void unmap_address(void* address)
	{
		if(!UnmapViewOfFile(address)) throw windows_exception(__FUNCTION__ ": UnmapViewOfFile failed.");
	}

	void extend_file(LARGE_INTEGER new_size)
	{
		mapping_ = CreateFileMapping(file_.handle(),0,PAGE_READWRITE | SEC_COMMIT,new_size.HighPart,new_size.LowPart,0);
		if(!mapping_) throw windows_exception(__FUNCTION__ ":  Could not create mapping");
		size_ = new_size;
	}

	void validate()
	{
		if(size_.QuadPart==0) throw std::exception(__FUNCTION__ ": Existing file corrupt (zero length)");
		if((size_.QuadPart % block_size_)>0) throw std::exception(__FUNCTION__ ": Existing file corrupt (not a multiple of block size)");
	}

private:
	windows_handle file_;
	windows_handle mapping_;
	mapped_blocks_t mapped_blocks_;
	LARGE_INTEGER size_;
};


} // namespace dumbnose
