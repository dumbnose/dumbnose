#pragma once


namespace dumbnose {


template<int dummy=0>
class env_var {
public:

	static void set_system_env_var(const std::wstring& name, const std::wstring& value)
	{
		static wchar_t* system_env_path = L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";

		windows_handle key;
		LONG result = RegCreateKeyExW(HKEY_LOCAL_MACHINE, system_env_path, 0, NULL ,REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, reinterpret_cast<HKEY*>(&key.handle_ref()), NULL);
		if(ERROR_SUCCESS!=result) throw windows_exception("Unable to open system environment key", result);

		result = RegSetValueExW(static_cast<HKEY>(key.handle()), name.c_str(), 0, REG_SZ, reinterpret_cast<const byte*>(value.c_str()), value.length()*sizeof(wchar_t));
		if(ERROR_SUCCESS!=result) throw windows_exception("Unable to set system environment value", result);
	}


	static void set_user_env_var(const std::wstring& name, const std::wstring& value)
	{
		static wchar_t* user_env_path = L"Environment";

		windows_handle key;
		LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, user_env_path, 0, NULL ,REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, reinterpret_cast<HKEY*>(&key.handle_ref()), NULL);
		if(ERROR_SUCCESS!=result) throw windows_exception("Unable to open system environment key", result);

		result = RegSetValueExW(static_cast<HKEY>(key.handle()), name.c_str(), 0, REG_SZ, reinterpret_cast<const byte*>(value.c_str()), value.length()*sizeof(wchar_t));
		if(ERROR_SUCCESS!=result) throw windows_exception("Unable to set system environment value", result);
	}

};





} // namespace dumbnose