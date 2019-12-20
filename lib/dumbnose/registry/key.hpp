#pragma once

#include <list>
#include <map>


namespace dumbnose { namespace registry {

class key
{
public:

	key() : handle_(new handle_holder()) {}
	~key() {}

	static key hklm() 
	{
		key hklm_key;
		hklm_key.open(HKEY_LOCAL_MACHINE,L"",KEY_READ);
		return hklm_key;
	}

	static key hkcu()
	{
		key hklm_key;
		hklm_key.open(HKEY_CURRENT_USER, L"", KEY_READ);
		return hklm_key;
	}

	const HKEY handle() const {return handle_->handle();}

	key open_read(std::wstring subkey_name) 
	{
		key subkey;
		subkey.open(handle_->handle(),subkey_name,KEY_READ);
		return subkey;
	}

	key open_all_access(std::wstring subkey_name) 
	{
		key subkey;
		subkey.open(handle_->handle(),subkey_name,KEY_ALL_ACCESS);
		return subkey;
	}

	std::list<std::wstring> enum_value_names()
	{
		std::list<std::wstring> result;
		wchar_t name_buffer[1024];
		DWORD type = 0;

		for(int i=0 ;; ++i)
		{
			DWORD name_buffer_len = ARRAYSIZE(name_buffer);
			LSTATUS status = RegEnumValue(handle_->handle(), i, name_buffer,&name_buffer_len,0,&type,0,0);
			if(ERROR_NO_MORE_ITEMS==status) {
				break;
			} else if(ERROR_SUCCESS!=status) {
				throw windows_exception("Could not enumerate values",status);
			}

			if(name_buffer_len>0) result.push_back(name_buffer);
		}

		return result;
	}

	void clear_enum_values()
	{
		auto values = enum_value_names();
		for (auto value : values) {
			RegDeleteValue(handle_->handle(), value.c_str());
		}
	}

	std::map<std::wstring, std::wstring> enum_values_as_strings()
	{
		std::map<std::wstring, std::wstring> result;
		wchar_t name_buffer[1024] = { 0 };
		wchar_t value_buffer[4096] = { 0 };
		DWORD type = 0;

		for(int i=0 ;; ++i)
		{
			DWORD name_buffer_len = ARRAYSIZE(name_buffer);
			DWORD value_buffer_len = sizeof(value_buffer);
			LSTATUS status = RegEnumValue(handle_->handle(), i, name_buffer,&name_buffer_len,0,&type,reinterpret_cast<LPBYTE>(value_buffer),&value_buffer_len);
			if(ERROR_NO_MORE_ITEMS==status) {
				break;
			} else if(ERROR_SUCCESS!=status) {
				throw windows_exception("Could not enumerate values",status);
			}

			if ((value_buffer_len > 0) && (REG_SZ == type)) result[name_buffer] = value_buffer;
		}

		return result;
	}

	void save_enum_values_as_strings(std::map<std::wstring, std::wstring> values)
	{
		// Clean up old values first
		clear_enum_values();

		for (auto value : values) {
			LSTATUS status = RegSetValueEx(handle_->handle(), value.first.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(value.second.c_str()), value.second.length()*sizeof(wchar_t));
			if (ERROR_SUCCESS != status) throw windows_exception("Could not enumerate values", status);
		}
	}

protected:

	// holder class
	class handle_holder {
	public:
		handle_holder() : handle_(0) {}
		handle_holder(HKEY handle) {assign(handle);}

		void assign(HKEY handle) {
			close();
			handle_ = handle;
		}

		const HKEY handle(){return handle_;}

		~handle_holder() {close();}

		void close() {if(handle_!=0){RegCloseKey(handle_);}}

	private:
		HKEY handle_;
	};

	void open(HKEY base_key, const std::wstring subkey_path, REGSAM access)
	{
		HKEY handle = 0;
		LONG result = RegOpenKeyEx(base_key, subkey_path.c_str(), 0, access, &handle);
		if(result!=ERROR_SUCCESS) throw windows_exception("Could not open key", result);
		handle_->assign(handle);
	}

private:
	std::shared_ptr<handle_holder> handle_;
};


}} // namespace dumbnose { namespace registry {
