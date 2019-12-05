#pragma once

#include <vector>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

namespace dumbnose {


class path_util
{
public:

	static inline std::vector<wchar_t> create_buffer(const std::wstring& path)
	{
		std::vector<wchar_t> new_path(path.size()+1);
		new_path.assign(path.begin(),path.end());
		new_path.push_back(L'\0');

		return new_path;
	}

	static std::wstring strip_path(const std::wstring& path)
	{
		if(path.size()==0) return L"";
		std::vector<wchar_t> new_path = create_buffer(path);
		PathStripPath(&new_path[0]);

		return &new_path[0];
	}

	static std::wstring remove_filespec(const std::wstring& path)
	{
		if(path.size()==0) return L"";

		std::vector<wchar_t> new_path = create_buffer(path);
		if(!PathRemoveFileSpec(&new_path[0])) throw windows_exception("PathRemoveFileSpec() failed");

		return &new_path[0];
	}

	static std::wstring get_current_directory()
	{
		std::vector<wchar_t> path(MAX_PATH+1);
		GetCurrentDirectory(path.size(),&path[0]);
		return &path[0];
	}

	static std::wstring strip_to_root(const std::wstring& path)
	{
		if(path.size()==0) return L"";
		std::vector<wchar_t> new_path = create_buffer(path);
		PathStripToRoot(&new_path[0]);

		return &new_path[0];
	}

	static const wchar_t* find_next_component(const std::wstring& path)
	{
		if(path.size()==0) return 0;
		return PathFindNextComponent(path.c_str());
	}

};

}
