#pragma once

#include <list>

namespace dumbnose { namespace registry {

class key
{
public:

	typedef std::list<std::wstring> wstring_list;

// constructors / destructor
	key() : handle_(new handle_holder()) {}
	~key() {}

	static key hklm() 
	{
		key hklm_key;
		hklm_key.open(HKEY_LOCAL_MACHINE,L"",KEY_READ);
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

	wstring_list enum_value_names()
	{
		wstring_list result;
		wchar_t name_buffer[1024];
		DWORD type = 0;

		for(int i=0 ;; ++i)
		{
			DWORD name_buffer_len = sizeof(name_buffer);
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

	wstring_list enum_value_strings()
	{
		wstring_list result;
		wchar_t name_buffer[1024];
		byte value_buffer[2048];
		DWORD type = 0;

		for(int i=0 ;; ++i)
		{
			DWORD name_buffer_len = sizeof(name_buffer);
			DWORD value_buffer_len = sizeof(value_buffer);
			LSTATUS status = RegEnumValue(handle_->handle(), i, name_buffer,&name_buffer_len,0,&type,value_buffer,&value_buffer_len);
			if(ERROR_NO_MORE_ITEMS==status) {
				break;
			} else if(ERROR_SUCCESS!=status) {
				throw windows_exception("Could not enumerate values",status);
			}

			if((value_buffer_len>0) && (REG_SZ==type)) result.push_back(std::wstring(reinterpret_cast<wchar_t*>(value_buffer)/*,value_buffer_len/sizeof(wchar_t)*/));
		}

		return result;
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
	shared_ptr<handle_holder> handle_;
};


}} // namespace dumbnose { namespace registry {
