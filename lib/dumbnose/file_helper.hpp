#pragma once


#include <windows.h>
#include "windows_exception.hpp"
#include "windows_handle.hpp"
#include <string>
#include <list>
#include <tuple>


namespace dumbnose { 

template<int dummy=0>
class file_helper {
public:

typedef std::list<std::tuple<std::wstring, WIN32_FIND_DATAW> > file_find_data_list_t;


static file_find_data_list_t find_files(const std::wstring& root, const std::wstring& search_pattern)
{
	WIN32_FIND_DATAW fd;
	HANDLE find = INVALID_HANDLE_VALUE;
	file_find_data_list_t results;

	std::wstring root_dir = root + L"\\";
	std::wstring search = root_dir + search_pattern;

	// search this directory
	find = FindFirstFileW(search.c_str(), &fd);
	if(find==INVALID_HANDLE_VALUE) return results;

	do {
		results.push_back(std::make_tuple(root_dir+fd.cFileName,fd));
	}while(FindNextFile(find,&fd));

	FindClose(find);

	return results;
}


static file_find_data_list_t search_path(const std::wstring& root, const std::wstring& search_pattern)
{
	// search the requested directory
	file_find_data_list_t results = find_files(root,search_pattern);

	// recurse subdirectories
	file_find_data_list_t children = find_files(root,L"*.*");
	for(file_find_data_list_t::iterator it=children.begin() ; it!=children.end() ; ++it) {
		if(std::get<1>(*it).dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) {
			file_find_data_list_t child_results = find_files(std::get<0>(*it),search_pattern);
			results.insert(results.end(),child_results.begin(),child_results.end());
		}
	}

	return results;
}

static wchar_t get_current_drive_letter()
{
	DWORD result = 0;
	WCHAR buffer[MAX_PATH];
	if(0==::GetCurrentDirectoryW(MAX_PATH, buffer)) throw windows_exception("Could not get current directory", GetLastError());

	return buffer[0];
}
};

} // namespace dumbnose
