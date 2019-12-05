#pragma once

#include <dumbnose/windows_exception.hpp>


namespace dumbnose {


class hive_util 
{
public:
	static HKEY mount_hive(const std::wstring& hive_name, const std::wstring& hive_file_path)
	{
		// mount the hive
		DWORD err = ::RegLoadKeyW(HKEY_USERS, hive_name.c_str(), hive_file_path.c_str());
		if (err!=ERROR_SUCCESS) throw windows_exception(__FUNCTION__ ": RegLoadKeyW() failed", err);

		// open the key with adequate access rights
		HKEY root;
		err = ::RegOpenKeyExW(HKEY_USERS, hive_name.c_str(), 0, KEY_ALL_ACCESS | WRITE_DAC, &root);
		if (err!=ERROR_SUCCESS) throw windows_exception(__FUNCTION__ ": RegOpenKeyExW() failed", err);

		return root;
	}

};


} // namespace dumbnose
