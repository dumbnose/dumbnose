#pragma once

#include <dumbnose/mutex.hpp>
#include <dumbnose/event.hpp>


namespace dumbnose {


class shared_memory
{
public:
	 shared_memory(unsigned int size, std::wstring name)
	 {
		mapping_ = OpenFileMappingW(FILE_MAP_ALL_ACCESS, false, name.c_str());

		// If mapping doesn't exist, create it
		if(NULL==mapping_)
		{
			mapping_ = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NULL, size, name.c_str());

			if(NULL==mapping_) throw GetLastError();
		}

		mem_start_ = MapViewOfFile(mapping_, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if(NULL==mem_start_) {
			CloseHandle(mapping_);
			throw GetLastError();
		}
	 }

	 ~shared_memory() {
		assert(NULL!=mapping_);

		CloseHandle(mapping_);
		mapping_ = NULL;
		mem_start_ = NULL;
	 }

	 void* mem_start()
	 {
		 assert(NULL!=mem_start_);

		 return mem_start_;
	 }

private:
	HANDLE			mapping_;		// File Mapping
	void*			mem_start_;		// Pointer to the start of the shared memory
};


};	// namespace dumbnose