#pragma once

#include <dumbnose/event.hpp>


namespace dumbnose {


template<class derived_t>
class service_dll {
public:

	service_dll(const std::wstring& name) : name_(name), stop_event_(true,false){
		status_.dwServiceType = SERVICE_WIN32_SHARE_PROCESS;
		status_.dwCurrentState = SERVICE_START_PENDING;
		status_.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		status_.dwWin32ExitCode = NO_ERROR;
	}

	virtual ~service_dll(){}

	void start() {
		svc_ = RegisterServiceCtrlHandlerEx(name_.c_str(),&static_handler,this);
		set_status(SERVICE_START_PENDING);
	}

	void main() {
		set_status(SERVICE_RUNNING);
		stop_event_.wait();
		set_status(SERVICE_STOPPED);
	}

	void set_status(DWORD state) {
		status_.dwCurrentState = state;
		SetServiceStatus(svc_,&status_);
	}

	virtual DWORD handler(DWORD control,DWORD event_type,LPVOID event_data)
	{
		switch(control) {
			case SERVICE_CONTROL_STOP:
				stop_event_.set();
				break;
			case SERVICE_CONTROL_INTERROGATE:
				break;
			default:
				return ERROR_CALL_NOT_IMPLEMENTED;
		}

		return NO_ERROR;
	}

	static DWORD WINAPI static_handler(DWORD control,DWORD event_type,LPVOID event_data,LPVOID context)
	{
		service_dll* service = reinterpret_cast<service_dll*>(context);

		return service->handler(control,event_type,event_data);
	}

protected:
	//
	// registration methods
	//
	void register_service(	const std::wstring& description,
							const std::wstring& displayname,
							const std::wstring& dll_path
						 )
	{
		create_service(displayname,description);
		create_svchost_config();
		set_service_params(dll_path);
	}

	void unregister_service()
	{
		SC_HANDLE scm = OpenSCManager(0,0,SC_MANAGER_ALL_ACCESS);
		if(scm==NULL) throw windows_exception("Could not open SC manager",GetLastError());
		ON_BLOCK_EXIT(&CloseServiceHandle,scm);

		SC_HANDLE svc = OpenService(scm,name_.c_str(),GENERIC_ALL);
		if(svc==NULL) throw windows_exception("Could not open service",GetLastError());
		ON_BLOCK_EXIT(&CloseServiceHandle,svc);

		if(!DeleteService(svc))  throw windows_exception("Could not delete service",GetLastError());
	}

protected:
	static const wchar_t svchost_key_[];
	static const wchar_t service_key_[];
	static const wchar_t svchost_cmd_line_[];

	std::wstring name_;
	SERVICE_STATUS_HANDLE svc_;
	SERVICE_STATUS status_;
	dumbnose::event stop_event_;

private:
	void create_service(const std::wstring& display_name, const std::wstring& description)
	{
		SC_HANDLE scm = OpenSCManager(0,0,SC_MANAGER_ALL_ACCESS);
		if(scm==NULL) throw windows_exception("Could not open SC manager", GetLastError());
		ON_BLOCK_EXIT(&CloseServiceHandle,scm);

		std::wstring cmd_line = svchost_cmd_line_ + name_;
		SC_HANDLE svc = CreateService(scm,name_.c_str(),display_name.c_str(),GENERIC_ALL,SERVICE_WIN32_SHARE_PROCESS,SERVICE_AUTO_START,SERVICE_ERROR_NORMAL,cmd_line.c_str(),0,0,0,0,0);
		if(svc==NULL) throw windows_exception("Could not create service", GetLastError());
		ON_BLOCK_EXIT(&CloseServiceHandle,svc);

		SERVICE_DESCRIPTION desc;
		desc.lpDescription = const_cast<wchar_t*>(description.c_str());
		if(!ChangeServiceConfig2(svc,SERVICE_CONFIG_DESCRIPTION,&desc))  throw windows_exception("Could not set service description", GetLastError());
	}

	void create_svchost_config()
	{
		HKEY key=0;
		LSTATUS status = RegOpenKey(HKEY_LOCAL_MACHINE,svchost_key_,&key);
		if(status!=ERROR_SUCCESS) throw windows_exception("Could not open SvcHost key",status);

		std::wstring multi_str = name_ + L'\0';
		status = RegSetValueEx(key,name_.c_str(),0,REG_MULTI_SZ,reinterpret_cast<LPBYTE>(const_cast<wchar_t*>(multi_str.c_str())),static_cast<DWORD>((multi_str.length()+1)*sizeof(wchar_t)));
		RegCloseKey(key);
		if(status!=ERROR_SUCCESS) throw windows_exception("Could not set service value under SvcHost key",status);
	}

	HRESULT set_service_params(const std::wstring& dll_path)
	{
		HKEY key=0;
		std::wstring reg_path = service_key_ + name_;
		LSTATUS status = RegOpenKey(HKEY_LOCAL_MACHINE,reg_path.c_str(),&key);
		if(status!=ERROR_SUCCESS)  throw windows_exception("Could not open SvcHost key",status);

		HKEY param_key=0;
		status = RegCreateKey(key,L"Parameters",&param_key);
		if(status!=ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

		status = RegSetValueEx(param_key,L"ServiceDll",0,REG_EXPAND_SZ,(LPBYTE) dll_path.c_str(),static_cast<DWORD>((dll_path.length()+1)*sizeof(wchar_t)));
		if(status!=ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

		DWORD unload_on_stop = 1;
		status = RegSetValueEx(param_key,L"ServiceDllUnLoadOnStop",0,REG_DWORD,(LPBYTE) &unload_on_stop,sizeof(unload_on_stop));
		if(status!=ERROR_SUCCESS) return HRESULT_FROM_WIN32(status);

		return S_OK;
	}

};


template<class derived_t>
const wchar_t service_dll<derived_t>::svchost_key_[] = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost";
template<class derived_t>
const wchar_t service_dll<derived_t>::service_key_[] = L"SYSTEM\\CurrentControlSet\\Services\\";
template<class derived_t>
const wchar_t service_dll<derived_t>::svchost_cmd_line_[] = L"%SystemRoot%\\System32\\svchost.exe -k ";


} // namespace dumbnose
