#pragma once


#include <dumbnose/windows_handle.hpp>
#include <dumbnose/scope_guard.hpp>


namespace dumbnose {


class windows_privileges
{
public:
	static void modify_privilege(LPCTSTR privilege, bool enable)
	{
		TOKEN_PRIVILEGES state;
		LUID             luid;
		windows_handle	 token;

		// Open the process token for this process.
		if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&token.handle_ref())) {
			throw windows_exception("Could not open process token");
		}

		// Get the local unique id for the privilege.
		if (!LookupPrivilegeValue(NULL, privilege,&luid)) {
			throw windows_exception("LookupPrivilegeValue failed");
		}

		// Assign values to the TOKEN_PRIVILEGE structure.
		state.PrivilegeCount = 1;
		state.Privileges[0].Luid = luid;
		state.Privileges[0].Attributes = (enable ? SE_PRIVILEGE_ENABLED : 0);

		// Adjust the token privilege.
		if (!AdjustTokenPrivileges(token.handle(),FALSE,&state,sizeof(state),NULL,NULL)) {
			throw windows_exception("AdjustTokenPrivileges failed");
		}
	}
};



} // namespace dumbnose
