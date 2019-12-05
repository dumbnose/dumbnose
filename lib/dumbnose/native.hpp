//
//
//  Native Nt API Wrapper
//
//

// Make sure windows is included first
#include <windows.h>

//
// Get as many Native API defs as we can
//
namespace Nt {
	extern "C"{
#pragma warning(push)
#pragma warning(disable: 4005)
#include <winternl.h>
NTSTATUS   ZwOpenKey(OUT PHANDLE  KeyHandle, IN ACCESS_MASK  DesiredAccess, IN POBJECT_ATTRIBUTES  ObjectAttributes);
#pragma warning(pop)
	}
}
using Nt::NTSTATUS;


namespace Rtl {
	extern "C"{
		typedef void (NTAPI *InitUnicodeStringFunc)(Nt::PUNICODE_STRING ucs,PCWSTR str);
extern InitUnicodeStringFunc InitUnicodeString;
	}
}

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
