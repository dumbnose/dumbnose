#pragma once

#include "windows_exception.hpp"
#include "windows_handle.hpp"
#include <Psapi.h>
#include <boost/algorithm/string.hpp>
#include <Dbghelp.h>


#pragma comment(lib,"psapi.lib")

namespace dumbnose {


class process_util 
{
protected:
	struct LookupContext
	{
		const std::string& module_name;
		bool is_module;
	};

	static BOOL CALLBACK is_module(PCSTR ModuleName,ULONG ModuleBase,ULONG ModuleSize,PVOID UserContext)
	{
		LookupContext* lc = static_cast<LookupContext*>(UserContext);
		if(lc==NULL) return FALSE;

		if(!lc->is_module) lc->is_module = boost::iends_with(ModuleName,lc->module_name);

		return TRUE;
	}


public:

	static bool contains_module(DWORD pid, const std::string& module_name)
	{
		windows_handle proc = dumbnose::process_util::open_process_handle(pid);
		if(!proc) return false;

		LookupContext lc = {module_name, false};
		if(!EnumerateLoadedModules(proc.handle(),(PENUMLOADED_MODULES_CALLBACK)&is_module,&lc)) throw windows_exception("Could not enumerate modules");

		return lc.is_module;
	}

	static windows_handle open_process_handle(DWORD pid, DWORD access = PROCESS_QUERY_INFORMATION | PROCESS_VM_READ)
	{
		return OpenProcess(access, FALSE, pid);
	}

	static std::wstring get_process_name(DWORD pid)
	{
		wchar_t process_name[MAX_PATH] = L"";

		windows_handle proc = open_process_handle(pid);
		if (!proc) return L"";  // if we can't open the process, just return ""
	
		HMODULE mod;
		DWORD needed;

		if (EnumProcessModules(proc.handle(), &mod, sizeof(mod), &needed)) {
			GetModuleBaseNameW(proc.handle(), mod, process_name, sizeof(process_name)/sizeof(wchar_t));
		}

		return process_name;
	}

	static DWORD get_pid_by_name(const std::wstring& process_name)
	{
		DWORD pids[2048];
		DWORD returned;
		int num_procs;

		if (!EnumProcesses(pids, sizeof(pids), &returned)) throw windows_exception("Could not enumerate processes", GetLastError());

		num_procs = returned / sizeof(DWORD);
		for (int i=0; i<num_procs; i++) {
			std::wstring curr_process_name = get_process_name(pids[i]);

			if(0==_wcsicmp(process_name.c_str(),curr_process_name.c_str())) return pids[i];
		}

		return -1;
	}

	static bool is_running(const std::wstring& process_name)
	{
		return get_pid_by_name(process_name)!=-1;
	}

	static std::wstring get_current_process_path()
	{
		wchar_t file_buffer[MAX_PATH+1];		
		if(0==GetModuleFileName(0,file_buffer,sizeof(file_buffer)/sizeof(file_buffer[0]))) throw windows_exception("dumbnose::process_util::get_current_process_path():  GetModuleFileName() failed");

		return file_buffer;
	}

	static dumbnose::windows_handle create_process(const std::wstring& exe_path)
	{
		STARTUPINFO si = {0};
		PROCESS_INFORMATION pi = {0};

		if(!CreateProcess(NULL,const_cast<wchar_t*>(exe_path.c_str()),NULL,NULL,FALSE,0,NULL,NULL,&si,&pi)) {
			throw windows_exception("CreateProcess() failed");
		}

		CloseHandle(pi.hThread);
		return pi.hProcess;
	}
};


} // namespace dumbnose