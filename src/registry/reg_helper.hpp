#pragma once

#include <dumbnose/windows_handle.hpp>
#include <vector>
#include <dumbnose/scope_guard.hpp>

namespace dumbnose { namespace registry {


template<int dummy=0>
class reg_helper
{
public:

	static windows_handle open_key(HKEY base_key, const wchar_t* subkey_path)
	{
		windows_handle key;
		LONG result = RegCreateKeyEx(base_key, subkey_path, 0, 0, 0, KEY_ALL_ACCESS, 0, reinterpret_cast<HKEY*>(&key.handle_ref()),0);
		if(result!=ERROR_SUCCESS) throw windows_exception("Could not create key", result);

		return key;
	}

	static void set_reg_value(HKEY base_key, const wchar_t* subkey_path, const wchar_t* value_name, DWORD value)
	{
		windows_handle key = open_key(base_key,subkey_path);

		LONG result = RegSetValueEx(reinterpret_cast<HKEY>(key.handle_ref()), value_name, 0, REG_DWORD, reinterpret_cast<BYTE*>(&value), sizeof(DWORD));
		if(result!=ERROR_SUCCESS) throw windows_exception("Could not set value", result);
	}

	static void set_reg_value(HKEY base_key, const wchar_t* subkey_path, const wchar_t* value_name, const std::wstring& value)
	{
		windows_handle key = open_key(base_key,subkey_path);

		LONG result = RegSetValueEx(reinterpret_cast<HKEY>(key.handle_ref()), value_name, 0, REG_SZ, reinterpret_cast<BYTE*>(const_cast<wchar_t*>(value.c_str())), sizeof(wchar_t)*(value.length()+1));
		if(result!=ERROR_SUCCESS) throw windows_exception("Could not set value", result);
	}

	static std::wstring get_reg_value_string(HKEY base_key, const wchar_t* subkey_path, const wchar_t* value_name)
	{
		HKEY key = 0;
		LONG result = RegOpenKeyEx(base_key, subkey_path, 0, KEY_READ, &key);
		if(result!=ERROR_SUCCESS) throw windows_exception("Could not open key", result);
		//ON_BLOCK_EXIT(&RegCloseKey,key);

		std::vector<BYTE> buffer(MAX_PATH*4);
		DWORD buffer_size = buffer.size();
		DWORD val_type = 0;

		result = RegQueryValueEx(key, value_name, 0, &val_type, &buffer[0], &buffer_size);
		if(result!=ERROR_SUCCESS) throw windows_exception("Could not get value", result);
		if(val_type!=REG_SZ) throw windows_exception("Value is not a string", result);

		return std::wstring(reinterpret_cast<wchar_t*>(&buffer[0]));
	}

};


}} // namespace dumbnose { namespace registry {
