// wil_security.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "wilsecurity.hpp"

using namespace dumbnose;

int main()
{
	windows_handle file = CreateFile(L"test.txt", MAXIMUM_ALLOWED, 0, 0, CREATE_ALWAYS, 0, 0);

	/* Wrap up the object in a CSid */
	ATL::sid sid(ATL::sids::System());
	
	std::wcout << L"Account Name: " << sid.account_name() << std::endl
			   << L"Domain Name:  " << sid.domain() << std::endl
			   << L"SID:          " << sid.sid_to_string() << std::endl
		;

	TRUSTEE TrusteeSid = { 0 };
	::BuildTrusteeWithSid(&TrusteeSid, const_cast<SID *>(sid.get_sid()));

    return 0;
}

