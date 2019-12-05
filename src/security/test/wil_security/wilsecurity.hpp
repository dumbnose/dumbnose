// Windows Internal Libraries (wil)  
//  
// wil Usage Guidelines:  
// https://microsoft.sharepoint.com/teams/osg_development/Shared%20Documents/Windows%20Internal%20Libraries%20for%20C++%20Usage%20Guide.docx?web=1  
//  
// wil Discussion Alias (wildisc):  
// http://idwebelements/GroupManagement.aspx?Group=wildisc&Operation=join  (one-click join)  
//  
//! @file  
//! C++ helpers for dealing with OS security features, e.g. security descriptors, SIDs, tokens, etc. 
//!
//! NOTE:  Users of this library need to link to userenv.lib
//!

#pragma once

#ifndef __cplusplus  
#error This file is designed for C++ consumers only  
#endif  


#include <atldef.h>

#pragma warning(push)
#pragma warning(disable: 4625) // copy constructor could not be generated because a base class copy constructor is inaccessible
#pragma warning(disable: 4626) // assignment operator could not be generated because a base class assignment operator is inaccessible
#pragma warning(disable: 4571) //catch(...) blocks compiled with /EHs do NOT catch or re-throw Structured Exceptions
#ifndef _CPPUNWIND
#pragma warning (disable : 4702)	// unreachable code
#endif

#define WIL_SECURITY_PACKING 8
#pragma pack(push,WIL_SECURITY_PACKING)

#include <sddl.h>
#include <userenv.h>
#include <aclapi.h>

#include <vector>

#include <atlcoll.h>
#include <atlstr.h>


namespace ATL
{


// @todo:  Remove this from the file before submitting.  Helpful for 
#pragma comment(lib, "userenv.lib")

// **************************************************************
// sid

class sid
{

public:
	sid() noexcept;

	explicit sid(
		_In_z_ LPCTSTR account_name,
		_In_opt_z_ LPCTSTR system = NULL);
	explicit sid(
		_In_ const SID *sid,
		_In_opt_z_ LPCTSTR system = NULL);
	sid(
		_In_ const SID_IDENTIFIER_AUTHORITY &identifier_authority,
		_In_ BYTE subauthority_count, ...);
	virtual ~sid() noexcept;

	sid(_In_ const sid &rhs);
	sid &operator=(_In_ const sid &rhs);

	sid(_In_ const SID &rhs);
	sid &operator=(_In_ const SID &rhs);

	bool load_account(
		_In_z_ LPCTSTR account_name,
		_In_opt_z_ LPCTSTR system = NULL);
	bool load_account(
		_In_ const SID *sid,
		_In_opt_z_ LPCTSTR system = NULL);

	LPCTSTR account_name() const;
	LPCTSTR domain() const;

	// @todo:  Choose a better name for this.
	LPCTSTR sid_to_string() const;

	const SID *get_sid() const;
	operator const SID *() const;
	SID_NAME_USE sid_name_use() const noexcept;

	UINT length() const noexcept;

	// SID functions
	bool equal_prefix(_In_ const sid &rhs) const noexcept;
	bool equal_prefix(_In_ const SID &rhs) const noexcept;

	const SID_IDENTIFIER_AUTHORITY *get_psid_identifier_authority() const noexcept;
	DWORD get_sub_authority(_In_ DWORD subauthority) const noexcept;
	UCHAR get_sub_authority_count() const noexcept;
	bool is_valid() const noexcept;

private:
	void copy(_In_ const SID &rhs);
	void clear() noexcept;
	void get_account_name_and_domain() const;
	SID* get_psid_() const noexcept;

	BYTE buffer_[SECURITY_MAX_SID_SIZE];
	bool valid_; // true if the CSid has been given a value

	mutable SID_NAME_USE sidnameuse_;
	mutable CString account_name_;
	mutable CString domain_;
	mutable CString sid_;

	CString system_;
};

bool operator==(
	_In_ const sid &lhs,
	_In_ const sid &rhs) noexcept;
bool operator!=(
	_In_ const sid &lhs,
	_In_ const sid &rhs) noexcept;

// sort operations are provided to allow CSids to be put into
// sorted stl collections (stl::[multi]map, stl::[multi]set)
bool operator<(
	_In_ const sid &lhs,
	_In_ const sid &rhs) noexcept;
bool operator>(
	_In_ const sid &lhs,
	_In_ const sid &rhs) noexcept;
bool operator<=(
	_In_ const sid &lhs,
	_In_ const sid &rhs) noexcept;
bool operator>=(
	_In_ const sid &lhs,
	_In_ const sid &rhs) noexcept;

// **************************************************************
// Well-known sids

namespace sids
{
// Universal
sid Null();
sid World();
sid Local();
sid CreatorOwner();
sid CreatorGroup();
sid CreatorOwnerServer();
sid CreatorGroupServer();

// NT Authority
sid Dialup();
sid Network();
sid Batch();
sid Interactive();
sid Service();
sid AnonymousLogon();
sid Proxy();
sid ServerLogon();
sid Self();
sid AuthenticatedUser();
sid RestrictedCode();
sid TerminalServer();
sid System();
sid NetworkService();

// NT Authority\BUILTIN
sid Admins();
sid Users();
sid Guests();
sid PowerUsers();
sid AccountOps();
sid SystemOps();
sid PrintOps();
sid BackupOps();
sid Replicator();
sid RasServers();
sid PreW2KAccess();
} // namespace Sids



#pragma region NotImplementedYet

//***************************************
// CAcl
//		CAce
//
//		CDacl
//			CAccessAce
//
//		CSacl
//			CAuditAce
//***************************************

// **************************************************************
// CAcl

class CAcl
{
public:
	CAcl() noexcept;
	virtual ~CAcl() noexcept;

	CAcl(_In_ const CAcl &rhs);
	CAcl &operator=(_In_ const CAcl &rhs);

	typedef CAtlArray<ACCESS_MASK> CAccessMaskArray;
	typedef CAtlArray<BYTE> CAceTypeArray;
	typedef CAtlArray<BYTE> CAceFlagArray;

	void GetAclEntries(
		_Out_ std::vector<sid> *pSids,
		_Out_opt_ CAccessMaskArray *pAccessMasks = NULL,
		_Out_opt_ CAceTypeArray *pAceTypes = NULL,
		_Out_opt_ CAceFlagArray *pAceFlags = NULL) const;
	void GetAclEntry(
		_In_ UINT nIndex,
		_Inout_opt_ sid* sid,
		_Out_opt_ ACCESS_MASK* pMask = NULL,
		_Out_opt_ BYTE* pType = NULL,
		_Out_opt_ BYTE* pFlags = NULL,
		_Out_opt_ GUID* pObjectType = NULL,
		_Out_opt_ GUID* pInheritedObjectType = NULL) const;

	bool RemoveAces(_In_ const sid &rSid);

	virtual UINT GetAceCount() const noexcept = 0;
	virtual void RemoveAllAces() noexcept = 0;
	virtual void RemoveAce(_In_ UINT nIndex) = 0;

	const ACL *GetPACL() const;
	operator const ACL *() const;
	UINT length() const;

	void SetNull() noexcept;
	void SetEmpty() noexcept;
	bool IsNull() const noexcept;
	bool IsEmpty() const noexcept;

private:
	mutable ACL *m_pAcl;
	bool m_bNull;

protected:
	void Dirty() noexcept;

	class CAce
	{
	public:
		CAce(
			_In_ const sid &rSid,
			_In_ ACCESS_MASK accessmask,
			_In_ BYTE aceflags);
		virtual ~CAce() noexcept;

		CAce(_In_ const CAce &rhs);
		CAce &operator=(_In_ const CAce &rhs);

		virtual void *GetACE() const = 0;
		virtual UINT length() const noexcept = 0;
		virtual BYTE AceType() const noexcept = 0;
		virtual bool IsObjectAce() const noexcept;
		virtual GUID ObjectType() const noexcept;
		virtual GUID InheritedObjectType() const noexcept;

		ACCESS_MASK AccessMask() const noexcept;
		BYTE AceFlags() const noexcept;
		const sid &Sid() const noexcept;

		void AddAccess(_In_ ACCESS_MASK accessmask) noexcept;

	protected:
		sid m_sid;
		ACCESS_MASK m_dwAccessMask;
		BYTE m_aceflags;
		mutable void *m_pAce;
	};

	virtual const CAce *GetAce(_In_ UINT nIndex) const = 0;
	virtual void PrepareAcesForACL() const noexcept;

	DWORD m_dwAclRevision;
};

// ************************************************
// CDacl

class CDacl :
	public CAcl
{
public:
	CDacl() noexcept;
	virtual ~CDacl() noexcept;

	CDacl(_In_ const CDacl &rhs);
	CDacl &operator=(_In_ const CDacl &rhs);

	CDacl(_In_ const ACL &rhs);
	CDacl &operator=(_In_ const ACL &rhs);

	bool AddAllowedAce(
		_In_ const sid &rSid,
		_In_ ACCESS_MASK accessmask,
		_In_ BYTE aceflags = 0);
	bool AddDeniedAce(
		_In_ const sid &rSid,
		_In_ ACCESS_MASK accessmask,
		_In_ BYTE aceflags = 0);
	bool AddAllowedAce(
		_In_ const sid &rSid,
		_In_ ACCESS_MASK accessmask,
		_In_ BYTE aceflags,
		_In_ const GUID *pObjectType,
		_In_ const GUID *pInheritedObjectType);
	bool AddDeniedAce(
		_In_ const sid &rSid,
		_In_ ACCESS_MASK accessmask,
		_In_ BYTE aceflags,
		_In_ const GUID *pObjectType,
		_In_ const GUID *pInheritedObjectType);
	void RemoveAllAces() noexcept;
	void RemoveAce(_In_ UINT nIndex);

	UINT GetAceCount() const noexcept;

private:
	void Copy(_In_ const CDacl &rhs);
	void Copy(_In_ const ACL &rhs);

	class CAccessAce : 
		public CAcl::CAce
	{
	public:
		CAccessAce(
			_In_ const sid &rSid,
			_In_ ACCESS_MASK accessmask,
			_In_ BYTE aceflags,
			_In_ bool bAllowAccess);
		virtual ~CAccessAce() noexcept;

		void *GetACE() const;
		UINT length() const noexcept;
		BYTE AceType() const noexcept;

		bool Allow() const noexcept;
		bool Inherited() const noexcept;

		static int Order(
			_In_ const CAccessAce &lhs,
			_In_ const CAccessAce &rhs) noexcept;

	protected:
		bool m_bAllow;
	};

	class CAccessObjectAce : 
		public CAccessAce
	{
	public:
		CAccessObjectAce(
			_In_ const sid &rSid,
			_In_ ACCESS_MASK accessmask,
			_In_ BYTE aceflags,
			_In_ bool bAllowAccess,
			_In_opt_ const GUID *pObjectType,
			_In_opt_ const GUID *pInheritedObjectType);
		virtual ~CAccessObjectAce() noexcept;

		CAccessObjectAce(_In_ const CAccessObjectAce &rhs);
		CAccessObjectAce &operator=(_In_ const CAccessObjectAce &rhs);

		void *GetACE() const;
		UINT length() const noexcept;
		BYTE AceType() const noexcept;
		bool IsObjectAce() const noexcept;
		virtual GUID ObjectType() const noexcept;
		virtual GUID InheritedObjectType() const noexcept;

	protected:
		GUID *m_pObjectType;
		GUID *m_pInheritedObjectType;
	};

	const CAcl::CAce *GetAce(_In_ UINT nIndex) const;

	void PrepareAcesForACL() const noexcept;

	mutable CAutoPtrArray<CAccessAce> m_acl;
};

//******************************************
// CSacl

class CSacl : 
	public CAcl
{
public:
	CSacl() noexcept;
	virtual ~CSacl() noexcept;

	CSacl(_In_ const CSacl &rhs);
	CSacl &operator=(_In_ const CSacl &rhs);

	CSacl(_In_ const ACL &rhs);
	CSacl &operator=(_In_ const ACL &rhs);

	bool AddAuditAce(
		_In_ const sid &rSid,
		_In_ ACCESS_MASK accessmask,
		_In_ bool bSuccess,
		_In_ bool bFailure,
		_In_ BYTE aceflags = 0);
	bool AddAuditAce(
		_In_ const sid &rSid,
		_In_ ACCESS_MASK accessmask,
		_In_ bool bSuccess,
		_In_ bool bFailure,
		_In_ BYTE aceflags,
		_In_ const GUID *pObjectType,
		_In_ const GUID *pInheritedObjectType);
	void RemoveAllAces() noexcept;
	void RemoveAce(_In_ UINT nIndex);

	UINT GetAceCount() const noexcept;

private:
	void Copy(_In_ const CSacl &rhs);
	void Copy(_In_ const ACL &rhs);

	class CAuditAce : 
		public CAcl::CAce
	{
	public:
		CAuditAce(
			_In_ const sid &rSid,
			_In_ ACCESS_MASK accessmask,
			_In_ BYTE aceflags,
			_In_ bool bAuditSuccess,
			_In_ bool bAuditFailure);
		virtual ~CAuditAce() noexcept;

		void *GetACE() const;
		UINT length() const noexcept;
		BYTE AceType() const noexcept;
	protected:
		bool m_bSuccess;
		bool m_bFailure;
	};

	class CAuditObjectAce :
		public CAuditAce
	{
	public:
		CAuditObjectAce(
			_In_ const sid &rSid,
			_In_ ACCESS_MASK accessmask,
			_In_ BYTE aceflags,
			_In_ bool bAuditSuccess,
			_In_ bool bAuditFailure,
			_In_opt_ const GUID *pObjectType,
			_In_opt_ const GUID *pInheritedObjectType);
		virtual ~CAuditObjectAce() noexcept;

		CAuditObjectAce(_In_ const CAuditObjectAce &rhs);
		CAuditObjectAce &operator=(_In_ const CAuditObjectAce &rhs);

		void *GetACE() const;
		UINT length() const noexcept;
		BYTE AceType() const noexcept;
		bool IsObjectAce() const noexcept;
		virtual GUID ObjectType() const noexcept;
		virtual GUID InheritedObjectType() const noexcept;

	protected:
		GUID *m_pObjectType;
		GUID *m_pInheritedObjectType;
	};

	const CAce *GetAce(_In_ UINT nIndex) const;

	CAutoPtrArray<CAuditAce> m_acl;
};

//******************************************
// CSecurityDesc

class CSecurityDesc
{
public:
	CSecurityDesc() noexcept;
	virtual ~CSecurityDesc() noexcept;

	CSecurityDesc(_In_ const CSecurityDesc &rhs);
	CSecurityDesc &operator=(_In_ const CSecurityDesc &rhs);

	CSecurityDesc(_In_ const SECURITY_DESCRIPTOR &rhs);
	CSecurityDesc &operator=(_In_ const SECURITY_DESCRIPTOR &rhs);

	bool FromString(_In_z_ LPCTSTR pstr);
	bool ToString(
		_In_ CString *pstr,
		_In_ SECURITY_INFORMATION si =
			OWNER_SECURITY_INFORMATION |
			GROUP_SECURITY_INFORMATION |
			DACL_SECURITY_INFORMATION |
			SACL_SECURITY_INFORMATION) const;
	void SetOwner(
		_In_ const sid &sid,
		_In_ bool bDefaulted = false);
	void SetGroup(
		_In_ const sid &sid,
		_In_ bool bDefaulted = false);
	void SetDacl(
		_In_ const CDacl &Dacl,
		_In_ bool bDefaulted = false);
	void SetDacl(
		_In_ bool bPresent,
		_In_ bool bDefaulted = false);
	void SetSacl(
		_In_ const CSacl &Sacl,
		_In_ bool bDefaulted = false);
	_Success_(return != false) bool GetOwner(
		_Out_ sid *sid,
		_Out_opt_ bool *pbDefaulted = NULL) const;
	_Success_(return != false) bool GetGroup(
		_Out_ sid *sid,
		_Out_opt_ bool *pbDefaulted = NULL) const;
	_Success_(return != false) bool GetDacl(
		_Out_ CDacl *pDacl,
		_Out_opt_ bool *pbPresent = NULL,
		_Out_opt_ bool *pbDefaulted = NULL) const;
	_Success_(return != false) bool GetSacl(
		_Out_ CSacl *pSacl,
		_Out_opt_ bool *pbPresent = NULL,
		_Out_opt_ bool *pbDefaulted = NULL) const;

	bool IsDaclDefaulted() const noexcept;
	bool IsDaclPresent() const noexcept;
	bool IsGroupDefaulted() const noexcept;
	bool IsOwnerDefaulted() const noexcept;
	bool IsSaclDefaulted() const noexcept;
	bool IsSaclPresent() const noexcept;
	bool IsSelfRelative() const noexcept;

	// Only meaningful on Win2k or better
	bool IsDaclAutoInherited() const noexcept;
	bool IsDaclProtected() const noexcept;
	bool IsSaclAutoInherited() const noexcept;
	bool IsSaclProtected() const noexcept;

	const SECURITY_DESCRIPTOR *GetPSECURITY_DESCRIPTOR() const noexcept;
	operator const SECURITY_DESCRIPTOR *() const noexcept;

	void GetSECURITY_DESCRIPTOR(
		_Out_ SECURITY_DESCRIPTOR *pSD,
		_Inout_ LPDWORD lpdwBufferLength);

	UINT length() noexcept;

	bool GetControl(_Out_ SECURITY_DESCRIPTOR_CONTROL *psdc) const noexcept;
	bool SetControl(
		_In_ SECURITY_DESCRIPTOR_CONTROL ControlBitsOfInterest,
		_In_ SECURITY_DESCRIPTOR_CONTROL ControlBitsToSet) noexcept;

	void MakeSelfRelative();
	void MakeAbsolute();

protected:
	virtual void Clear() noexcept;
	_At_(this->m_pSecurityDescriptor, _Post_notnull_ _Post_writable_size_(1))
	void AllocateAndInitializeSecurityDescriptor();
	void Init(_In_ const SECURITY_DESCRIPTOR &rhs);

	SECURITY_DESCRIPTOR *m_pSecurityDescriptor;
};

// **************************************************************
// CSecurityAttributes

class CSecurityAttributes :
	public SECURITY_ATTRIBUTES
{
public:
	CSecurityAttributes() noexcept;
	explicit CSecurityAttributes(
		_In_ const CSecurityDesc &rSecurityDescriptor,
		_In_ bool bInheritsHandle = false);

	void Set(
		_In_ const CSecurityDesc &rSecurityDescriptor,
		_In_ bool bInheritsHandle = false);

protected:
	CSecurityDesc m_SecurityDescriptor;
};

template<>
class CElementTraits< LUID > :
	public CElementTraitsBase< LUID >
{
public:
	typedef const LUID& INARGTYPE;
	typedef LUID& OUTARGTYPE;

	static ULONG Hash(_In_ INARGTYPE luid) noexcept
	{
		return luid.HighPart ^ luid.LowPart;
	}

	static BOOL CompareElements(
		_In_ INARGTYPE element1,
		_In_ INARGTYPE element2) noexcept
	{
		return element1.HighPart == element2.HighPart && element1.LowPart == element2.LowPart;
	}

	static int CompareElementsOrdered(
		_In_ INARGTYPE element1,
		_In_ INARGTYPE element2 ) noexcept
	{
		_LARGE_INTEGER li1, li2;
		li1.LowPart = element1.LowPart;
		li1.HighPart = element1.HighPart;
		li2.LowPart = element2.LowPart;
		li2.HighPart = element2.HighPart;

		if( li1.QuadPart > li2.QuadPart )
			return( 1 );
		else if( li1.QuadPart < li2.QuadPart )
			return( -1 );

		return( 0 );
	}
};

typedef CAtlArray<LUID> CLUIDArray;

//******************************************************
// CTokenPrivileges

class CTokenPrivileges
{
public:
	CTokenPrivileges() noexcept;
	virtual ~CTokenPrivileges() noexcept;

	CTokenPrivileges(_In_ const CTokenPrivileges &rhs);
	CTokenPrivileges &operator=(_In_ const CTokenPrivileges &rhs);

	CTokenPrivileges(_In_ const TOKEN_PRIVILEGES &rPrivileges);
	CTokenPrivileges &operator=(_In_ const TOKEN_PRIVILEGES &rPrivileges);

	void Add(_In_ const TOKEN_PRIVILEGES &rPrivileges);
	bool Add(
		_In_z_ LPCTSTR pszPrivilege,
		_In_ bool bEnable);

	typedef CAtlArray<CString> CNames;
	typedef CAtlArray<DWORD> CAttributes;

	_Success_(return != false) bool LookupPrivilege(
		_In_z_ LPCTSTR pszPrivilege,
		_Out_opt_ DWORD *pdwAttributes = NULL) const;
	void GetNamesAndAttributes(
		_Inout_ CNames *pNames,
		_Inout_opt_ CAttributes *pAttributes = NULL) const;
	void GetDisplayNames(_Inout_ CNames *pDisplayNames) const;
	void GetLuidsAndAttributes(
		_Inout_ CLUIDArray *pPrivileges,
		_Inout_opt_ CAttributes *pAttributes = NULL) const;

	bool Delete(_In_z_ LPCTSTR pszPrivilege) noexcept;
	void DeleteAll() noexcept;

	UINT GetCount() const noexcept;
	UINT length() const noexcept;

	const TOKEN_PRIVILEGES *GetPTOKEN_PRIVILEGES() const;
	operator const TOKEN_PRIVILEGES *() const;

private:
	typedef CAtlMap<LUID, DWORD> Map;
	Map m_mapTokenPrivileges;
	mutable TOKEN_PRIVILEGES *m_pTokenPrivileges;
	bool m_bDirty;

	void AddPrivileges(_In_ const TOKEN_PRIVILEGES &rPrivileges);
};

//******************************************************
// CTokenGroups

class CTokenGroups
{
public:
	CTokenGroups() noexcept;
	virtual ~CTokenGroups() noexcept;

	CTokenGroups(_In_ const CTokenGroups &rhs);
	CTokenGroups &operator=(_In_ const CTokenGroups &rhs);

	CTokenGroups(_In_ const TOKEN_GROUPS &rhs);
	CTokenGroups &operator=(_In_ const TOKEN_GROUPS &rhs);

	void Add(_In_ const TOKEN_GROUPS &rTokenGroups);
	void Add(_In_ const sid &rSid, _In_ DWORD dwAttributes);

	_Success_(return != false) bool LookupSid(
		_In_ const sid &rSid,
		_Out_opt_ DWORD *pdwAttributes = NULL) const noexcept;
	void GetSidsAndAttributes(
		_Inout_ std::vector<sid> *pSids,
		_Inout_opt_ CAtlArray<DWORD> *pAttributes = NULL) const;

	bool Delete(_In_ const sid &rSid) noexcept;
	void DeleteAll() noexcept;

	UINT GetCount() const noexcept;
	UINT length() const noexcept;

	const TOKEN_GROUPS *GetPTOKEN_GROUPS() const;
	operator const TOKEN_GROUPS *() const;

private:
	class CTGElementTraits :
		public CElementTraitsBase< sid >
	{
	public:
		static UINT Hash(_In_ const sid &sid) noexcept
		{
			return sid.get_sub_authority(sid.get_sub_authority_count() - 1);
		}

		static bool CompareElements(
			_In_ INARGTYPE element1,
			_In_ INARGTYPE element2 ) noexcept
		{
			return( element1 == element2 );
		}
	};

	typedef CAtlMap<sid, DWORD, CTGElementTraits> Map;
	Map m_mapTokenGroups;
	mutable TOKEN_GROUPS *m_pTokenGroups;
	mutable bool m_bDirty;

	void AddTokenGroups(_In_ const TOKEN_GROUPS &rTokenGroups);
};

// *************************************
// CAccessToken

class CAccessToken
{
public:
	CAccessToken() noexcept;
	virtual ~CAccessToken() noexcept;

	void Attach(_In_ HANDLE hToken) noexcept;
	HANDLE Detach() noexcept;
	HANDLE GetHandle() const noexcept;
	HKEY HKeyCurrentUser() const noexcept;

	// Privileges    
	_Success_(return != false) bool EnablePrivilege(
		_In_z_ LPCTSTR pszPrivilege,
		_In_opt_ CTokenPrivileges *pPreviousState = NULL,
		_Out_opt_ bool* pbErrNotAllAssigned=NULL);    
	_Success_(return != false) bool EnablePrivileges(
		_In_ const CAtlArray<LPCTSTR> &rPrivileges,
		_Inout_opt_ CTokenPrivileges *pPreviousState = NULL,
		_Out_opt_ bool* pbErrNotAllAssigned=NULL);    
	_Success_(return != false) bool DisablePrivilege(
		_In_z_ LPCTSTR pszPrivilege,
		_In_opt_ CTokenPrivileges *pPreviousState = NULL,
		_Out_opt_ bool* pbErrNotAllAssigned=NULL);    
	_Success_(return != false) bool DisablePrivileges(
		_In_ const CAtlArray<LPCTSTR> &rPrivileges,
		_Inout_opt_ CTokenPrivileges *pPreviousState = NULL,
		_Out_opt_ bool* pbErrNotAllAssigned=NULL);
	_Success_(return != false) bool EnableDisablePrivileges(
		_In_ const CTokenPrivileges &rPrivilenges,
		_Inout_opt_ CTokenPrivileges *pPreviousState = NULL,
		_Out_opt_ bool* pbErrNotAllAssigned=NULL);
	_Success_(return != false) bool PrivilegeCheck(
		_In_ PPRIVILEGE_SET RequiredPrivileges,
		_Out_ bool *pbResult) const noexcept;

	bool GetLogonSid(_Inout_ sid *sid) const;
	_Success_(return != false) bool GetTokenId(_Out_ LUID *pluid) const;
	_Success_(return != false) bool GetLogonSessionId(_Out_ LUID *pluid) const;

	bool CheckTokenMembership(
		_In_ const sid &rSid,
		_Inout_ bool *pbIsMember) const;
	bool IsTokenRestricted() const noexcept;

	// Token Information
protected:
	void InfoTypeToRetType(
		_Inout_ sid *pRet,
		_In_ const TOKEN_USER &rWork) const
	{
		ATLENSURE(pRet);
		*pRet = *static_cast<SID *>(rWork.User.Sid);
	}

	void InfoTypeToRetType(
		_Inout_ CTokenGroups *pRet,
		_In_ const TOKEN_GROUPS &rWork) const
	{
		ATLENSURE(pRet);
		*pRet = rWork;
	}

	void InfoTypeToRetType(
		_Inout_ CTokenPrivileges *pRet,
		_In_ const TOKEN_PRIVILEGES &rWork) const
	{
		ATLENSURE(pRet);
		*pRet = rWork;
	}

	void InfoTypeToRetType(
		_Inout_ sid *pRet,
		_In_ const TOKEN_OWNER &rWork) const
	{
		ATLENSURE(pRet);
		*pRet = *static_cast<SID *>(rWork.Owner);
	}

	void InfoTypeToRetType(
		_Inout_ sid *pRet,
		_In_ const TOKEN_PRIMARY_GROUP &rWork) const
	{
		ATLENSURE(pRet);
		*pRet = *static_cast<SID *>(rWork.PrimaryGroup);
	}

	void InfoTypeToRetType(
		_Inout_ CDacl *pRet,
		_In_ const TOKEN_DEFAULT_DACL &rWork) const
	{
		ATLENSURE(pRet);
		*pRet = *rWork.DefaultDacl;
	}

	template<typename RET_T, typename INFO_T>
	_Success_(return != false) bool GetInfoConvert(
		_Inout_ RET_T *pRet,
		_In_ TOKEN_INFORMATION_CLASS TokenClass,
		_Out_opt_ INFO_T *pWork = NULL) const
	{
		ATLASSERT(pRet);
		if(!pRet)
			return false;

		DWORD dwLen;
		::GetTokenInformation(m_hToken, TokenClass, NULL, 0, &dwLen);
		if(::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return false;

		USES_ATL_SAFE_ALLOCA;
		pWork = static_cast<INFO_T *>(_ATL_SAFE_ALLOCA(dwLen, _ATL_SAFE_ALLOCA_DEF_THRESHOLD));
		if (pWork == NULL)
			return false;
		if(!::GetTokenInformation(m_hToken, TokenClass, pWork, dwLen, &dwLen))
			return false;

		InfoTypeToRetType(pRet, *pWork);
		return true;
	}

	template<typename RET_T>
	bool GetInfo(
		_Inout_ RET_T *pRet,
		_In_ TOKEN_INFORMATION_CLASS TokenClass) const
	{
		ATLASSERT(pRet);
		if(!pRet)
			return false;

		DWORD dwLen;
		if(!::GetTokenInformation(m_hToken, TokenClass, pRet, sizeof(RET_T), &dwLen))
			return false;
		return true;
	}

public:
	bool GetDefaultDacl(_Inout_ CDacl *pDacl) const;
	bool GetGroups(_Inout_ CTokenGroups *pGroups) const;
	bool GetImpersonationLevel(_Inout_ SECURITY_IMPERSONATION_LEVEL *pImpersonationLevel) const;
	bool GetOwner(_Inout_ sid *sid) const;
	bool GetPrimaryGroup(_Inout_ sid *sid) const;
	bool GetPrivileges(_Inout_ CTokenPrivileges *pPrivileges) const;
	bool GetTerminalServicesSessionId(_Inout_ DWORD *pdwSessionId) const;
	bool GetSource(_Inout_ TOKEN_SOURCE *pSource) const;
	bool GetStatistics(_Inout_ TOKEN_STATISTICS *pStatistics) const;
	bool GetType(_Inout_ TOKEN_TYPE *pType) const;
	bool GetUser(_Inout_ sid *sid) const;

	bool SetOwner(_In_ const sid &rSid);
	bool SetPrimaryGroup(_In_ const sid &rSid);
	bool SetDefaultDacl(_In_ const CDacl &rDacl);

	bool CreateImpersonationToken(
		_Inout_ CAccessToken *pImp,
		_In_ SECURITY_IMPERSONATION_LEVEL sil = SecurityImpersonation) const;
	bool CreatePrimaryToken(
		_Inout_ CAccessToken *pPri,
		_In_ DWORD dwDesiredAccess = MAXIMUM_ALLOWED,
		_In_opt_ const CSecurityAttributes *pTokenAttributes = NULL) const;

	bool CreateRestrictedToken(
		_Inout_ CAccessToken *pRestrictedToken,
		_In_ const CTokenGroups &SidsToDisable,
		_In_ const CTokenGroups &SidsToRestrict,
		_In_ const CTokenPrivileges &PrivilegesToDelete = CTokenPrivileges()) const;

	// Token API type functions
	bool GetProcessToken(
		_In_ DWORD dwDesiredAccess,
		_In_opt_ HANDLE hProcess = NULL) noexcept;
	bool GetThreadToken(
		_In_ DWORD dwDesiredAccess,
		_In_opt_ HANDLE hThread = NULL,
		_In_ bool bOpenAsSelf = true) noexcept;
	bool GetEffectiveToken(_In_ DWORD dwDesiredAccess) noexcept;

	bool OpenThreadToken(
		_In_ DWORD dwDesiredAccess,
		_In_ bool bImpersonate = false,
		_In_ bool bOpenAsSelf = true,
		_In_ SECURITY_IMPERSONATION_LEVEL sil = SecurityImpersonation);

	bool OpenCOMClientToken(
		_In_ DWORD dwDesiredAccess,
		_In_ bool bImpersonate = false,
		_In_ bool bOpenAsSelf = true);

	bool OpenNamedPipeClientToken(
		_In_ HANDLE hPipe,
		_In_ DWORD dwDesiredAccess,
		_In_ bool bImpersonate = false,
		_In_ bool bOpenAsSelf = true);
	bool OpenRPCClientToken(
		_In_ RPC_BINDING_HANDLE BindingHandle,
		_In_ DWORD dwDesiredAccess,
		_In_ bool bImpersonate = false,
		_In_ bool bOpenAsSelf = true);

	bool ImpersonateLoggedOnUser() const;
	bool Impersonate(_In_opt_ HANDLE hThread = NULL) const;
	bool Revert(_In_opt_ HANDLE hThread = NULL) const noexcept;

	bool LoadUserProfile();
	HANDLE GetProfile() const noexcept;

	// Must hold Tcb privilege
	bool LogonUser(
		_In_z_ LPCTSTR pszUserName,
		_In_z_ LPCTSTR pszDomain,
		_In_z_ LPCTSTR pszPassword,
		_In_ DWORD dwLogonType = LOGON32_LOGON_INTERACTIVE,
		_In_ DWORD dwLogonProvider = LOGON32_PROVIDER_DEFAULT) noexcept;

	// Must hold AssignPrimaryToken (unless restricted token) and
	// IncreaseQuota privileges
	bool CreateProcessAsUser(
		_In_opt_z_ LPCTSTR pApplicationName,
		_In_opt_z_ LPTSTR pCommandLine,
		_In_ LPPROCESS_INFORMATION pProcessInformation,
		_In_ LPSTARTUPINFO pStartupInfo,
		_In_ DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS,
		_In_ bool bLoadProfile = false,
		_In_opt_ const CSecurityAttributes *pProcessAttributes = NULL,
		_In_opt_ const CSecurityAttributes *pThreadAttributes = NULL,
		_In_ bool bInherit = false,
		_In_opt_z_ LPCTSTR pCurrentDirectory = NULL) noexcept;

protected:
	_Success_(return != false) bool EnableDisablePrivileges(
		_In_ const CAtlArray<LPCTSTR> &rPrivileges,
		_In_ bool bEnable,
		_Inout_opt_ CTokenPrivileges *pPreviousState,
		_Out_opt_ bool* pbErrNotAllAssigned=NULL);
	bool CheckImpersonation() const noexcept;

	bool RevertToLevel(_In_opt_ SECURITY_IMPERSONATION_LEVEL *pSil) const noexcept;

	virtual void Clear() noexcept;

	HANDLE m_hToken, m_hProfile;

private:
	CAccessToken(_In_ const CAccessToken &rhs);
	CAccessToken &operator=(_In_ const CAccessToken &rhs);

	class CRevert
	{
	public:
		virtual bool Revert() noexcept = 0;
	};

	class CRevertToSelf : 
		public CRevert
	{
	public:
		bool Revert() noexcept
		{
			return 0 != ::RevertToSelf();
		}
	};

	class CCoRevertToSelf :
		public CRevert
	{
	public:
		bool Revert() noexcept
		{
			return SUCCEEDED(::CoRevertToSelf());
		}
	};

	class CRpcRevertToSelfEx : 
		public CRevert
	{
	public:
		CRpcRevertToSelfEx(_In_ RPC_BINDING_HANDLE BindingHandle) noexcept :
			m_hBinding(BindingHandle)
		{
		}
		bool Revert() noexcept
		{
			return RPC_S_OK == ::RpcRevertToSelfEx(m_hBinding);
		}

	private:
		RPC_BINDING_HANDLE m_hBinding;
	};
	mutable CRevert *m_pRevert;
};

//*******************************************
// CAutoRevertImpersonation

class CAutoRevertImpersonation
{
public:
	CAutoRevertImpersonation(_In_ const CAccessToken* pAT) noexcept;
	~CAutoRevertImpersonation() noexcept;

	void Attach(_In_ const CAccessToken* pAT) noexcept;
	const CAccessToken* Detach() noexcept;

	const CAccessToken* GetAccessToken() noexcept;

private:
	const CAccessToken* m_pAT;

	CAutoRevertImpersonation(_In_ const CAutoRevertImpersonation &rhs);
	CAutoRevertImpersonation &operator=(_In_ const CAutoRevertImpersonation &rhs);
};

//*******************************************
// CPrivateObjectSecurityDesc

class CPrivateObjectSecurityDesc : 
	public CSecurityDesc
{
public:
	CPrivateObjectSecurityDesc() noexcept;
	~CPrivateObjectSecurityDesc() noexcept;

	bool Create(
		_In_opt_ const CSecurityDesc *pParent,
		_In_opt_ const CSecurityDesc *pCreator,
		_In_ bool bIsDirectoryObject,
		_In_ const CAccessToken &Token,
		_In_ PGENERIC_MAPPING GenericMapping) noexcept;

	bool Create(
		_In_opt_ const CSecurityDesc *pParent,
		_In_opt_ const CSecurityDesc *pCreator,
		_In_opt_ GUID *ObjectType,
		_In_ bool bIsContainerObject,
		_In_ ULONG AutoInheritFlags,
		_In_ const CAccessToken &Token,
		_In_ PGENERIC_MAPPING GenericMapping) noexcept;

	_Success_(return != false) bool Get(
		_In_ SECURITY_INFORMATION si,
		_Inout_ CSecurityDesc *pResult) const noexcept;

	bool Set(
		_In_ SECURITY_INFORMATION si,
		_In_ const CSecurityDesc &Modification,
		_In_ PGENERIC_MAPPING GenericMapping,
		_In_ const CAccessToken &Token) noexcept;

	bool Set(
		_In_ SECURITY_INFORMATION si,
		_In_ const CSecurityDesc &Modification,
		_In_ ULONG AutoInheritFlags,
		_In_ PGENERIC_MAPPING GenericMapping,
		_In_ const CAccessToken &Token) noexcept;

	bool ConvertToAutoInherit(
		_In_opt_ const CSecurityDesc *pParent,
		_In_opt_ GUID *ObjectType,
		_In_ bool bIsDirectoryObject,
		_In_ PGENERIC_MAPPING GenericMapping) noexcept;

protected:
	void Clear() noexcept;

private:
	bool m_bPrivate;

	CPrivateObjectSecurityDesc(_In_ const CPrivateObjectSecurityDesc &rhs);
	CPrivateObjectSecurityDesc &operator=(_In_ const CPrivateObjectSecurityDesc &rhs);
};
//
// **************************************************************
// CSid implementation

#pragma endregion


inline sid::sid() noexcept :
	valid_(false),
	sidnameuse_(SidTypeInvalid)
{
}

inline sid::sid(
		_In_z_ LPCTSTR account_name,
		_In_opt_z_ LPCTSTR system /* = NULL */) :
	valid_(false),
	sidnameuse_(SidTypeInvalid)
{
	if(!load_account(account_name, system))
		AtlThrowLastWin32();
}

inline sid::sid(
		_In_ const SID *sid,
		_In_opt_z_ LPCTSTR system /* = NULL */) :
	valid_(false),
	sidnameuse_(SidTypeInvalid)
{
	if(!load_account(sid, system))
		AtlThrowLastWin32();
}

#pragma warning(push)
#pragma warning(disable : 4793)
inline sid::sid(
		_In_ const SID_IDENTIFIER_AUTHORITY &identifier_authority,
		_In_ BYTE subauthority_count,
		...) :
	valid_(false),
	sidnameuse_(SidTypeInvalid)
{
	BYTE buffer[SECURITY_MAX_SID_SIZE];
	SID *sid = reinterpret_cast<SID*>(buffer);

	ATLASSERT(subauthority_count);
	if(!subauthority_count || ::GetSidLengthRequired(subauthority_count) > SECURITY_MAX_SID_SIZE)
		AtlThrow(E_INVALIDARG);

	if(!::InitializeSid(sid,
		const_cast<SID_IDENTIFIER_AUTHORITY *>(&identifier_authority),
		subauthority_count))
	{
		AtlThrowLastWin32();
	}

	va_list args;
	va_start(args, subauthority_count);
	for(UINT i = 0; i < subauthority_count; i++)
		*::GetSidSubAuthority(sid, i) = va_arg(args, DWORD);
	va_end(args);

	copy(*sid);

	sidnameuse_ = SidTypeUnknown;
}
#pragma warning(pop)

inline sid::~sid() noexcept
{
}

inline sid::sid(_In_ const sid &rhs) :
	sidnameuse_(rhs.sidnameuse_),
	valid_(rhs.valid_),
	account_name_(rhs.account_name_),
	domain_(rhs.domain_),
	sid_(rhs.sid_)
{
	if (!rhs.valid_)
		return;

	if(!rhs.is_valid())
		AtlThrow(E_INVALIDARG);

	if(!::CopySid(rhs.length(), get_psid_(), rhs.get_psid_()))
	{
		HRESULT hr = AtlHresultFromLastError();
		AtlThrow(hr);
	}
}

inline sid &sid::operator=(_In_ const sid &rhs)
{
	if(this != &rhs)
	{
		sidnameuse_ = rhs.sidnameuse_;
		account_name_ = rhs.account_name_;
		domain_ = rhs.domain_;
		sid_ = rhs.sid_;
		valid_ = rhs.valid_;

		if (valid_)
		{
			if(!::CopySid(rhs.length(), get_psid_(), rhs.get_psid_()))
			{
				HRESULT hr = AtlHresultFromLastError();
				valid_ = false;
				AtlThrow(hr);
			}
		}
	}
	return *this;
}

inline sid::sid(_In_ const SID &rhs) :
	valid_(false),
	sidnameuse_(SidTypeInvalid)
{
	copy(rhs);
}

inline sid &sid::operator=(_In_ const SID &rhs)
{
	if (!valid_ || get_psid_() != &rhs)
	{
		clear();
		copy(rhs);

		sidnameuse_ = SidTypeUnknown;
	}
	return *this;
}

inline bool sid::load_account(
	_In_z_ LPCTSTR account_name,
	_In_opt_z_ LPCTSTR system /* = NULL */)
{
    clear();

	ATLASSERT(account_name);
	if (!account_name)
    {
		return false;
    }

	static const DWORD dwDomainSize = 128; // reasonable to start with
	BYTE buffSid[SECURITY_MAX_SID_SIZE];
	CTempBuffer<TCHAR, dwDomainSize> buffDomain;
	buffDomain.Allocate(dwDomainSize); // just assign the static buffer

	SID *sid = reinterpret_cast<SID *>(buffSid);
	TCHAR *szDomain = static_cast<TCHAR *>(buffDomain);
	DWORD cbSid = SECURITY_MAX_SID_SIZE;
	DWORD cbDomain = dwDomainSize;

	BOOL bSuccess = ::LookupAccountName(system, account_name, sid, &cbSid, szDomain, &cbDomain, &sidnameuse_);
	if (!bSuccess && ::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		// We could have gotten the insufficient buffer error because
		// one or both of sid and szDomain was too small. Check for that
		// here.
		if (cbSid > SECURITY_MAX_SID_SIZE)
		{
			// Should never happen. Fail
			AtlThrow(E_FAIL);
		}

		if (cbDomain > dwDomainSize)
		{
			// Failed because domain was not big enough, reallocate it.
			buffDomain.Reallocate(cbDomain);
			szDomain = static_cast<TCHAR *>(buffDomain);
		}
		bSuccess = ::LookupAccountName(system, account_name, sid, &cbSid, szDomain, &cbDomain, &sidnameuse_);
	}

	if (bSuccess)
	{
		// should be taken care of by ::LookupAccountName
		ATLASSERT(::GetLengthSid(sid) < SECURITY_MAX_SID_SIZE);

		// LookupAccountName doesn't change cbSid on success (although it changes cbDomain)
		valid_ = true;
		if (::CopySid(cbSid, get_psid_(), sid))
		{
			domain_ = szDomain;
			account_name_ = account_name;
			system_ = system;
			return true;
		}
	}

	clear();
	return false;
}

inline bool sid::load_account(
	_In_ const SID *sid,
	_In_opt_z_ LPCTSTR system /* = NULL */)
{
    clear();
	ATLASSERT(sid);

	if(sid)
	{
		_ATLTRY
		{
			system_ = system;
			copy(*sid);
			return true;
		}
		_ATLCATCHALL()
		{
			clear();
			throw;
		}
	}
	return false;
}

inline LPCTSTR sid::account_name() const
{
	if(account_name_.IsEmpty())
		get_account_name_and_domain();
	return account_name_;
}

inline LPCTSTR sid::domain() const
{
	if(domain_.IsEmpty())
		get_account_name_and_domain();
	return domain_;
}

inline LPCTSTR sid::sid_to_string() const
{
	_ATLTRY
	{
		if(sid_.IsEmpty())
		{
#if(_WIN32_WINNT >= 0x0500)
			LPTSTR pszSid;
			if(::ConvertSidToStringSid(get_psid_(), &pszSid))
			{
				sid_ = pszSid;
				::LocalFree(pszSid);
			}
#else
			SID_IDENTIFIER_AUTHORITY *psia = ::GetSidIdentifierAuthority(_GetPSID());
			UINT i;

			if(psia->Value[0] || psia->Value[1])
			{
				m_strSid.Format(_T("S-%d-0x%02hx%02hx%02hx%02hx%02hx%02hx"), SID_REVISION,
					(USHORT)psia->Value[0],
					(USHORT)psia->Value[1],
					(USHORT)psia->Value[2],
					(USHORT)psia->Value[3],
					(USHORT)psia->Value[4],
					(USHORT)psia->Value[5]);
			}
			else
			{
				ULONG nAuthority = 0;
				for(i = 2; i < 6; i++)
				{
					nAuthority <<= 8;
					nAuthority |= psia->Value[i];
				}
				m_strSid.Format(_T("S-%d-%lu"), SID_REVISION, nAuthority);
			}

			UINT subauthority_count = *::GetSidSubAuthorityCount(_GetPSID());
			CString strTemp;
			for(i = 0; i < subauthority_count; i++)
			{
				strTemp.Format(_T("-%lu"), *::GetSidSubAuthority(_GetPSID(), i));
				m_strSid += strTemp;
			}
#endif
		}
		return sid_;
	}
	_ATLCATCHALL()
	{
		sid_.Empty();
		throw;
	}
}

inline const SID *sid::get_sid() const
{
	return get_psid_();
}

inline sid::operator const SID *() const
{
	return get_sid();
}

inline SID_NAME_USE sid::sid_name_use() const noexcept
{
	return sidnameuse_;
}

inline UINT sid::length() const noexcept
{
	ATLASSERT(is_valid());
	return ::GetLengthSid(get_psid_());
}

inline bool sid::equal_prefix(_In_ const sid &rhs) const noexcept
{
	if (get_psid_() == NULL || rhs.get_psid_() == NULL || !is_valid() || !rhs.is_valid())
	{
		return FALSE;
	}
	return 0 != ::EqualPrefixSid(get_psid_(), rhs.get_psid_());
}

inline bool sid::equal_prefix(_In_ const SID &rhs) const noexcept
{
	if (get_psid_() == NULL || !is_valid() || !::IsValidSid(const_cast<SID *>(&rhs)))
	{
		return FALSE;
	}
	return 0 != ::EqualPrefixSid(get_psid_(), const_cast<SID *>(&rhs));
}

inline const SID_IDENTIFIER_AUTHORITY *sid::get_psid_identifier_authority() const noexcept
{
	ATLASSERT(is_valid());
	return ::GetSidIdentifierAuthority(get_psid_());
}

inline DWORD sid::get_sub_authority(_In_ DWORD subauthority) const noexcept
{
	ATLASSERT(is_valid());
	return *::GetSidSubAuthority(get_psid_(), subauthority);
}

inline UCHAR sid::get_sub_authority_count() const noexcept
{
	ATLASSERT(is_valid());
	return *::GetSidSubAuthorityCount(get_psid_());
}

inline bool sid::is_valid() const noexcept
{
	if (!valid_)
		return false;
	return 0 != ::IsValidSid(get_psid_());
}

inline void sid::clear() noexcept
{
	sidnameuse_ = SidTypeInvalid;
	account_name_.Empty();
	domain_.Empty();
	sid_.Empty();
	system_.Empty();
	valid_ = false;
}

inline void sid::copy(_In_ const SID &rhs)
{
	// This function assumes everything is cleaned up/initialized
	// (with the exception of m_strSystem).
	// It does some sanity checking to prevent memory leaks, but
	// you should clean up all members of CSid before calling this
	// function.  (i.e., results are unpredictable on error)

	ATLASSUME(sidnameuse_ == SidTypeInvalid);
	ATLASSUME(account_name_.IsEmpty());
	ATLASSUME(domain_.IsEmpty());
	ATLASSUME(sid_.IsEmpty());

	SID *p = const_cast<SID *>(&rhs);
	if(!::IsValidSid(p))
		AtlThrow(E_INVALIDARG);

	DWORD dwLengthSid = ::GetLengthSid(p);
	if (dwLengthSid > SECURITY_MAX_SID_SIZE)
		AtlThrow(E_INVALIDARG);

	valid_ = true;
	if(!::CopySid(dwLengthSid, get_psid_(), p))
	{
		HRESULT hr = AtlHresultFromLastError();
		valid_ = false;
		AtlThrow(hr);
	}
}

inline void sid::get_account_name_and_domain() const
{
	static const DWORD dwMax = 32;
	DWORD cbName = dwMax, cbDomain = dwMax;
	TCHAR szName[dwMax], szDomain[dwMax];

#pragma warning(push)
#pragma warning(disable: 6202)
	/* Prefast false warning: we do not use cbName or cbDomain as char buffers when call LookupAccountSid.*/
	if(::LookupAccountSid(system_, get_psid_(), szName, &cbName, szDomain, &cbDomain, &sidnameuse_))
#pragma warning(pop)
	{
		account_name_ = szName;
		domain_ = szDomain;
	}
	else
	{
		switch(::GetLastError())
		{
		case ERROR_INSUFFICIENT_BUFFER:
		{
			LPTSTR pszName = account_name_.GetBuffer(cbName);
			LPTSTR pszDomain = domain_.GetBuffer(cbDomain);

			if (!::LookupAccountSid(system_, get_psid_(), pszName, &cbName, pszDomain, &cbDomain, &sidnameuse_))
				AtlThrowLastWin32();

			account_name_.ReleaseBuffer();
			domain_.ReleaseBuffer();
			break;
		}

		case ERROR_NONE_MAPPED:
			account_name_.Empty();
			domain_.Empty();
			sidnameuse_ = SidTypeUnknown;
			break;

		default:
			ATLASSERT(FALSE);
		}
	}
}

inline SID* sid::get_psid_() const noexcept
{
	ATLASSUME(valid_);
	return reinterpret_cast<SID*>(const_cast<BYTE*>(buffer_));
}

inline bool operator==(_In_ const sid &lhs, _In_ const sid &rhs) noexcept
{
	if( lhs.get_sid() == NULL || rhs.get_sid() == NULL || !lhs.is_valid() || !rhs.is_valid() )
		return false;
	return 0 != ::EqualSid(const_cast<SID*>(lhs.get_sid()), const_cast<SID*>(rhs.get_sid()));
}

inline bool operator!=(_In_ const sid &lhs, _In_ const sid &rhs) noexcept
{
	return !(lhs == rhs);
}

inline bool operator<(_In_ const sid &lhs, _In_ const sid &rhs) noexcept
{
	// all other ordered comparisons are done in terms of this one
	const SID_IDENTIFIER_AUTHORITY* la = lhs.get_psid_identifier_authority();
	const SID_IDENTIFIER_AUTHORITY* ra = rhs.get_psid_identifier_authority();

	for (int i=0; i<6; i++)
	{
		if (la->Value[i] < ra->Value[i])
		{
			return true;
		}
		else if (la->Value[i] > ra->Value[i])
		{
			return false;
		}
	}

	for (UCHAR i=0; i<rhs.get_sub_authority_count(); i++)
	{
		if (lhs.get_sub_authority_count() == i)
		{
			// lhs is a prefix of rhs
			return true;
		}

		if (lhs.get_sub_authority(i) < rhs.get_sub_authority(i))
		{
			return true;
		}
		else if (lhs.get_sub_authority(i) > rhs.get_sub_authority(i))
		{
			return false;
		}
	}

	return false;
}

inline bool operator>(
	_In_ const sid &lhs,
	_In_ const sid &rhs) noexcept
{
	return (rhs < lhs);
}

inline bool operator<=(
	_In_ const sid &lhs,
	_In_ const sid &rhs) noexcept
{
	return !(rhs < lhs);
}

inline bool operator>=(
	_In_ const sid &lhs,
	_In_ const sid &rhs) noexcept
{
	return !(lhs < rhs);
}

// **************************************************************
// Well-known sids

namespace sids
{
__declspec(selectany) extern const SID_IDENTIFIER_AUTHORITY
	SecurityNullSidAuthority		= SECURITY_NULL_SID_AUTHORITY,
	SecurityWorldSidAuthority		= SECURITY_WORLD_SID_AUTHORITY,
	SecurityLocalSidAuthority		= SECURITY_LOCAL_SID_AUTHORITY,
	SecurityCreatorSidAuthority		= SECURITY_CREATOR_SID_AUTHORITY,
	SecurityNonUniqueAuthority		= SECURITY_NON_UNIQUE_AUTHORITY,
	SecurityNTAuthority				= SECURITY_NT_AUTHORITY;

// Universal
inline sid Null()
{
	return sid(SecurityNullSidAuthority, 1, SECURITY_NULL_RID);
}
inline sid World()
{
	return sid(SecurityWorldSidAuthority, 1, SECURITY_WORLD_RID);
}
inline sid Local()
{
	return sid(SecurityLocalSidAuthority, 1, SECURITY_LOCAL_RID);
}
inline sid CreatorOwner()
{
	return sid(SecurityCreatorSidAuthority, 1, SECURITY_CREATOR_OWNER_RID);
}
inline sid CreatorGroup()
{
	return sid(SecurityCreatorSidAuthority, 1, SECURITY_CREATOR_GROUP_RID);
}
inline sid CreatorOwnerServer()
{
	return sid(SecurityCreatorSidAuthority, 1, SECURITY_CREATOR_OWNER_SERVER_RID);
}
inline sid CreatorGroupServer()
{
	return sid(SecurityCreatorSidAuthority, 1, SECURITY_CREATOR_GROUP_SERVER_RID);
}

// NT Authority
inline sid Dialup()
{
	return sid(SecurityNTAuthority, 1, SECURITY_DIALUP_RID);
}
inline sid Network()
{
	return sid(SecurityNTAuthority, 1, SECURITY_NETWORK_RID);
}
inline sid Batch()
{
	return sid(SecurityNTAuthority, 1, SECURITY_BATCH_RID);
}
inline sid Interactive()
{
	return sid(SecurityNTAuthority, 1, SECURITY_INTERACTIVE_RID);
}
inline sid Service()
{
	return sid(SecurityNTAuthority, 1, SECURITY_SERVICE_RID);
}
inline sid AnonymousLogon()
{
	return sid(SecurityNTAuthority, 1, SECURITY_ANONYMOUS_LOGON_RID);
}
inline sid Proxy()
{
	return sid(SecurityNTAuthority, 1, SECURITY_PROXY_RID);
}
inline sid ServerLogon()
{
	return sid(SecurityNTAuthority, 1, SECURITY_SERVER_LOGON_RID);
}
inline sid Self()
{
	return sid(SecurityNTAuthority, 1, SECURITY_PRINCIPAL_SELF_RID);
}
inline sid AuthenticatedUser()
{
	return sid(SecurityNTAuthority, 1, SECURITY_AUTHENTICATED_USER_RID);
}
inline sid RestrictedCode()
{
	return sid(SecurityNTAuthority, 1, SECURITY_RESTRICTED_CODE_RID);
}
inline sid TerminalServer()
{
	return sid(SecurityNTAuthority, 1, SECURITY_TERMINAL_SERVER_RID);
}
inline sid System()
{
	return sid(SecurityNTAuthority, 1, SECURITY_LOCAL_SYSTEM_RID);
}


inline sid NetworkService()
{
	return sid(SecurityNTAuthority, 1, SECURITY_NETWORK_SERVICE_RID);

}

// NT Authority\BUILTIN
inline sid Admins()
{
	return sid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS);
}
inline sid Users()
{
	return sid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS);
}
inline sid Guests()
{
	return sid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_GUESTS);
}
inline sid PowerUsers()
{
	return sid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_POWER_USERS);
}
inline sid AccountOps()
{
	return sid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ACCOUNT_OPS);
}
inline sid SystemOps()
{
	return sid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_SYSTEM_OPS);
}
inline sid PrintOps()
{
	return sid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_PRINT_OPS);
}
inline sid BackupOps()
{
	return sid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_BACKUP_OPS);
}
inline sid Replicator()
{
	return sid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_REPLICATOR);
}
inline sid RasServers()
{
	return sid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_RAS_SERVERS);
}
inline sid PreW2KAccess()
{
	return sid(SecurityNTAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_PREW2KCOMPACCESS);
}
} // namespace Sids

#pragma region NotImplementedYet

  //
// **************************************************************
// CAcl implementation

inline CAcl::CAcl() noexcept :
	m_pAcl(NULL),
	m_bNull(false),
	m_dwAclRevision(ACL_REVISION)
{
}

inline CAcl::~CAcl() noexcept
{
	free(m_pAcl);
}

inline CAcl::CAcl(_In_ const CAcl &rhs) :
	m_pAcl(NULL),
	m_bNull(rhs.m_bNull),
	m_dwAclRevision(rhs.m_dwAclRevision)
{
}

inline CAcl &CAcl::operator=(_In_ const CAcl &rhs)
{
	if(this != &rhs)
	{
		free(m_pAcl);
		m_pAcl = NULL;
		m_bNull = rhs.m_bNull;
		m_dwAclRevision = rhs.m_dwAclRevision;
	}
	return *this;
}

inline void CAcl::GetAclEntries(
	_Out_ std::vector<sid> *pSids,
	_Out_opt_ CAccessMaskArray *pAccessMasks /* = NULL */,
	_Out_opt_ CAceTypeArray *pAceTypes /* = NULL */,
	_Out_opt_ CAceFlagArray *pAceFlags /* = NULL */) const
{
	ATLASSERT(pSids);
	if(pSids)
	{
		pSids->clear();
		if(pAccessMasks)
			pAccessMasks->RemoveAll();
		if(pAceTypes)
			pAceTypes->RemoveAll();
		if(pAceFlags)
			pAceFlags->RemoveAll();

		const CAce *pAce;
		const UINT nCount = GetAceCount();
		for(UINT i = 0; i < nCount; i++)
		{
			pAce = GetAce(i);

			pSids->push_back(pAce->Sid());
			if(pAccessMasks)
				pAccessMasks->Add(pAce->AccessMask());
			if(pAceTypes)
				pAceTypes->Add(pAce->AceType());
			if(pAceFlags)
				pAceFlags->Add(pAce->AceFlags());
		}
	}
}

inline void CAcl::GetAclEntry(
	_In_ UINT nIndex,
	_Inout_opt_ sid* sid,
	_Out_opt_ ACCESS_MASK* pMask /* = NULL */,
	_Out_opt_ BYTE* pType /* = NULL */,
	_Out_opt_ BYTE* pFlags /* = NULL */,
	_Out_opt_ GUID* pObjectType /* = NULL */,
	_Out_opt_ GUID* pInheritedObjectType /* = NULL */) const
{
	const CAce* pAce = GetAce(nIndex);

	if (sid)
		*sid = pAce->Sid();
	if (pMask)
		*pMask = pAce->AccessMask();
	if (pType)
		*pType = pAce->AceType();
	if (pFlags)
		*pFlags = pAce->AceFlags();
	if (pObjectType)
		*pObjectType = pAce->ObjectType();
	if (pInheritedObjectType)
		*pInheritedObjectType = pAce->InheritedObjectType();
}

inline bool CAcl::RemoveAces(_In_ const sid &rSid)
{
	ATLASSERT(rSid.is_valid());
	if (!rSid.is_valid())
		AtlThrow(E_INVALIDARG);

	if(IsNull())
		return false;

	bool bRet = false;
	const CAce *pAce;
	UINT nIndex = 0;

	while(nIndex < GetAceCount())
	{
		pAce = GetAce(nIndex);
		if(rSid == pAce->Sid())
		{
			RemoveAce(nIndex);
			bRet = true;
		}
		else
			nIndex++;
	}

	if(bRet)
		Dirty();

	return bRet;
}

inline const ACL *CAcl::GetPACL() const
{
	if(!m_pAcl && !m_bNull)
	{
		UINT nAclLength = sizeof(ACL);
		const CAce *pAce;
		UINT i;
		const UINT nCount = GetAceCount();

		for(i = 0; i < nCount; i++)
		{
			pAce = GetAce(i);
			ATLASSERT(pAce);
			if(pAce)
				nAclLength += pAce->length();
		}

		m_pAcl = static_cast<ACL *>(malloc(nAclLength));
		if(!m_pAcl)
			AtlThrow(E_OUTOFMEMORY);

		if(!::InitializeAcl(m_pAcl, (DWORD) nAclLength, m_dwAclRevision))
		{
			HRESULT hr = AtlHresultFromLastError();
			free(m_pAcl);
			m_pAcl = NULL;
			AtlThrow(hr);
		}
		else
		{
			PrepareAcesForACL();

			for(i = 0; i < nCount; i++)
			{
				pAce = GetAce(i);
				ATLASSERT(pAce);
				if(!pAce ||
					!::AddAce(m_pAcl, m_dwAclRevision, MAXDWORD, pAce->GetACE(), (DWORD) pAce->length()))
				{
					HRESULT hr = AtlHresultFromLastError();
					free(m_pAcl);
					m_pAcl = NULL;
					AtlThrow(hr);
				}
			}
		}
	}
	return m_pAcl;
}

inline CAcl::operator const ACL *() const
{
	return GetPACL();
}

inline UINT CAcl::length() const
{
	ACL *pAcl = const_cast<ACL *>(GetPACL());
	ACL_SIZE_INFORMATION AclSize;

	ATLENSURE(pAcl);

	if(!::GetAclInformation(pAcl, &AclSize, sizeof(AclSize), AclSizeInformation))
		AtlThrowLastWin32();

	return AclSize.AclBytesInUse;
}

inline void CAcl::SetNull() noexcept
{
	RemoveAllAces();
	m_bNull = true;
}

inline void CAcl::SetEmpty() noexcept
{
	RemoveAllAces();
	m_bNull = false;
}

inline bool CAcl::IsNull() const noexcept
{
	return m_bNull;
}

inline bool CAcl::IsEmpty() const noexcept
{
	return !m_bNull && 0 == GetAceCount();
}

inline void CAcl::Dirty() noexcept
{
	free(m_pAcl);
	m_pAcl = NULL;
}

inline void CAcl::PrepareAcesForACL() const noexcept
{
}

// **************************************************************
// CAcl::CAce implementation

inline CAcl::CAce::CAce(
		_In_ const sid &rSid,
		_In_ ACCESS_MASK accessmask,
		_In_ BYTE aceflags) :
	m_dwAccessMask(accessmask),
	m_sid(rSid),
	m_aceflags(aceflags),
	m_pAce(NULL)
{
}

inline CAcl::CAce::~CAce() noexcept
{
	if(m_pAce)
	{
		free(m_pAce);
		m_pAce = NULL;
	}
}

inline CAcl::CAce::CAce(_In_ const CAce &rhs) :
	m_sid(rhs.m_sid),
	m_dwAccessMask(rhs.m_dwAccessMask),
	m_aceflags(rhs.m_aceflags),
	m_pAce(NULL)
{
}

inline CAcl::CAce &CAcl::CAce::operator=(_In_ const CAce &rhs)
{
	if(this != &rhs)
	{
		m_sid = rhs.m_sid;
		m_dwAccessMask = rhs.m_dwAccessMask;
		m_aceflags = rhs.m_aceflags;
		if(m_pAce)
		{
			free(m_pAce);
		}
		m_pAce = NULL;
	}
	return *this;
}

inline bool CAcl::CAce::IsObjectAce() const noexcept
{
	return false;
}

inline GUID CAcl::CAce::ObjectType() const noexcept
{
	return GUID_NULL;
}

inline GUID CAcl::CAce::InheritedObjectType() const noexcept
{
	return GUID_NULL;
}

inline ACCESS_MASK CAcl::CAce::AccessMask() const noexcept
{
	return m_dwAccessMask;
}

inline BYTE CAcl::CAce::AceFlags() const noexcept
{
	return m_aceflags;
}

inline const sid &CAcl::CAce::Sid() const noexcept
{
	return m_sid;
}

inline void CAcl::CAce::AddAccess(_In_ ACCESS_MASK accessmask) noexcept
{
	m_dwAccessMask |= accessmask;
	if(m_pAce)
	{
		free(m_pAce);
		m_pAce = NULL;
	}
}

// ************************************************
// CDacl implementation

inline CDacl::CDacl() noexcept
{
}

inline CDacl::~CDacl() noexcept
{
	CDacl::RemoveAllAces();
}

inline CDacl::CDacl(_In_ const CDacl &rhs)
{
	Copy(rhs);
}

inline CDacl &CDacl::operator=(_In_ const CDacl &rhs)
{
	if (this != &rhs)
	{
		RemoveAllAces();
		Copy(rhs);
	}

	return *this;
}

inline CDacl::CDacl(_In_ const ACL &rhs)
{
	Copy(rhs);
}

inline CDacl &CDacl::operator=(_In_ const ACL &rhs)
{
	RemoveAllAces();

	Copy(rhs);
	return *this;
}

inline bool CDacl::AddAllowedAce(
	_In_ const sid &rSid,
	_In_ ACCESS_MASK accessmask,
	_In_ BYTE aceflags /* = 0 */)
{
	ATLASSERT(rSid.is_valid());
	if(!rSid.is_valid())
		return false;

	if (IsNull())
		SetEmpty();

	CAutoPtr<CAccessAce> pAce;
	ATLTRY(pAce.Attach(new CAccessAce(rSid, accessmask, aceflags, true)));
	if(!pAce)
		AtlThrow(E_OUTOFMEMORY);

	m_acl.Add(pAce);

	Dirty();
	return true;
}

inline bool CDacl::AddDeniedAce(
	_In_ const sid &rSid,
	_In_ ACCESS_MASK accessmask,
	_In_ BYTE aceflags /* = 0 */)
{
	ATLASSERT(rSid.is_valid());
	if(!rSid.is_valid())
		return false;

	if (IsNull())
		SetEmpty();

	CAutoPtr<CAccessAce> pAce;
	ATLTRY(pAce.Attach(new CAccessAce(rSid, accessmask, aceflags, false)));
	if(!pAce)
		AtlThrow(E_OUTOFMEMORY);

	m_acl.Add(pAce);

	Dirty();
	return true;
}

#if(_WIN32_WINNT >= 0x0500)
inline bool CDacl::AddAllowedAce(
	_In_ const sid &rSid,
	_In_ ACCESS_MASK accessmask,
	_In_ BYTE aceflags,
	_In_ const GUID *pObjectType,
	_In_ const GUID *pInheritedObjectType)
{
	if(!pObjectType && !pInheritedObjectType)
		return AddAllowedAce(rSid, accessmask, aceflags);

	ATLASSERT(rSid.is_valid());
	if(!rSid.is_valid())
		return false;

	if (IsNull())
		SetEmpty();

	CAutoPtr<CAccessAce> pAce;
	ATLTRY(pAce.Attach(new CAccessObjectAce(rSid, accessmask, aceflags, true,
		pObjectType, pInheritedObjectType)));
	if(!pAce)
		AtlThrow(E_OUTOFMEMORY);

	m_acl.Add(pAce);

	m_dwAclRevision = ACL_REVISION_DS;
	Dirty();
	return true;
}
#endif

#if(_WIN32_WINNT >= 0x0500)
inline bool CDacl::AddDeniedAce(
	_In_ const sid &rSid,
	_In_ ACCESS_MASK accessmask,
	_In_ BYTE aceflags,
	_In_ const GUID *pObjectType,
	_In_ const GUID *pInheritedObjectType)
{
	if(!pObjectType && !pInheritedObjectType)
		return AddDeniedAce(rSid, accessmask, aceflags);

	ATLASSERT(rSid.is_valid());
	if(!rSid.is_valid())
		return false;

	if (IsNull())
		SetEmpty();

	CAutoPtr<CAccessAce> pAce;
	ATLTRY(pAce.Attach(new CAccessObjectAce(rSid, accessmask, aceflags, false,
		pObjectType, pInheritedObjectType)));
	if(!pAce)
		AtlThrow(E_OUTOFMEMORY);

	m_acl.Add(pAce);

	m_dwAclRevision = ACL_REVISION_DS;
	Dirty();
	return true;
}
#endif

inline void CDacl::RemoveAllAces() noexcept
{
	m_acl.RemoveAll();
	Dirty();
}

inline void CDacl::RemoveAce(_In_ UINT nIndex)
{
	m_acl.RemoveAt(nIndex);
}

inline UINT CDacl::GetAceCount() const noexcept
{
	return (UINT) m_acl.GetCount();
}

inline void CDacl::Copy(_In_ const CDacl &rhs)
{
	sid sid;
	ACCESS_MASK accessmask;
	BYTE type;
	BYTE flags;
	GUID guidType;
	GUID guidInheritedType;
	CAutoPtr<CAccessAce> pAce;

	Dirty();

	if (rhs.IsNull())
		SetNull();
	else
		SetEmpty();

	m_dwAclRevision = rhs.m_dwAclRevision;

	for(UINT i=0; i<rhs.GetAceCount(); i++)
	{
		rhs.GetAclEntry(i, &sid, &accessmask, &type, &flags, &guidType, &guidInheritedType);
		switch (type)
		{
		case ACCESS_ALLOWED_ACE_TYPE:
		case ACCESS_DENIED_ACE_TYPE:
			ATLTRY(pAce.Attach(new CAccessAce(sid, accessmask, flags, ACCESS_ALLOWED_ACE_TYPE == type)));
			if (!pAce)
				AtlThrow(E_OUTOFMEMORY);
			m_acl.Add(pAce);
			break;

#if(_WIN32_WINNT >= 0x0500)
		case ACCESS_ALLOWED_OBJECT_ACE_TYPE:
		case ACCESS_DENIED_OBJECT_ACE_TYPE:
		{
			GUID *pguidType = NULL;
			GUID *pguidInheritedType = NULL;
			if(guidType != GUID_NULL)
			{
				pguidType = &guidType;
			}

			if(guidInheritedType != GUID_NULL)
			{
				pguidInheritedType = &guidInheritedType;
			}

			ATLTRY(pAce.Attach(new CAccessObjectAce(
					sid,
					accessmask,
					flags,
					ACCESS_ALLOWED_OBJECT_ACE_TYPE == type,
					pguidType,
					pguidInheritedType)));
			if (!pAce)
				AtlThrow(E_OUTOFMEMORY);
			m_acl.Add(pAce);
			break;
		}
#endif

		default:
			// Wrong ACE type
			ATLASSERT(false);
		}
	}
}

inline void CDacl::Copy(_In_ const ACL &rhs)
{
	ACL *pAcl = const_cast<ACL *>(&rhs);
	if (pAcl == NULL)
	{
		SetNull();
		return;
	}
	ACL_SIZE_INFORMATION aclsizeinfo;
	ACL_REVISION_INFORMATION aclrevisioninfo;
	ACE_HEADER *pHeader;
	sid sid;
	ACCESS_MASK accessmask;
	CAutoPtr<CAccessAce> pAce;

	Dirty();

	if(!::GetAclInformation(pAcl, &aclsizeinfo, sizeof(aclsizeinfo), AclSizeInformation))
		AtlThrowLastWin32();

	if(!::GetAclInformation(pAcl, &aclrevisioninfo, sizeof(aclrevisioninfo), AclRevisionInformation))
		AtlThrowLastWin32();
	m_dwAclRevision = aclrevisioninfo.AclRevision;

	for(DWORD i = 0; i < aclsizeinfo.AceCount; i++)
	{
		if(!::GetAce(pAcl, i, reinterpret_cast<void **>(&pHeader)))
			AtlThrowLastWin32();

		accessmask = *reinterpret_cast<ACCESS_MASK *>
			(reinterpret_cast<BYTE *>(pHeader) + sizeof(ACE_HEADER));

		switch(pHeader->AceType)
		{
		case ACCESS_ALLOWED_ACE_TYPE:
		case ACCESS_DENIED_ACE_TYPE:
			sid = *reinterpret_cast<SID *>
				(reinterpret_cast<BYTE *>(pHeader) + sizeof(ACE_HEADER) + sizeof(ACCESS_MASK));

			ATLTRY(pAce.Attach(new CAccessAce(sid, accessmask, pHeader->AceFlags,
				ACCESS_ALLOWED_ACE_TYPE == pHeader->AceType)));
			if (!pAce)
				AtlThrow(E_OUTOFMEMORY);
			m_acl.Add(pAce);
			break;

#if(_WIN32_WINNT >= 0x0500)
		case ACCESS_ALLOWED_OBJECT_ACE_TYPE:
		case ACCESS_DENIED_OBJECT_ACE_TYPE:
		{
			GUID *pObjectType = NULL, *pInheritedObjectType = NULL;
			BYTE *pb = reinterpret_cast<BYTE *>
				(pHeader) + offsetof(ACCESS_ALLOWED_OBJECT_ACE, SidStart);
			DWORD dwFlags = reinterpret_cast<ACCESS_ALLOWED_OBJECT_ACE *>(pHeader)->Flags;

			if(dwFlags & ACE_OBJECT_TYPE_PRESENT)
			{
				pObjectType = reinterpret_cast<GUID *>
					(reinterpret_cast<BYTE *>(pHeader) +
					offsetof(ACCESS_ALLOWED_OBJECT_ACE, ObjectType));
			}
			else
				pb -= sizeof(GUID);

			if(dwFlags & ACE_INHERITED_OBJECT_TYPE_PRESENT)
			{
				pInheritedObjectType = reinterpret_cast<GUID *>
					(reinterpret_cast<BYTE *>(pHeader) +
					(pObjectType ?
					offsetof(ACCESS_ALLOWED_OBJECT_ACE, InheritedObjectType) :
					offsetof(ACCESS_ALLOWED_OBJECT_ACE, ObjectType)));
			}
			else
				pb -= sizeof(GUID);

			sid = *reinterpret_cast<SID *>(pb);

			ATLTRY(pAce.Attach(new CAccessObjectAce(sid, accessmask, pHeader->AceFlags,
				ACCESS_ALLOWED_OBJECT_ACE_TYPE == pHeader->AceType,
				pObjectType, pInheritedObjectType)));
			if (!pAce)
				AtlThrow(E_OUTOFMEMORY);
			m_acl.Add(pAce);
			break;
		}
#endif

		default:
			// Wrong ACE type
			ATLASSERT(false);
		}
	}
}

inline const CDacl::CAce *CDacl::GetAce(_In_ UINT nIndex) const
{
	return m_acl[nIndex];
}

inline void CDacl::PrepareAcesForACL() const noexcept
{
	// For a dacl, sort the aces
	size_t i, j, h = 1;
	const size_t nCount = m_acl.GetCount();
	CAutoPtr<CAccessAce> spAce;

	while(h * 3 + 1 < nCount)
		h = 3 * h + 1;

	while(h > 0)
	{
		for(i = h - 1; i < nCount; i++)
		{
			spAce = m_acl[i];

			for(j = i; j >= h && CAccessAce::Order(*m_acl[j - h], *spAce) == -1; j -= h)
				m_acl[j] = m_acl[j - h];

			m_acl[j] = spAce;
		}

		h /= 3;
	}
}

// ************************************************
// CDacl::CAccessAce implementation

inline CDacl::CAccessAce::CAccessAce(
		_In_ const sid &rSid,
		_In_ ACCESS_MASK accessmask,
		_In_ BYTE aceflags,
		_In_ bool bAllowAccess) :
	CAce(rSid, accessmask, aceflags),
	m_bAllow(bAllowAccess)
{
}

inline CDacl::CAccessAce::~CAccessAce() noexcept
{
}

inline void *CDacl::CAccessAce::GetACE() const
{
	C_ASSERT(sizeof(ACCESS_ALLOWED_ACE) == sizeof(ACCESS_DENIED_ACE));
	C_ASSERT(offsetof(ACCESS_ALLOWED_ACE, Header)==offsetof(ACCESS_DENIED_ACE, Header));
	C_ASSERT(offsetof(ACCESS_ALLOWED_ACE, Mask)==offsetof(ACCESS_DENIED_ACE, Mask));
	C_ASSERT(offsetof(ACCESS_ALLOWED_ACE, SidStart)==offsetof(ACCESS_DENIED_ACE, SidStart));

	if(!m_pAce)
	{
		UINT nLength = length();
		ACCESS_ALLOWED_ACE *pAce = static_cast<ACCESS_ALLOWED_ACE *>(malloc(nLength));
		if(!pAce)
			AtlThrow(E_OUTOFMEMORY);

		memset(pAce, 0x00, nLength);

		pAce->Header.AceSize = static_cast<WORD>(nLength);
		pAce->Header.AceFlags = m_aceflags;
		pAce->Header.AceType = AceType();

		pAce->Mask = m_dwAccessMask;
		ATLASSERT(nLength-offsetof(ACCESS_ALLOWED_ACE, SidStart) >= m_sid.length());
		Checked::memcpy_s(&pAce->SidStart, nLength-offsetof(ACCESS_ALLOWED_ACE, SidStart), m_sid.get_sid(), m_sid.length());

		m_pAce = pAce;
	}
	return m_pAce;
}

inline UINT CDacl::CAccessAce::length() const noexcept
{
	return offsetof(ACCESS_ALLOWED_ACE, SidStart) + m_sid.length();
}

inline BYTE CDacl::CAccessAce::AceType() const noexcept
{
	return (BYTE)(m_bAllow ? ACCESS_ALLOWED_ACE_TYPE : ACCESS_DENIED_ACE_TYPE);
}

inline bool CDacl::CAccessAce::Allow() const noexcept
{
	return m_bAllow;
}

inline bool CDacl::CAccessAce::Inherited() const noexcept
{
	return 0 != (m_aceflags & INHERITED_ACE);
}

inline int CDacl::CAccessAce::Order(
	_In_ const CDacl::CAccessAce &lhs,
	_In_ const CDacl::CAccessAce &rhs) noexcept
{
	// The order is:
	// denied direct aces
	// denied direct object aces
	// allowed direct aces
	// allowed direct object aces
	// denied inherit aces
	// denied inherit object aces
	// allowed inherit aces
	// allowed inherit object aces

	// inherited aces are always "greater" than non-inherited aces
	if(lhs.Inherited() && !rhs.Inherited())
		return -1;
	if(!lhs.Inherited() && rhs.Inherited())
		return 1;

	// if the aces are *both* either inherited or non-inherited, continue...

	// allowed aces are always "greater" than denied aces (subject to above)
	if(lhs.Allow() && !rhs.Allow())
		return -1;
	if(!lhs.Allow() && rhs.Allow())
		return 1;

	// if the aces are *both* either allowed or denied, continue...

	// object aces are always "greater" than non-object aces (subject to above)
	if(lhs.IsObjectAce() && !rhs.IsObjectAce())
		return -1;
	if(!lhs.IsObjectAce() && rhs.IsObjectAce())
		return 1;

	// aces are "equal" (e.g., both are access denied inherited object aces)
	return 0;
}

#if(_WIN32_WINNT >= 0x0500)
// ************************************************
// CDacl::CAccessObjectAce implementation

inline CDacl::CAccessObjectAce::CAccessObjectAce(
		_In_ const sid &rSid,
		_In_ ACCESS_MASK accessmask,
		_In_ BYTE aceflags,
		_In_ bool bAllowAccess,
		_In_opt_ const GUID *pObjectType,
		_In_opt_ const GUID *pInheritedObjectType) :
	CAccessAce(rSid, accessmask, aceflags, bAllowAccess),
	m_pObjectType(NULL),
	m_pInheritedObjectType(NULL)
{
	if(pObjectType)
	{
		m_pObjectType = _ATL_NEW GUID(*pObjectType);
		if(!m_pObjectType)
			AtlThrow(E_OUTOFMEMORY);
	}

	if(pInheritedObjectType)
	{
		m_pInheritedObjectType = _ATL_NEW GUID(*pInheritedObjectType);
		if(!m_pInheritedObjectType)
		{
			delete m_pObjectType;
			m_pObjectType = NULL;
			AtlThrow(E_OUTOFMEMORY);
		}
	}
}

inline CDacl::CAccessObjectAce::~CAccessObjectAce() noexcept
{
	delete m_pObjectType;
	delete m_pInheritedObjectType;
}

inline CDacl::CAccessObjectAce::CAccessObjectAce(_In_ const CAccessObjectAce &rhs) :
	CAccessAce(rhs),
	m_pObjectType(NULL),
	m_pInheritedObjectType(NULL)
{
	*this = rhs;
}

inline CDacl::CAccessObjectAce &CDacl::CAccessObjectAce::operator=(
	_In_ const CAccessObjectAce &rhs)
{
	if(this != &rhs)
	{
		CAccessAce::operator=(rhs);

		if(rhs.m_pObjectType)
		{
			if(!m_pObjectType)
			{
				m_pObjectType = _ATL_NEW GUID;
				if(!m_pObjectType)
					AtlThrow(E_OUTOFMEMORY);
			}
			*m_pObjectType = *rhs.m_pObjectType;
		}
		else
		{
			delete m_pObjectType;
			m_pObjectType = NULL;
		}

		if(rhs.m_pInheritedObjectType)
		{
			if(!m_pInheritedObjectType)
			{
				m_pInheritedObjectType = _ATL_NEW GUID;
				if(!m_pInheritedObjectType)
				{
					delete m_pObjectType;
					m_pObjectType = NULL;
					AtlThrow(E_OUTOFMEMORY);
				}
			}
			*m_pInheritedObjectType = *rhs.m_pInheritedObjectType;
		}
		else
		{
			delete m_pInheritedObjectType;
			m_pInheritedObjectType = NULL;
		}
	}
	return *this;
}

inline void *CDacl::CAccessObjectAce::GetACE() const
{
	C_ASSERT(sizeof(ACCESS_ALLOWED_OBJECT_ACE) == sizeof(ACCESS_DENIED_OBJECT_ACE));
	C_ASSERT(offsetof(ACCESS_ALLOWED_OBJECT_ACE, Header)==offsetof(ACCESS_DENIED_OBJECT_ACE, Header));
	C_ASSERT(offsetof(ACCESS_ALLOWED_OBJECT_ACE, Mask)==offsetof(ACCESS_DENIED_OBJECT_ACE, Mask));
	C_ASSERT(offsetof(ACCESS_ALLOWED_OBJECT_ACE, Flags)==offsetof(ACCESS_DENIED_OBJECT_ACE, Flags));
	C_ASSERT(offsetof(ACCESS_ALLOWED_OBJECT_ACE, ObjectType)==offsetof(ACCESS_DENIED_OBJECT_ACE, ObjectType));
	C_ASSERT(offsetof(ACCESS_ALLOWED_OBJECT_ACE, InheritedObjectType)==offsetof(ACCESS_DENIED_OBJECT_ACE, InheritedObjectType));
	C_ASSERT(offsetof(ACCESS_ALLOWED_OBJECT_ACE, SidStart)==offsetof(ACCESS_DENIED_OBJECT_ACE, SidStart));

	if(!m_pAce)
	{
		UINT nLength = length();

		ACCESS_ALLOWED_OBJECT_ACE *pAce = static_cast<ACCESS_ALLOWED_OBJECT_ACE *>(malloc(nLength));
		if(!pAce)
			AtlThrow(E_OUTOFMEMORY);

		memset(pAce, 0x00, nLength);

		pAce->Header.AceSize = static_cast<WORD>(nLength);
		pAce->Header.AceFlags = m_aceflags;
		pAce->Header.AceType = AceType();

		pAce->Mask = m_dwAccessMask;
		pAce->Flags = 0;

		BYTE *pb = (reinterpret_cast<BYTE *>(pAce)) + offsetof(ACCESS_ALLOWED_OBJECT_ACE, SidStart);
		if(!m_pObjectType)
			pb -= sizeof(GUID);
		else
		{
			pAce->ObjectType = *m_pObjectType;
			pAce->Flags |= ACE_OBJECT_TYPE_PRESENT;
		}

		if(!m_pInheritedObjectType)
			pb -= sizeof(GUID);
		else
		{
			if(m_pObjectType)
				pAce->InheritedObjectType = *m_pInheritedObjectType;
			else
				pAce->ObjectType = *m_pInheritedObjectType;
			pAce->Flags |= ACE_INHERITED_OBJECT_TYPE_PRESENT;
		}
		
		size_t sidSpaceAvailable = nLength - (pb - reinterpret_cast<BYTE*>(pAce));
		if (sidSpaceAvailable > nLength)
		{
			sidSpaceAvailable = 0;
		}
		
		ATLASSERT(sidSpaceAvailable >= m_sid.length());
		Checked::memcpy_s(pb, sidSpaceAvailable, m_sid.get_sid(), m_sid.length());
		m_pAce = pAce;
	}
	return m_pAce;
}

inline UINT CDacl::CAccessObjectAce::length() const noexcept
{
	UINT nLength = offsetof(ACCESS_ALLOWED_OBJECT_ACE, SidStart);

	if(!m_pObjectType)
		nLength -= sizeof(GUID);
	if(!m_pInheritedObjectType)
		nLength -= sizeof(GUID);

	nLength += m_sid.length();

	return nLength;
}

inline BYTE CDacl::CAccessObjectAce::AceType() const noexcept
{
	return (BYTE)(m_bAllow ? ACCESS_ALLOWED_OBJECT_ACE_TYPE : ACCESS_DENIED_OBJECT_ACE_TYPE);
}

inline bool CDacl::CAccessObjectAce::IsObjectAce() const noexcept
{
	return true;
}

inline GUID CDacl::CAccessObjectAce::ObjectType() const noexcept
{
	return m_pObjectType ? *m_pObjectType : GUID_NULL;
}

inline GUID CDacl::CAccessObjectAce::InheritedObjectType() const noexcept
{
	return m_pInheritedObjectType ? *m_pInheritedObjectType : GUID_NULL;
}
#endif // _WIN32_WINNT

//******************************************
// CSacl implementation

inline CSacl::CSacl() noexcept
{
}

inline CSacl::~CSacl() noexcept
{
	CSacl::RemoveAllAces();
}

inline CSacl::CSacl(_In_ const CSacl &rhs)
{
	Copy(rhs);
}

inline CSacl &CSacl::operator=(_In_ const CSacl &rhs)
{
	if (this != &rhs)
	{
		RemoveAllAces();
		Copy(rhs);
	}

	return *this;
}

inline CSacl::CSacl(_In_ const ACL &rhs)
{
	Copy(rhs);
}

inline CSacl &CSacl::operator=(_In_ const ACL &rhs)
{
	RemoveAllAces();

	Copy(rhs);
	return *this;
}

inline bool CSacl::AddAuditAce(
	_In_ const sid &rSid,
	_In_ ACCESS_MASK accessmask,
	_In_ bool bSuccess,
	_In_ bool bFailure,
	_In_ BYTE aceflags /* = 0 */)
{
	ATLASSERT(rSid.is_valid());
	if(!rSid.is_valid())
		return false;

	if (IsNull())
		SetEmpty();

	CAutoPtr<CAuditAce> pAce;
	ATLTRY(pAce.Attach(new CAuditAce(rSid, accessmask, aceflags, bSuccess, bFailure)))
	if(!pAce)
		AtlThrow(E_OUTOFMEMORY);

	m_acl.Add(pAce);

	Dirty();
	return true;
}

#if(_WIN32_WINNT >= 0x0500)
inline bool CSacl::AddAuditAce(
	_In_ const sid &rSid,
	_In_ ACCESS_MASK accessmask,
	_In_ bool bSuccess,
	_In_ bool bFailure,
	_In_ BYTE aceflags,
	_In_ const GUID *pObjectType,
	_In_ const GUID *pInheritedObjectType)
{
	if(!pObjectType && !pInheritedObjectType)
		return AddAuditAce(rSid, accessmask, bSuccess, bFailure, aceflags);

	ATLASSERT(rSid.is_valid());
	if(!rSid.is_valid())
		return false;

	if (IsNull())
		SetEmpty();

	CAutoPtr<CAuditAce> pAce;
	ATLTRY(pAce.Attach(new CAuditObjectAce(rSid, accessmask, aceflags, bSuccess,
		bFailure, pObjectType, pInheritedObjectType)));
	if(!pAce)
		AtlThrow(E_OUTOFMEMORY);

	m_acl.Add(pAce);
	m_dwAclRevision = ACL_REVISION_DS;
	Dirty();
	return true;
}
#endif

inline void CSacl::RemoveAllAces() noexcept
{
	m_acl.RemoveAll();
	Dirty();
}

inline void CSacl::RemoveAce(_In_ UINT nIndex)
{
	m_acl.RemoveAt(nIndex);
}

inline UINT CSacl::GetAceCount() const noexcept
{
	return (UINT) m_acl.GetCount();
}

inline void CSacl::Copy(_In_ const CSacl &rhs)
{
	sid sid;
	ACCESS_MASK accessmask;
	BYTE type;
	BYTE flags;
	GUID guidType;
	GUID guidInheritedType;
	bool bSuccess;
	bool bFailure;
	CAutoPtr<CAuditAce> pAce;

	Dirty();

	if (rhs.IsNull())
		SetNull();
	else
		SetEmpty();

	m_dwAclRevision = rhs.m_dwAclRevision;

	for (UINT i=0; i<rhs.GetAceCount(); i++)
	{
		rhs.GetAclEntry(i, &sid, &accessmask, &type, &flags, &guidType, &guidInheritedType);

		bSuccess = 0 != (flags & SUCCESSFUL_ACCESS_ACE_FLAG);
		bFailure = 0 != (flags & FAILED_ACCESS_ACE_FLAG);

		switch (type)
		{
		case SYSTEM_AUDIT_ACE_TYPE:
			ATLTRY(pAce.Attach(new CAuditAce(sid, accessmask, flags, bSuccess, bFailure)));
			if (!pAce)
				AtlThrow(E_OUTOFMEMORY);
			m_acl.Add(pAce);
			break;

#if(_WIN32_WINNT >= 0x0500)
		case SYSTEM_AUDIT_OBJECT_ACE_TYPE:
		{
			GUID *pguidType = NULL;
			GUID *pguidInheritedType = NULL;
			if(guidType != GUID_NULL)
			{
				pguidType = &guidType;
			}

			if(guidInheritedType != GUID_NULL)
			{
				pguidInheritedType = &guidInheritedType;
			}

			ATLTRY(pAce.Attach(new CAuditObjectAce(
					sid,
					accessmask,
					flags,
					bSuccess,
					bFailure,
					pguidType,
					pguidInheritedType)));
			if(!pAce)
				AtlThrow(E_OUTOFMEMORY);
			m_acl.Add(pAce);
			break;
		}
#endif
		default:
			// Wrong ACE type
			ATLASSERT(false);
		}
	}
}

inline void CSacl::Copy(_In_ const ACL &rhs)
{
	ACL *pAcl = const_cast<ACL *>(&rhs);
	ACL_SIZE_INFORMATION aclsizeinfo;
	ACL_REVISION_INFORMATION aclrevisioninfo;
	ACE_HEADER *pHeader;
	sid sid;
	ACCESS_MASK accessmask;
	bool bSuccess, bFailure;
	CAutoPtr<CAuditAce> pAce;

	Dirty();

	if(!::GetAclInformation(pAcl, &aclsizeinfo, sizeof(aclsizeinfo), AclSizeInformation))
		AtlThrowLastWin32();

	if(!::GetAclInformation(pAcl, &aclrevisioninfo, sizeof(aclrevisioninfo), AclRevisionInformation))
		AtlThrowLastWin32();
	m_dwAclRevision = aclrevisioninfo.AclRevision;

	for(DWORD i = 0; i < aclsizeinfo.AceCount; i++)
	{
		if(!::GetAce(pAcl, i, reinterpret_cast<void **>(&pHeader)))
			AtlThrowLastWin32();

		accessmask = *reinterpret_cast<ACCESS_MASK *>
			(reinterpret_cast<BYTE *>(pHeader) + sizeof(ACE_HEADER));

		bSuccess = 0 != (pHeader->AceFlags & SUCCESSFUL_ACCESS_ACE_FLAG);
		bFailure = 0 != (pHeader->AceFlags & FAILED_ACCESS_ACE_FLAG);

		switch(pHeader->AceType)
		{
		case SYSTEM_AUDIT_ACE_TYPE:
			sid = *reinterpret_cast<SID *>
				(reinterpret_cast<BYTE *>(pHeader) +	sizeof(ACE_HEADER) + sizeof(ACCESS_MASK));
			ATLTRY(pAce.Attach(new CAuditAce(sid, accessmask, pHeader->AceFlags, bSuccess, bFailure)));
			if (!pAce)
				AtlThrow(E_OUTOFMEMORY);
			m_acl.Add(pAce);
			break;

#if(_WIN32_WINNT >= 0x0500)
		case SYSTEM_AUDIT_OBJECT_ACE_TYPE:
		{
			GUID *pObjectType = NULL, *pInheritedObjectType = NULL;
			BYTE *pb = reinterpret_cast<BYTE *>
				(pHeader) + offsetof(SYSTEM_AUDIT_OBJECT_ACE, SidStart);
			DWORD dwFlags = reinterpret_cast<SYSTEM_AUDIT_OBJECT_ACE *>(pHeader)->Flags;

			if(dwFlags & ACE_OBJECT_TYPE_PRESENT)
			{
				pObjectType = reinterpret_cast<GUID *>
					(reinterpret_cast<BYTE *>(pHeader) +
					offsetof(SYSTEM_AUDIT_OBJECT_ACE, ObjectType));
			}
			else
				pb -= sizeof(GUID);

			if(dwFlags & ACE_INHERITED_OBJECT_TYPE_PRESENT)
			{
				pInheritedObjectType = reinterpret_cast<GUID *>
					(reinterpret_cast<BYTE *>(pHeader) +
					(pObjectType ?
					offsetof(SYSTEM_AUDIT_OBJECT_ACE, InheritedObjectType) :
					offsetof(SYSTEM_AUDIT_OBJECT_ACE, ObjectType)));
			}
			else
				pb -= sizeof(GUID);

			sid = *reinterpret_cast<SID *>(pb);

			ATLTRY(pAce.Attach(new CAuditObjectAce(sid, accessmask, pHeader->AceFlags,
				bSuccess, bFailure, pObjectType, pInheritedObjectType)));
			if(!pAce)
				AtlThrow(E_OUTOFMEMORY);
			m_acl.Add(pAce);
			break;
		}
#endif
		default:
			// Wrong ACE type
			ATLASSERT(false);
		}
	}
}

inline const CSacl::CAce *CSacl::GetAce(_In_ UINT nIndex) const
{
	return m_acl[nIndex];
}

//******************************************
// CSacl::CAuditAce implementation

inline CSacl::CAuditAce::CAuditAce(
		_In_ const sid &rSid,
		_In_ ACCESS_MASK accessmask,
		_In_ BYTE aceflags,
		_In_ bool bAuditSuccess,
		_In_ bool bAuditFailure) :
	CAce(rSid, accessmask, aceflags),
	m_bSuccess(bAuditSuccess),
	m_bFailure(bAuditFailure)
{
	if (m_bSuccess)
		m_aceflags |= SUCCESSFUL_ACCESS_ACE_FLAG;
	if (m_bFailure)
		m_aceflags |= FAILED_ACCESS_ACE_FLAG;
}

inline CSacl::CAuditAce::~CAuditAce() noexcept
{
}

inline void *CSacl::CAuditAce::GetACE() const
{
	if(!m_pAce)
	{
		UINT nLength = length();
		SYSTEM_AUDIT_ACE *pAce = static_cast<SYSTEM_AUDIT_ACE *>(malloc(nLength));
		if(!pAce)
			AtlThrow(E_OUTOFMEMORY);

		memset(pAce, 0x00, nLength);

		pAce->Header.AceSize = static_cast<WORD>(nLength);
		pAce->Header.AceFlags = m_aceflags;
		pAce->Header.AceType = AceType();

		pAce->Mask = m_dwAccessMask;
		ATLASSERT(nLength-offsetof(SYSTEM_AUDIT_ACE, SidStart) >= m_sid.length());
		Checked::memcpy_s(&pAce->SidStart, nLength-offsetof(SYSTEM_AUDIT_ACE, SidStart), m_sid.get_sid(), m_sid.length());

		m_pAce = pAce;
	}
	return m_pAce;
}

inline UINT CSacl::CAuditAce::length() const noexcept
{
	return offsetof(SYSTEM_AUDIT_ACE, SidStart) + m_sid.length();
}

inline BYTE CSacl::CAuditAce::AceType() const noexcept
{
	return SYSTEM_AUDIT_ACE_TYPE;
}

#if(_WIN32_WINNT >= 0x0500)
//******************************************
// CSacl::CAuditObjectAce implementation

inline CSacl::CAuditObjectAce::CAuditObjectAce(
		_In_ const sid &rSid,
		_In_ ACCESS_MASK accessmask,
		_In_ BYTE aceflags,
		_In_ bool bAuditSuccess,
		_In_ bool bAuditFailure,
		_In_opt_ const GUID *pObjectType,
		_In_opt_ const GUID *pInheritedObjectType) :
	CAuditAce(rSid, accessmask, aceflags, bAuditSuccess, bAuditFailure),
	m_pObjectType(NULL),
	m_pInheritedObjectType(NULL)
{
	if(pObjectType)
	{
		m_pObjectType = _ATL_NEW GUID(*pObjectType);
		if(!m_pObjectType)
			AtlThrow(E_OUTOFMEMORY);
	}

	if(pInheritedObjectType)
	{
		m_pInheritedObjectType = _ATL_NEW GUID(*pInheritedObjectType);
		if(!m_pInheritedObjectType)
		{
			delete m_pObjectType;
			m_pObjectType = NULL;
			AtlThrow(E_OUTOFMEMORY);
		}
	}
}

inline CSacl::CAuditObjectAce::~CAuditObjectAce() noexcept
{
	delete m_pObjectType;
	delete m_pInheritedObjectType;
}

inline CSacl::CAuditObjectAce::CAuditObjectAce(_In_ const CAuditObjectAce &rhs) :
	CAuditAce(rhs),
	m_pObjectType(NULL),
	m_pInheritedObjectType(NULL)
{
	*this = rhs;
}

inline CSacl::CAuditObjectAce &CSacl::CAuditObjectAce::operator=(
	_In_ const CAuditObjectAce &rhs)
{
	if(this != &rhs)
	{
		CAuditAce::operator=(rhs);

		if(rhs.m_pObjectType)
		{
			if(!m_pObjectType)
			{
				m_pObjectType = _ATL_NEW GUID;
				if(!m_pObjectType)
					AtlThrow(E_OUTOFMEMORY);
			}
			*m_pObjectType = *rhs.m_pObjectType;
		}
		else
		{
			delete m_pObjectType;
			m_pObjectType = NULL;
		}

		if(rhs.m_pInheritedObjectType)
		{
			if(!m_pInheritedObjectType)
			{
				m_pInheritedObjectType = _ATL_NEW GUID;
				if(!m_pInheritedObjectType)
				{
					delete m_pObjectType;
					m_pObjectType = NULL;
					AtlThrow(E_OUTOFMEMORY);
				}
			}
			*m_pInheritedObjectType = *rhs.m_pInheritedObjectType;
		}
		else
		{
			delete m_pInheritedObjectType;
			m_pInheritedObjectType = NULL;
		}
	}
	return *this;
}

inline void *CSacl::CAuditObjectAce::GetACE() const
{
	if(!m_pAce)
	{
		UINT nLength = length();
		SYSTEM_AUDIT_OBJECT_ACE *pAce = static_cast<SYSTEM_AUDIT_OBJECT_ACE *>(malloc(nLength));
		if(!pAce)
			AtlThrow(E_OUTOFMEMORY);

		memset(pAce, 0x00, nLength);

		pAce->Header.AceType = SYSTEM_AUDIT_OBJECT_ACE_TYPE;
		pAce->Header.AceSize = static_cast<WORD>(nLength);
		pAce->Header.AceFlags = m_aceflags;

		pAce->Mask = m_dwAccessMask;
		pAce->Flags = 0;

		if(m_bSuccess)
			pAce->Header.AceFlags |= SUCCESSFUL_ACCESS_ACE_FLAG;
		else
			pAce->Header.AceFlags &= ~SUCCESSFUL_ACCESS_ACE_FLAG;

		if(m_bFailure)
			pAce->Header.AceFlags |= FAILED_ACCESS_ACE_FLAG;
		else
			pAce->Header.AceFlags &= ~FAILED_ACCESS_ACE_FLAG;

		BYTE *pb = ((BYTE *) pAce) + offsetof(SYSTEM_AUDIT_OBJECT_ACE, SidStart);
		if(!m_pObjectType)
			pb -= sizeof(GUID);
		else
		{
			pAce->ObjectType = *m_pObjectType;
			pAce->Flags |= ACE_OBJECT_TYPE_PRESENT;
		}

		if(!m_pInheritedObjectType)
			pb -= sizeof(GUID);
		else
		{
			if(m_pObjectType)
				pAce->InheritedObjectType = *m_pInheritedObjectType;
			else
				pAce->ObjectType = *m_pInheritedObjectType;
			pAce->Flags |= ACE_INHERITED_OBJECT_TYPE_PRESENT;
		}

		size_t sidSpaceAvailable = nLength - (pb - reinterpret_cast<BYTE*>(pAce));
		if (sidSpaceAvailable > nLength)
		{
			sidSpaceAvailable = 0;
		}

		ATLASSERT(sidSpaceAvailable >= m_sid.length());
		Checked::memcpy_s(pb, sidSpaceAvailable, m_sid.get_sid(), m_sid.length());
		m_pAce = pAce;
	}
	return m_pAce;
}

inline UINT CSacl::CAuditObjectAce::length() const noexcept
{
	UINT nLength = offsetof(SYSTEM_AUDIT_OBJECT_ACE, SidStart);

	if(!m_pObjectType)
		nLength -= sizeof(GUID);
	if(!m_pInheritedObjectType)
		nLength -= sizeof(GUID);

	nLength += m_sid.length();

	return nLength;
}

inline BYTE CSacl::CAuditObjectAce::AceType() const noexcept
{
	return SYSTEM_AUDIT_OBJECT_ACE_TYPE;
}

inline bool CSacl::CAuditObjectAce::IsObjectAce() const noexcept
{
	return true;
}

inline GUID CSacl::CAuditObjectAce::ObjectType() const noexcept
{
	return m_pObjectType ? *m_pObjectType : GUID_NULL;
}

inline GUID CSacl::CAuditObjectAce::InheritedObjectType() const noexcept
{
	return m_pInheritedObjectType ? *m_pInheritedObjectType : GUID_NULL;
}
#endif

//******************************************
// CSecurityDesc implementation

inline CSecurityDesc::CSecurityDesc() noexcept :
	m_pSecurityDescriptor(NULL)
{
}

inline CSecurityDesc::~CSecurityDesc() noexcept
{
	Clear();
}

inline CSecurityDesc::CSecurityDesc(
		_In_ const CSecurityDesc &rhs)
	: m_pSecurityDescriptor(NULL)
{
	if(rhs.m_pSecurityDescriptor)
		Init(*rhs.m_pSecurityDescriptor);
}

inline CSecurityDesc &CSecurityDesc::operator=(
	_In_ const CSecurityDesc &rhs)
{
	if(this != &rhs)
	{
		Clear();
		if(rhs.m_pSecurityDescriptor)
			Init(*rhs.m_pSecurityDescriptor);
	}
	return *this;
}

inline CSecurityDesc::CSecurityDesc(
		_In_ const SECURITY_DESCRIPTOR &rhs) :
	m_pSecurityDescriptor(NULL)
{
	Init(rhs);
}

inline CSecurityDesc &CSecurityDesc::operator=(
	_In_ const SECURITY_DESCRIPTOR &rhs)
{
	if(m_pSecurityDescriptor != &rhs)
	{
		Clear();
		Init(rhs);
	}
	return *this;
}

#if(_WIN32_WINNT >= 0x0500)
inline bool CSecurityDesc::FromString(_In_z_ LPCTSTR pstr)
{
	SECURITY_DESCRIPTOR *pSD;
	if(!::ConvertStringSecurityDescriptorToSecurityDescriptor(pstr, SDDL_REVISION_1,
			(PSECURITY_DESCRIPTOR *) &pSD, NULL))
		AtlThrowLastWin32();

	*this = *pSD;
	::LocalFree(pSD);

	return true;
}

inline bool CSecurityDesc::ToString(
	_In_ CString *pstr,
	_In_ SECURITY_INFORMATION si /* =
		OWNER_SECURITY_INFORMATION |
		GROUP_SECURITY_INFORMATION |
		DACL_SECURITY_INFORMATION |
		SACL_SECURITY_INFORMATION */) const
{
	ATLASSERT(pstr);
	if(!pstr || !m_pSecurityDescriptor)
		return false;

	LPTSTR pszStringSecurityDescriptor;
	if(!::ConvertSecurityDescriptorToStringSecurityDescriptor(m_pSecurityDescriptor,
			SDDL_REVISION_1,
			si,
			&pszStringSecurityDescriptor,
			NULL))
		AtlThrowLastWin32();

	_ATLTRY
	{
		*pstr = pszStringSecurityDescriptor;
	}
	_ATLCATCHALL()
	{
		::LocalFree(pszStringSecurityDescriptor);
		throw;
	}

	::LocalFree(pszStringSecurityDescriptor);
	return true;
}
#endif

ATLPREFAST_SUPPRESS(6014)
inline void CSecurityDesc::SetOwner(
	_In_ const sid &sid,
	_In_ bool bDefaulted /* = false */)
{
	if(m_pSecurityDescriptor)
		MakeAbsolute();

	PSID pNewOwner, pOldOwner;
	if(m_pSecurityDescriptor)
	{
		BOOL _bDefaulted;
		if(!::GetSecurityDescriptorOwner(m_pSecurityDescriptor, &pOldOwner, &_bDefaulted))
			AtlThrowLastWin32();
	}
	else
	{
		AllocateAndInitializeSecurityDescriptor();
		pOldOwner = NULL;
	}

	if(!sid.is_valid())
		AtlThrow(E_FAIL);

	UINT nSidLength = sid.length();
	pNewOwner = malloc(nSidLength);
	if(!pNewOwner)
		AtlThrow(E_OUTOFMEMORY);

	_Analysis_assume_(m_pSecurityDescriptor != NULL); // Guaranteed by "AllocateAndInitializeSecurityDescriptor"
	if(!::CopySid((DWORD) nSidLength, pNewOwner, const_cast<SID *>(sid.get_sid())) ||
		!::SetSecurityDescriptorOwner(m_pSecurityDescriptor, pNewOwner, bDefaulted))
	{
		HRESULT hr = AtlHresultFromLastError();
		free(pNewOwner);
		AtlThrow(hr);
	}

	free(pOldOwner);
}
ATLPREFAST_UNSUPPRESS()

ATLPREFAST_SUPPRESS(6014)
inline void CSecurityDesc::SetGroup(
	_In_ const sid &sid,
	_In_ bool bDefaulted /* = false */)
{
	if(m_pSecurityDescriptor)
		MakeAbsolute();

	PSID pNewGroup, pOldGroup;
	if(m_pSecurityDescriptor)
	{
		BOOL _bDefaulted;
		if(!::GetSecurityDescriptorGroup(m_pSecurityDescriptor, &pOldGroup, &_bDefaulted))
			AtlThrowLastWin32();
	}
	else
	{
		AllocateAndInitializeSecurityDescriptor();
		pOldGroup = NULL;
	}

	if(!sid.is_valid())
		AtlThrow(E_FAIL);

	UINT nSidLength = sid.length();
	pNewGroup = malloc(nSidLength);
	if(!pNewGroup)
		AtlThrow(E_OUTOFMEMORY);

	_Analysis_assume_(m_pSecurityDescriptor != NULL); // Guaranteed by "AllocateAndInitializeSecurityDescriptor"
	if(!::CopySid((DWORD) nSidLength, pNewGroup, const_cast<SID *>(sid.get_sid())) ||
		!::SetSecurityDescriptorGroup(m_pSecurityDescriptor, pNewGroup, bDefaulted))
	{
		HRESULT hr = AtlHresultFromLastError();
		free(pNewGroup);
		AtlThrow(hr);
	}

	free(pOldGroup);
}
ATLPREFAST_UNSUPPRESS()

ATLPREFAST_SUPPRESS(6014)
inline void CSecurityDesc::SetDacl(
	_In_ const CDacl &Dacl,
	_In_ bool bDefaulted /* = false */)
{
	if(m_pSecurityDescriptor)
		MakeAbsolute();

	PACL pNewDacl, pOldDacl = NULL;
	if(m_pSecurityDescriptor)
	{
		BOOL _bDefaulted, _bPresent;
		if(!::GetSecurityDescriptorDacl(m_pSecurityDescriptor, &_bPresent, &pOldDacl, &_bDefaulted))
			AtlThrowLastWin32();
	}
	else
		AllocateAndInitializeSecurityDescriptor();

	if(Dacl.IsNull() || Dacl.IsEmpty())
		pNewDacl = NULL;
	else
	{
		UINT nAclLength = Dacl.length();
		ATLASSUME(nAclLength > 0);

		pNewDacl = static_cast<ACL *>(malloc(nAclLength));
		if(!pNewDacl)
			AtlThrow(E_OUTOFMEMORY);

		Checked::memcpy_s(pNewDacl, nAclLength, Dacl.GetPACL(), nAclLength);
	}

#ifdef _DEBUG
	if(Dacl.IsNull())
	{
		// setting a NULL DACL is almost always the wrong thing to do
		ATLTRACE(atlTraceSecurity, 2, _T("Error: Setting Dacl to Null offers no security\n"));
		ATLASSERT(FALSE);
	}
#endif

	_Analysis_assume_(m_pSecurityDescriptor != NULL); // Guaranteed by "AllocateAndInitializeSecurityDescriptor"
	if(!::SetSecurityDescriptorDacl(m_pSecurityDescriptor, Dacl.IsNull() || pNewDacl, pNewDacl, bDefaulted))
	{
		HRESULT hr = AtlHresultFromLastError();
		free(pNewDacl);
		AtlThrow(hr);
	}

	free(pOldDacl);
}
ATLPREFAST_UNSUPPRESS()

inline void CSecurityDesc::SetDacl(
	_In_ bool bPresent,
	_In_ bool bDefaulted /* = false */)
{
	if(m_pSecurityDescriptor)
		MakeAbsolute();

	PACL pOldDacl = NULL;
	if(m_pSecurityDescriptor)
	{
		BOOL _bDefaulted, _bPresent;
		if(!::GetSecurityDescriptorDacl(m_pSecurityDescriptor, &_bPresent, &pOldDacl, &_bDefaulted))
			AtlThrowLastWin32();
	}
	else
		AllocateAndInitializeSecurityDescriptor();

#ifdef _DEBUG
	if(bPresent)
	{
		// setting a NULL DACL is almost always the wrong thing to do
		ATLTRACE(atlTraceSecurity, 2, _T("Error: Setting Dacl to Null offers no security\n"));
		ATLASSERT(FALSE);
	}
#endif

	_Analysis_assume_(m_pSecurityDescriptor != NULL); // Guaranteed by "AllocateAndInitializeSecurityDescriptor"
	if(!::SetSecurityDescriptorDacl(m_pSecurityDescriptor, bPresent, NULL, bDefaulted))
		AtlThrowLastWin32();

	free(pOldDacl);
}

ATLPREFAST_SUPPRESS(6014)
inline void CSecurityDesc::SetSacl(
	_In_ const CSacl &Sacl,
	_In_ bool bDefaulted /* = false */)
{
	if(m_pSecurityDescriptor)
		MakeAbsolute();

	PACL pNewSacl, pOldSacl = NULL;
	if(m_pSecurityDescriptor)
	{
		BOOL _bDefaulted, _bPresent;
		if(!::GetSecurityDescriptorSacl(m_pSecurityDescriptor, &_bPresent, &pOldSacl, &_bDefaulted))
			AtlThrowLastWin32();
	}
	else
		AllocateAndInitializeSecurityDescriptor();

	if(Sacl.IsNull() || Sacl.IsEmpty())
		pNewSacl = NULL;
	else
	{
		UINT nAclLength = Sacl.length();
		ATLASSERT(nAclLength > 0);

		pNewSacl = static_cast<ACL *>(malloc(nAclLength));
		if(!pNewSacl)
			AtlThrow(E_OUTOFMEMORY);

		Checked::memcpy_s(pNewSacl, nAclLength, Sacl.GetPACL(), nAclLength);
	}

	_Analysis_assume_(m_pSecurityDescriptor != NULL); // Guaranteed by "AllocateAndInitializeSecurityDescriptor"
	if(!::SetSecurityDescriptorSacl(m_pSecurityDescriptor, Sacl.IsNull() || pNewSacl, pNewSacl, bDefaulted))
	{
		HRESULT hr = AtlHresultFromLastError();
		free(pNewSacl);
		AtlThrow(hr);
	}

	free(pOldSacl);
}
ATLPREFAST_UNSUPPRESS()

inline _Success_(return != false) bool CSecurityDesc::GetOwner(
	_Out_ sid *sid,
	_Out_opt_ bool *pbDefaulted /* = NULL */) const
{
	ATLASSERT(sid);
	SID *pOwner;
	BOOL bDefaulted;

	if(!sid || !m_pSecurityDescriptor ||
		!::GetSecurityDescriptorOwner(m_pSecurityDescriptor, (PSID *) &pOwner, &bDefaulted))
	{
		return false;
	}

	*sid = *pOwner;

	if(pbDefaulted)
		*pbDefaulted = 0 != bDefaulted;

	return true;
}

inline _Success_(return != false) bool CSecurityDesc::GetGroup(
	_Out_ sid *sid,
	_Out_opt_ bool *pbDefaulted /* = NULL */) const
{
	ATLASSERT(sid);
	SID *pGroup;
	BOOL bDefaulted;

	if(!sid || !m_pSecurityDescriptor ||
		!::GetSecurityDescriptorGroup(m_pSecurityDescriptor, (PSID *) &pGroup, &bDefaulted))
	{
		return false;
	}

	*sid = *pGroup;

	if(pbDefaulted)
		*pbDefaulted = 0 != bDefaulted;

	return true;
}

inline _Success_(return != false) bool CSecurityDesc::GetDacl(
	_Out_ CDacl *pDacl,
	_Out_opt_ bool *pbPresent /* = NULL */,
	_Out_opt_ bool *pbDefaulted /* = NULL */) const
{
	ACL *pAcl;
	BOOL bPresent, bDefaulted;

	if(!m_pSecurityDescriptor ||
		!::GetSecurityDescriptorDacl(m_pSecurityDescriptor, &bPresent, &pAcl, &bDefaulted))
	{
		return false;
	}

	if(pDacl)
	{
		if(bPresent)
		{
			if(pAcl)
				*pDacl = *pAcl;
			else
				pDacl->SetNull();
		}
		else
			pDacl->SetEmpty();
	}

	if(pbPresent)
		*pbPresent = 0 != bPresent;

	if(pbDefaulted)
		*pbDefaulted = 0 != bDefaulted;

	return true;
}

inline _Success_(return != false) bool CSecurityDesc::GetSacl(
	_Out_ CSacl *pSacl,
	_Out_opt_ bool *pbPresent /* = NULL */,
	_Out_opt_ bool *pbDefaulted /* = NULL */) const
{
	ACL *pAcl;
	BOOL bPresent, bDefaulted;

	if(!m_pSecurityDescriptor ||
		!::GetSecurityDescriptorSacl(m_pSecurityDescriptor, &bPresent, &pAcl, &bDefaulted))
	{
		return false;
	}

	if(pSacl)
	{
		if(bPresent)
		{
			if(pAcl)
				*pSacl = *pAcl;
			else
				pSacl->SetNull();
		}
		else
			pSacl->SetEmpty();
	}

	if(pbPresent)
		*pbPresent = 0 != bPresent;

	if(pbDefaulted)
		*pbDefaulted = 0 != bDefaulted;

	return true;
}

inline bool CSecurityDesc::IsDaclDefaulted() const noexcept
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!GetControl(&sdc))
		return false;

	return (sdc & SE_DACL_PRESENT) &&
		(sdc & SE_DACL_DEFAULTED);
}

inline bool CSecurityDesc::IsDaclPresent() const noexcept
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!GetControl(&sdc))
		return false;

	return 0 != (sdc & SE_DACL_PRESENT);
}

inline bool CSecurityDesc::IsGroupDefaulted() const noexcept
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!GetControl(&sdc))
		return false;

	return 0 != (sdc & SE_GROUP_DEFAULTED);
}

inline bool CSecurityDesc::IsOwnerDefaulted() const noexcept
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!GetControl(&sdc))
		return false;

	return (sdc & SE_OWNER_DEFAULTED);
}

inline bool CSecurityDesc::IsSaclDefaulted() const noexcept
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!GetControl(&sdc))
		return false;

	return (sdc & SE_SACL_PRESENT) &&
		(sdc & SE_SACL_DEFAULTED);
}

inline bool CSecurityDesc::IsSaclPresent() const noexcept
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!GetControl(&sdc))
		return false;

	return 0 != (sdc & SE_SACL_PRESENT);
}

inline bool CSecurityDesc::IsSelfRelative() const noexcept
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!GetControl(&sdc))
		return false;

	return 0 != (sdc & SE_SELF_RELATIVE);
}

inline bool CSecurityDesc::IsDaclAutoInherited() const noexcept
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!GetControl(&sdc))
		return false;

	return 0 != (sdc & SE_DACL_AUTO_INHERITED);
}

inline bool CSecurityDesc::IsDaclProtected() const noexcept
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!GetControl(&sdc))
		return false;

	return 0 != (sdc & SE_DACL_PROTECTED);
}

inline bool CSecurityDesc::IsSaclAutoInherited() const noexcept
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!GetControl(&sdc))
		return false;

	return 0 != (sdc & SE_SACL_AUTO_INHERITED);
}

inline bool CSecurityDesc::IsSaclProtected() const noexcept
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!GetControl(&sdc))
		return false;

	return 0 != (sdc & SE_SACL_PROTECTED);
}

inline const SECURITY_DESCRIPTOR *CSecurityDesc::GetPSECURITY_DESCRIPTOR() const noexcept
{
	return m_pSecurityDescriptor;
}

inline CSecurityDesc::operator const SECURITY_DESCRIPTOR *() const noexcept
{
	return GetPSECURITY_DESCRIPTOR();
}

inline void CSecurityDesc::GetSECURITY_DESCRIPTOR(
	_Out_ SECURITY_DESCRIPTOR *pSD,
	_Inout_ LPDWORD lpdwBufferLength)
{
	ATLASSERT(lpdwBufferLength && m_pSecurityDescriptor);
	if(!lpdwBufferLength)
		AtlThrow(E_INVALIDARG);
	if (!m_pSecurityDescriptor)
		AtlThrow(E_UNEXPECTED);

	MakeAbsolute();

	if (!::MakeSelfRelativeSD(m_pSecurityDescriptor, pSD, lpdwBufferLength) &&
			(pSD || GetLastError() != ERROR_INSUFFICIENT_BUFFER))
		AtlThrowLastWin32();
}

inline UINT CSecurityDesc::length() noexcept
{
	return ::GetSecurityDescriptorLength(m_pSecurityDescriptor);
}

inline bool CSecurityDesc::GetControl(
	_Out_ SECURITY_DESCRIPTOR_CONTROL *psdc) const noexcept
{
	ATLASSERT(psdc);
	if(!psdc)
		return false;

	DWORD dwRev;
	*psdc = 0;
	if(!m_pSecurityDescriptor ||
		!::GetSecurityDescriptorControl(m_pSecurityDescriptor, psdc, &dwRev))
	{
		return false;
	}
	return true;
}

#if(_WIN32_WINNT >= 0x0500)
inline bool CSecurityDesc::SetControl(
	_In_ SECURITY_DESCRIPTOR_CONTROL ControlBitsOfInterest,
	_In_ SECURITY_DESCRIPTOR_CONTROL ControlBitsToSet) noexcept
{
	ATLASSUME(m_pSecurityDescriptor);
	if (!m_pSecurityDescriptor)
		return false;

	return 0 != ::SetSecurityDescriptorControl(m_pSecurityDescriptor,
		ControlBitsOfInterest, ControlBitsToSet);
}
#endif

inline void CSecurityDesc::MakeSelfRelative()
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!m_pSecurityDescriptor)
		return;

	if (!GetControl(&sdc))
		AtlThrow(E_FAIL);

	if(sdc & SE_SELF_RELATIVE)
		return;

	SECURITY_DESCRIPTOR *pSD;
	DWORD dwLen = 0;

	::MakeSelfRelativeSD(m_pSecurityDescriptor, NULL, &dwLen);
	if(::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		AtlThrowLastWin32();

	pSD = static_cast<SECURITY_DESCRIPTOR *>(malloc(dwLen));
	if(!pSD)
		AtlThrow(E_OUTOFMEMORY);

	if(!::MakeSelfRelativeSD(m_pSecurityDescriptor, pSD, &dwLen))
	{
		HRESULT hr = AtlHresultFromLastError();
		free(pSD);
		AtlThrow(hr);
	}

	Clear();
	m_pSecurityDescriptor = pSD;
}

ATLPREFAST_SUPPRESS(6014)
inline void CSecurityDesc::MakeAbsolute()
{
	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!m_pSecurityDescriptor)
		return;

	if (!GetControl(&sdc))
		AtlThrow(E_FAIL);;

	if(!(sdc & SE_SELF_RELATIVE))
		return;

	SECURITY_DESCRIPTOR *pSD;
	SID *pOwner, *pGroup;
	ACL *pDacl, *pSacl;
	DWORD dwSD, dwOwner, dwGroup, dwDacl, dwSacl;

	dwSD = dwOwner = dwGroup = dwDacl = dwSacl = 0;

	::MakeAbsoluteSD(m_pSecurityDescriptor, NULL, &dwSD, NULL, &dwDacl,
		NULL, &dwSacl, NULL, &dwOwner, NULL, &dwGroup);
	if(::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		AtlThrowLastWin32();

	pSD    = static_cast<SECURITY_DESCRIPTOR *>(malloc(dwSD));
	pOwner = static_cast<SID *>(dwOwner ? malloc(dwOwner) : NULL);
	pGroup = static_cast<SID *>(dwGroup ? malloc(dwGroup) : NULL);
	pDacl  = static_cast<ACL *>(dwDacl ? malloc(dwDacl) : NULL);
	pSacl  = static_cast<ACL *>(dwSacl ? malloc(dwSacl) : NULL);

	bool bFailed=false;
	HRESULT hr=S_OK;
	if(	!pSD ||
		(dwOwner && !pOwner) ||
		(dwGroup && !pGroup) ||
		(dwDacl && !pDacl) ||
		(dwSacl && !pSacl))
	{
		bFailed=true;
		hr=E_OUTOFMEMORY;
	}
	else
	{
		if(!::MakeAbsoluteSD(m_pSecurityDescriptor,
			pSD, &dwSD,
			pDacl, &dwDacl,
			pSacl, &dwSacl,
			pOwner, &dwOwner,
			pGroup, &dwGroup))
		{
			hr = AtlHresultFromLastError();
			bFailed=true;
		}
	}

	if(bFailed)
	{
		free(pSD);
		free(pOwner);
		free(pGroup);
		free(pDacl);
		free(pSacl);
		AtlThrow(hr);
	}

	Clear();
	m_pSecurityDescriptor = pSD;
}
ATLPREFAST_UNSUPPRESS()

inline void CSecurityDesc::Clear() noexcept
{
	if(m_pSecurityDescriptor)
	{
		SECURITY_DESCRIPTOR_CONTROL sdc;
		if(GetControl(&sdc) && !(sdc & SE_SELF_RELATIVE))
		{
			PSID pOwner, pGroup;
			ACL *pDacl, *pSacl;
			BOOL bDefaulted, bPresent;

			::GetSecurityDescriptorOwner(m_pSecurityDescriptor, &pOwner, &bDefaulted);
			free(pOwner);
			::GetSecurityDescriptorGroup(m_pSecurityDescriptor, &pGroup, &bDefaulted);
			free(pGroup);
			::GetSecurityDescriptorDacl(m_pSecurityDescriptor, &bPresent, &pDacl, &bDefaulted);
			if(bPresent)
				free(pDacl);
			::GetSecurityDescriptorSacl(m_pSecurityDescriptor, &bPresent, &pSacl, &bDefaulted);
			if(bPresent)
				free(pSacl);
		}
		free(m_pSecurityDescriptor);
		m_pSecurityDescriptor = NULL;
	}
}

_At_(this->m_pSecurityDescriptor, _Post_notnull_ _Post_writable_size_(1))
inline void CSecurityDesc::AllocateAndInitializeSecurityDescriptor()
{
	// m_pSecurityDescriptor should be NULL.
	ATLASSERT(!m_pSecurityDescriptor);

	m_pSecurityDescriptor = static_cast<SECURITY_DESCRIPTOR *>(malloc(sizeof(SECURITY_DESCRIPTOR)));
	if(!m_pSecurityDescriptor)
		AtlThrow(E_OUTOFMEMORY);

	if(!::InitializeSecurityDescriptor(m_pSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION))
	{
		HRESULT hr = AtlHresultFromLastError();
		free(m_pSecurityDescriptor);
		m_pSecurityDescriptor = NULL;
		AtlThrow(hr);
	}
}

inline void CSecurityDesc::Init(_In_ const SECURITY_DESCRIPTOR &rhs)
{
	SECURITY_DESCRIPTOR *pSD = const_cast<SECURITY_DESCRIPTOR *>(&rhs);
	DWORD dwRev, dwLen = ::GetSecurityDescriptorLength(pSD);

	m_pSecurityDescriptor = static_cast<SECURITY_DESCRIPTOR *>(malloc(dwLen));
	if(!m_pSecurityDescriptor)
		AtlThrow(E_OUTOFMEMORY);

	SECURITY_DESCRIPTOR_CONTROL sdc;
	if(!::GetSecurityDescriptorControl(pSD, &sdc, &dwRev))
	{
		HRESULT hr = AtlHresultFromLastError();
		free(m_pSecurityDescriptor);
		m_pSecurityDescriptor = NULL;
		AtlThrow(hr);
	}

	if(sdc & SE_SELF_RELATIVE)
	{
		Checked::memcpy_s(m_pSecurityDescriptor, dwLen, pSD, dwLen);
	}
	else
	{
		if(!::MakeSelfRelativeSD(pSD, m_pSecurityDescriptor, &dwLen))
		{
			HRESULT hr = AtlHresultFromLastError();
			free(m_pSecurityDescriptor);
			m_pSecurityDescriptor = NULL;
			AtlThrow(hr);
		}
	}
}

//******************************************
// CSecurityAttributes implementation

inline CSecurityAttributes::CSecurityAttributes() noexcept
{
	nLength = 0;
	lpSecurityDescriptor = NULL;
	bInheritHandle = FALSE;
}

inline CSecurityAttributes::CSecurityAttributes(
		_In_ const CSecurityDesc &rSecurityDescriptor,
		_In_ bool bInheritsHandle /* = false */) :
	m_SecurityDescriptor(rSecurityDescriptor)
{
	Set(m_SecurityDescriptor, bInheritsHandle);
}

inline void CSecurityAttributes::Set(
	_In_ const CSecurityDesc &rSecurityDescriptor,
	_In_ bool bInheritsHandle /* = false */)
{
	m_SecurityDescriptor = rSecurityDescriptor;
	nLength = sizeof(SECURITY_ATTRIBUTES);
	lpSecurityDescriptor = const_cast<SECURITY_DESCRIPTOR *>
		(m_SecurityDescriptor.GetPSECURITY_DESCRIPTOR());
	this->bInheritHandle = bInheritsHandle;
}

//******************************************************
// CTokenPrivileges implementation

inline CTokenPrivileges::CTokenPrivileges() noexcept :
	m_bDirty(true),
	m_pTokenPrivileges(NULL)
{
}

inline CTokenPrivileges::~CTokenPrivileges() noexcept
{
	free(m_pTokenPrivileges);
}

inline CTokenPrivileges::CTokenPrivileges(
		_In_ const CTokenPrivileges &rhs) :
	m_pTokenPrivileges(NULL),
	m_bDirty(true)
{
	const Map::CPair *pPair;
	POSITION pos = rhs.m_mapTokenPrivileges.GetStartPosition();
	while(pos)
	{
		pPair = rhs.m_mapTokenPrivileges.GetNext(pos);
		m_mapTokenPrivileges.SetAt(pPair->m_key, pPair->m_value);
	}
}

inline CTokenPrivileges &CTokenPrivileges::operator=(
	_In_ const CTokenPrivileges &rhs)
{
	if(this != &rhs)
	{
		m_mapTokenPrivileges.RemoveAll();

		const Map::CPair *pPair;
		POSITION pos = rhs.m_mapTokenPrivileges.GetStartPosition();
		while(pos)
		{
			pPair = rhs.m_mapTokenPrivileges.GetNext(pos);
			m_mapTokenPrivileges.SetAt(pPair->m_key, pPair->m_value);
		}
		m_bDirty = true;
	}
	return *this;
}

inline CTokenPrivileges::CTokenPrivileges(
		_In_ const TOKEN_PRIVILEGES &rPrivileges) :
	m_pTokenPrivileges(NULL)
{
	AddPrivileges(rPrivileges);
}

inline CTokenPrivileges &CTokenPrivileges::operator=(
	_In_ const TOKEN_PRIVILEGES &rPrivileges)
{
	m_mapTokenPrivileges.RemoveAll();
	AddPrivileges(rPrivileges);
	return *this;
}

inline void CTokenPrivileges::Add(
	_In_ const TOKEN_PRIVILEGES &rPrivileges)
{
	AddPrivileges(rPrivileges);
}

inline bool CTokenPrivileges::Add(
	_In_z_ LPCTSTR pszPrivilege,
	_In_ bool bEnable)
{
	LUID_AND_ATTRIBUTES la;
	if(!::LookupPrivilegeValue(NULL, pszPrivilege, &la.Luid))
		return false;

	la.Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;

	m_mapTokenPrivileges.SetAt(la.Luid, la.Attributes);

	m_bDirty = true;
	return true;
}

inline _Success_(return != false) bool CTokenPrivileges::LookupPrivilege(
	_In_z_ LPCTSTR pszPrivilege,
	_Out_opt_ DWORD *pdwAttributes /* = NULL */) const
{
	DWORD dwAttributes;
	LUID luid;

	if(!::LookupPrivilegeValue(NULL, pszPrivilege, &luid))
		return false;

	if(m_mapTokenPrivileges.Lookup(luid, dwAttributes))
	{
		if(pdwAttributes)
			*pdwAttributes = dwAttributes;
		return true;
	}
	return false;
}

inline void CTokenPrivileges::GetNamesAndAttributes(
	_Inout_ CNames *pNames,
	_Inout_opt_ CAttributes *pAttributes /* = NULL */) const
{
	ATLASSERT(pNames);
	if(pNames)
	{
		CAutoVectorPtr<TCHAR> psz;
		DWORD cbName = 0, cbTmp;
		const Map::CPair *pPair;

		pNames->RemoveAll();
		if(pAttributes)
			pAttributes->RemoveAll();

		POSITION pos = m_mapTokenPrivileges.GetStartPosition();
		while(pos)
		{
			pPair = m_mapTokenPrivileges.GetNext(pos);

			cbTmp = cbName;
#pragma warning(push)
#pragma warning(disable: 6535)
			/* prefast noise VSW 492728 */
			if(!::LookupPrivilegeName(NULL, const_cast<LUID *>(&pPair->m_key), psz, &cbTmp))
#pragma warning(pop)
				if(::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					psz.Free();
					if(!psz.Allocate(cbTmp + 1))
					{
						pNames->RemoveAll();
						if(pAttributes)
							pAttributes->RemoveAll();
						AtlThrow(E_OUTOFMEMORY);
					}

					cbName = cbTmp;
					if(!::LookupPrivilegeName(NULL, const_cast<LUID *>(&pPair->m_key), psz, &cbTmp))
						break;
				}
				else
					break;

			pNames->Add((LPTSTR) psz);
			if(pAttributes)
				pAttributes->Add(pPair->m_value);
		}

		if(pos)
		{
			pNames->RemoveAll();
			if(pAttributes)
				pAttributes->RemoveAll();
		}
	}
}

inline void CTokenPrivileges::GetDisplayNames(
	_Inout_ CNames *pDisplayNames) const
{
	ATLASSERT(pDisplayNames);
	if(pDisplayNames)
	{
		DWORD dwLang, cbTmp, cbDisplayName = 0;
		CAutoVectorPtr<TCHAR> psz;
		CNames Names;
		UINT i;

		GetNamesAndAttributes(&Names);

		pDisplayNames->RemoveAll();

		for(i = 0; i < Names.GetCount(); i++)
		{
			cbTmp = cbDisplayName;
#pragma warning(push)
#pragma warning(disable: 6535)
			/* prefast noise VSW 492728 */
			if(!::LookupPrivilegeDisplayName(NULL, Names[i], psz, &cbTmp, &dwLang))
#pragma warning(pop)
			{
				if(::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					psz.Free();
					if(!psz.Allocate(cbTmp + 1))
					{
						pDisplayNames->RemoveAll();
						AtlThrow(E_OUTOFMEMORY);
					}

					cbDisplayName = cbTmp;
					if(!::LookupPrivilegeDisplayName(NULL, Names[i], psz, &cbTmp, &dwLang))
						break;
				}
				else
					break;
			}
			pDisplayNames->Add((LPTSTR) psz);
		}

		if(i != Names.GetCount())
			pDisplayNames->RemoveAll();
	}
}

inline void CTokenPrivileges::GetLuidsAndAttributes(
	_Inout_ CLUIDArray *pLuids,
	_Inout_opt_ CAttributes *pAttributes /* = NULL */) const
{
	ATLASSERT(pLuids);
	if(pLuids)
	{
		const Map::CPair *pPair;

		pLuids->RemoveAll();
		if(pAttributes)
			pAttributes->RemoveAll();

		POSITION pos = m_mapTokenPrivileges.GetStartPosition();
		while(pos)
		{
			pPair = m_mapTokenPrivileges.GetNext(pos);
			pLuids->Add(pPair->m_key);
			if(pAttributes)
				pAttributes->Add(pPair->m_value);
		}
	}
}

inline bool CTokenPrivileges::Delete(_In_z_ LPCTSTR pszPrivilege) noexcept
{
	LUID Luid;
	if(!::LookupPrivilegeValue(NULL, pszPrivilege, &Luid))
		return false;

	if(!m_mapTokenPrivileges.RemoveKey(Luid))
		return false;

	m_bDirty = true;
	return true;
}

inline void CTokenPrivileges::DeleteAll() noexcept
{
	m_mapTokenPrivileges.RemoveAll();
	m_bDirty = true;
}

inline UINT CTokenPrivileges::GetCount() const noexcept
{
	return (UINT) m_mapTokenPrivileges.GetCount();
}

inline UINT CTokenPrivileges::length() const noexcept
{
	return offsetof(TOKEN_PRIVILEGES, Privileges) + sizeof(LUID_AND_ATTRIBUTES) * GetCount();
}

inline const TOKEN_PRIVILEGES *CTokenPrivileges::GetPTOKEN_PRIVILEGES() const
{
	if(m_bDirty)
	{
		free(m_pTokenPrivileges);
		m_pTokenPrivileges = NULL;

		if(m_mapTokenPrivileges.GetCount())
		{
			m_pTokenPrivileges = static_cast<TOKEN_PRIVILEGES *>(malloc(length()));
			if(!m_pTokenPrivileges)
				AtlThrow(E_OUTOFMEMORY);

			m_pTokenPrivileges->PrivilegeCount = (DWORD) GetCount();

			UINT i = 0;
			POSITION pos = m_mapTokenPrivileges.GetStartPosition();
			const Map::CPair *pPair;
			while(pos)
			{
				pPair = m_mapTokenPrivileges.GetNext(pos);
				m_pTokenPrivileges->Privileges[i].Luid = pPair->m_key;
				m_pTokenPrivileges->Privileges[i].Attributes = pPair->m_value;

				i++;
			}
		}
	}
	return m_pTokenPrivileges;
}

inline CTokenPrivileges::operator const TOKEN_PRIVILEGES *() const
{
	return GetPTOKEN_PRIVILEGES();
}

inline void CTokenPrivileges::AddPrivileges(
	_In_ const TOKEN_PRIVILEGES &rPrivileges)
{
	m_bDirty = true;
	for(UINT i = 0; i < rPrivileges.PrivilegeCount; i++)
#pragma warning(push)
#pragma warning(disable: 6385)
		/* prefast noise VSW 492737 */
		m_mapTokenPrivileges.SetAt(
			rPrivileges.Privileges[i].Luid, rPrivileges.Privileges[i].Attributes);
#pragma warning(pop)
}

//******************************************************
// CTokenGroups implementation

inline CTokenGroups::CTokenGroups() noexcept :
	m_pTokenGroups(NULL),
	m_bDirty(true)
{
}

inline CTokenGroups::~CTokenGroups() noexcept
{
	free(m_pTokenGroups);
}

inline CTokenGroups::CTokenGroups(
		_In_ const CTokenGroups &rhs) :
	m_pTokenGroups(NULL),
	m_bDirty(true)
{
	const Map::CPair *pPair;
	POSITION pos = rhs.m_mapTokenGroups.GetStartPosition();
	while(pos)
	{
		pPair = rhs.m_mapTokenGroups.GetNext(pos);
		m_mapTokenGroups.SetAt(pPair->m_key, pPair->m_value);
	}
}

inline CTokenGroups &CTokenGroups::operator=(
	_In_ const CTokenGroups &rhs)
{
	if(this != &rhs)
	{
		m_mapTokenGroups.RemoveAll();

		const Map::CPair *pPair;
		POSITION pos = rhs.m_mapTokenGroups.GetStartPosition();
		while(pos)
		{
			pPair = rhs.m_mapTokenGroups.GetNext(pos);
			m_mapTokenGroups.SetAt(pPair->m_key, pPair->m_value);
		}
		m_bDirty = true;
	}
	return *this;
}

inline CTokenGroups::CTokenGroups(
		_In_ const TOKEN_GROUPS &rhs) :
	m_pTokenGroups(NULL)
{
	AddTokenGroups(rhs);
}

inline CTokenGroups &CTokenGroups::operator=(
	_In_ const TOKEN_GROUPS &rhs)
{
	m_mapTokenGroups.RemoveAll();
	AddTokenGroups(rhs);
	return *this;
}

inline void CTokenGroups::Add(
	_In_ const TOKEN_GROUPS &rTokenGroups)
{
	AddTokenGroups(rTokenGroups);
}

inline void CTokenGroups::Add(
	_In_ const sid &rSid,
	_In_ DWORD dwAttributes)
{
	m_mapTokenGroups.SetAt(rSid, dwAttributes);
	m_bDirty = true;
}

inline _Success_(return != false) bool CTokenGroups::LookupSid(
	_In_ const sid &rSid,
	_Out_opt_ DWORD *pdwAttributes /* = NULL */) const noexcept
{
	DWORD dwAttributes;
	if(m_mapTokenGroups.Lookup(rSid, dwAttributes))
	{
		if(pdwAttributes)
			*pdwAttributes = dwAttributes;
		return true;
	}
	return false;
}

inline void CTokenGroups::GetSidsAndAttributes(
	_Inout_ std::vector<sid> *pSids,
	_Inout_opt_ CAtlArray<DWORD> *pAttributes /* = NULL */) const
{
	ATLASSERT(pSids);
	if(pSids)
	{
		const Map::CPair *pPair;

		pSids->clear();
		if(pAttributes)
			pAttributes->RemoveAll();

		POSITION pos = m_mapTokenGroups.GetStartPosition();
		while(pos)
		{
			pPair = m_mapTokenGroups.GetNext(pos);
			pSids->push_back(pPair->m_key);
			if(pAttributes)
				pAttributes->Add(pPair->m_value);
		}
	}
}

inline bool CTokenGroups::Delete(
	_In_ const sid &rSid) noexcept
{
	return m_mapTokenGroups.RemoveKey(rSid);
}

inline void CTokenGroups::DeleteAll() noexcept
{
	m_mapTokenGroups.RemoveAll();
	m_bDirty = true;
}

inline UINT CTokenGroups::GetCount() const noexcept
{
	return (UINT) m_mapTokenGroups.GetCount();
}

inline UINT CTokenGroups::length() const noexcept
{
	return UINT(offsetof(TOKEN_GROUPS, Groups) + sizeof(SID_AND_ATTRIBUTES) * m_mapTokenGroups.GetCount());
}

inline const TOKEN_GROUPS *CTokenGroups::GetPTOKEN_GROUPS() const
{
	if(m_bDirty)
	{
		free(m_pTokenGroups);
		m_pTokenGroups = NULL;

		if(m_mapTokenGroups.GetCount())
		{
			m_pTokenGroups = static_cast<TOKEN_GROUPS *>(malloc(length()));
			if(!m_pTokenGroups)
				AtlThrow(E_OUTOFMEMORY);

			m_pTokenGroups->GroupCount = (DWORD) m_mapTokenGroups.GetCount();

			UINT i = 0;
			POSITION pos = m_mapTokenGroups.GetStartPosition();
			const Map::CPair *pPair;
			while(pos)
			{
				pPair = m_mapTokenGroups.GetNext(pos);
				m_pTokenGroups->Groups[i].Sid = const_cast<SID *>(pPair->m_key.get_sid());
				m_pTokenGroups->Groups[i].Attributes = pPair->m_value;

				i++;
			}
		}
	}
	return m_pTokenGroups;
}

inline CTokenGroups::operator const TOKEN_GROUPS *() const
{
	return GetPTOKEN_GROUPS();
}

inline void CTokenGroups::AddTokenGroups(
	_In_ const TOKEN_GROUPS &rTokenGroups)
{
	m_bDirty = true;
	for(UINT i = 0; i < rTokenGroups.GroupCount; i++)
#pragma warning(push)
#pragma warning(disable: 6385)
		/* prefast noise VSW 492737 */
		m_mapTokenGroups.SetAt(
			sid(static_cast<SID *>(rTokenGroups.Groups[i].Sid)),
			rTokenGroups.Groups[i].Attributes);
#pragma warning(pop)
}

// *************************************
// CAccessToken implementation

inline CAccessToken::CAccessToken() noexcept :
	m_hToken(NULL),
	m_hProfile(NULL),
	m_pRevert(NULL)
{
}

inline CAccessToken::~CAccessToken() noexcept
{
	Clear();
}

inline void CAccessToken::Attach(_In_ HANDLE hToken) noexcept
{
	ATLASSUME(m_hToken == NULL);
	m_hToken = hToken;
}

inline HANDLE CAccessToken::Detach() noexcept
{
	HANDLE hToken = m_hToken;
	m_hToken = NULL;
	Clear();
	return hToken;
}

inline HANDLE CAccessToken::GetHandle() const noexcept
{
	return m_hToken;
}

inline HKEY CAccessToken::HKeyCurrentUser() const noexcept
{
	return reinterpret_cast<HKEY>(m_hProfile);
}

inline _Success_(return != false) bool CAccessToken::EnablePrivilege(
	_In_z_ LPCTSTR pszPrivilege,
	_In_opt_ CTokenPrivileges *pPreviousState /* = NULL */,
	_Out_opt_ bool* pbErrNotAllAssigned /*=NULL*/)
{
	CTokenPrivileges NewState;
	NewState.Add(pszPrivilege, true);
	return EnableDisablePrivileges(NewState, pPreviousState,pbErrNotAllAssigned);
}

inline _Success_(return != false) bool CAccessToken::EnablePrivileges(
	_In_ const CAtlArray<LPCTSTR> &rPrivileges,
	_Inout_opt_ CTokenPrivileges *pPreviousState /* = NULL */,
	_Out_opt_ bool* pbErrNotAllAssigned /*=NULL*/)
{
	return EnableDisablePrivileges(rPrivileges, true, pPreviousState,pbErrNotAllAssigned);
}

inline _Success_(return != false) bool CAccessToken::DisablePrivilege(
	_In_z_ LPCTSTR pszPrivilege,
	_In_opt_ CTokenPrivileges *pPreviousState /* = NULL */,
	_Out_opt_ bool* pbErrNotAllAssigned /*=NULL*/)
{
	CTokenPrivileges NewState;
	NewState.Add(pszPrivilege, false);
	return EnableDisablePrivileges(NewState, pPreviousState,pbErrNotAllAssigned);
}

inline _Success_(return != false) bool CAccessToken::DisablePrivileges(
	_In_ const CAtlArray<LPCTSTR> &rPrivileges,
	_Inout_opt_ CTokenPrivileges *pPreviousState /* = NULL */,
	_Out_opt_ bool* pbErrNotAllAssigned /*=NULL*/)
{
	return EnableDisablePrivileges(rPrivileges, false, pPreviousState,pbErrNotAllAssigned);
}

ATLPREFAST_SUPPRESS(6101)
inline _Success_(return != false) bool CAccessToken::EnableDisablePrivileges(
	_In_ const CTokenPrivileges &rNewState,
	_Inout_opt_ CTokenPrivileges *pPreviousState /* = NULL */,
	_Out_opt_ bool* pbErrNotAllAssigned /*=NULL*/)
{
	if(!rNewState.GetCount())
	{
		return true;
	}

	TOKEN_PRIVILEGES *pNewState = const_cast<TOKEN_PRIVILEGES *>(rNewState.GetPTOKEN_PRIVILEGES());

	DWORD dwLength = DWORD(offsetof(TOKEN_PRIVILEGES, Privileges) +
		rNewState.GetCount() * sizeof(LUID_AND_ATTRIBUTES));

	USES_ATL_SAFE_ALLOCA;
	TOKEN_PRIVILEGES *pPrevState = static_cast<TOKEN_PRIVILEGES *>(_ATL_SAFE_ALLOCA(dwLength, _ATL_SAFE_ALLOCA_DEF_THRESHOLD));
	if(::AdjustTokenPrivileges(m_hToken, FALSE, pNewState, dwLength, pPrevState, &dwLength))
	{
		if (pbErrNotAllAssigned)
		{
			if(::GetLastError() == ERROR_NOT_ALL_ASSIGNED)
			{
				*pbErrNotAllAssigned=true;
			} else
			{
				*pbErrNotAllAssigned=false;
			}
		}
	} else
	{
		return false;
	}

	if(pPreviousState != NULL && pPrevState != NULL)
	{
		pPreviousState->Add(*pPrevState);
	}

	return true;
}
ATLPREFAST_UNSUPPRESS()

inline _Success_(return != false) bool CAccessToken::PrivilegeCheck(
	_In_ PPRIVILEGE_SET RequiredPrivileges,
	_Out_ bool *pbResult) const noexcept
{
	BOOL bResult;
	if(!::PrivilegeCheck(m_hToken, RequiredPrivileges, &bResult))
		return false;

	*pbResult = 0 != bResult;
	return true;
}

inline bool CAccessToken::GetLogonSid(_Inout_ sid *sid) const
{
	ATLASSERT(sid);
	if(!sid)
		return false;

	DWORD dwLen;
	::GetTokenInformation(m_hToken, TokenGroups, NULL, 0, &dwLen);
	if(::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		return false;

	USES_ATL_SAFE_ALLOCA;
	TOKEN_GROUPS *pGroups = static_cast<TOKEN_GROUPS *>(_ATL_SAFE_ALLOCA(dwLen, _ATL_SAFE_ALLOCA_DEF_THRESHOLD));
	if(::GetTokenInformation(m_hToken, TokenGroups, pGroups, dwLen, &dwLen))
	{
		ATLASSUME(pGroups != NULL);
		for(UINT i = 0; i < pGroups->GroupCount; i++)
		{
			if(pGroups->Groups[i].Attributes & SE_GROUP_LOGON_ID)
			{
				*sid = *static_cast<SID *>(pGroups->Groups[i].Sid);
				return true;
			}
		}
	}
	return false;
}

inline _Success_(return != false) bool CAccessToken::GetTokenId(_Out_ LUID *pluid) const
{
	ATLASSERT(pluid);
	if(!pluid)
		return false;

	TOKEN_STATISTICS Statistics = { 0 };
	if(!GetStatistics(&Statistics))
		return false;

	*pluid = Statistics.TokenId;
	return true;
}

inline _Success_(return != false) bool CAccessToken::GetLogonSessionId(_Out_ LUID *pluid) const
{
	ATLASSERT(pluid);
	if(!pluid)
		return false;

	TOKEN_STATISTICS Statistics = { 0 };
	if(!GetStatistics(&Statistics))
		return false;

	*pluid = Statistics.AuthenticationId;
	return true;
}

inline bool CAccessToken::CheckTokenMembership(
	_In_ const sid &rSid,
	_Inout_ bool *pbIsMember) const
{
	// "this" must be an impersonation token and NOT a primary token
	BOOL bIsMember;

	ATLASSERT(pbIsMember);
	if (!pbIsMember)
		return false;

#if(_WIN32_WINNT >= 0x0500)
	if(::CheckTokenMembership(m_hToken, const_cast<SID *>(rSid.get_sid()), &bIsMember))
#else
	GENERIC_MAPPING gm = {0, 0, 0, 1};
	PRIVILEGE_SET ps;
	DWORD cb = sizeof(PRIVILEGE_SET);
	DWORD ga;
	CSecurityDesc sd;
	CDacl dacl;

	if (!dacl.AddAllowedAce(rSid, 1))
		return false;
	sd.SetOwner(rSid);
	sd.SetGroup(rSid);
	sd.SetDacl(dacl);

	if(::AccessCheck(const_cast<SECURITY_DESCRIPTOR *>(sd.GetPSECURITY_DESCRIPTOR()),
		m_hToken, 1, &gm, &ps, &cb, &ga, &bIsMember))
#endif
	{
		*pbIsMember = 0 != bIsMember;
		return true;
	}
	return false;
}

#if(_WIN32_WINNT >= 0x0500)
inline bool CAccessToken::IsTokenRestricted() const noexcept
{
	return 0 != ::IsTokenRestricted(m_hToken);
}
#endif

inline bool CAccessToken::GetDefaultDacl(_Inout_ CDacl *pDacl) const
{
	return GetInfoConvert<CDacl, TOKEN_DEFAULT_DACL>(pDacl, TokenDefaultDacl);
}

inline bool CAccessToken::GetGroups(_Inout_ CTokenGroups *pGroups) const
{
	return GetInfoConvert<CTokenGroups, TOKEN_GROUPS>(pGroups, TokenGroups);
}

inline bool CAccessToken::GetImpersonationLevel(
	_Inout_ SECURITY_IMPERSONATION_LEVEL *pImpersonationLevel) const
{
	return GetInfo<SECURITY_IMPERSONATION_LEVEL>(pImpersonationLevel, TokenImpersonationLevel);
}

inline bool CAccessToken::GetOwner(_Inout_ sid *sid) const
{
	return GetInfoConvert<ATL::sid, TOKEN_OWNER>(sid, TokenOwner);
}

inline bool CAccessToken::GetPrimaryGroup(_Inout_ sid *sid) const
{
	return GetInfoConvert<ATL::sid, TOKEN_PRIMARY_GROUP>(sid, TokenPrimaryGroup);
}

inline bool CAccessToken::GetPrivileges(_Inout_ CTokenPrivileges *pPrivileges) const
{
	return GetInfoConvert<CTokenPrivileges, TOKEN_PRIVILEGES>(pPrivileges, TokenPrivileges);
}

inline bool CAccessToken::GetTerminalServicesSessionId(_Inout_ DWORD *pdwSessionId) const
{
	return GetInfo<DWORD>(pdwSessionId, TokenSessionId);
}

inline bool CAccessToken::GetSource(_Inout_ TOKEN_SOURCE *pSource) const
{
	return GetInfo<TOKEN_SOURCE>(pSource, TokenSource);
}

inline bool CAccessToken::GetStatistics(_Inout_ TOKEN_STATISTICS *pStatistics) const
{
	return GetInfo<TOKEN_STATISTICS>(pStatistics, TokenStatistics);
}

inline bool CAccessToken::GetType(_Inout_ TOKEN_TYPE *pType) const
{
	return GetInfo<TOKEN_TYPE>(pType, TokenType);
}

inline bool CAccessToken::GetUser(_Inout_ sid *sid) const
{
	return GetInfoConvert<ATL::sid, TOKEN_USER>(sid, TokenUser);
}

inline bool CAccessToken::SetOwner(_In_ const sid &rSid)

{
	TOKEN_OWNER to;
	to.Owner = const_cast<SID *>(rSid.get_sid());
	return 0 != ::SetTokenInformation(m_hToken, TokenOwner, &to, sizeof(to));
}

inline bool CAccessToken::SetPrimaryGroup(_In_ const sid &rSid)
{
	TOKEN_PRIMARY_GROUP tpg;
	tpg.PrimaryGroup = const_cast<SID *>(rSid.get_sid());
	return 0 != ::SetTokenInformation(m_hToken, TokenPrimaryGroup, &tpg, sizeof(tpg));
}

inline bool CAccessToken::SetDefaultDacl(_In_ const CDacl &rDacl)
{
	TOKEN_DEFAULT_DACL tdd;
	tdd.DefaultDacl = const_cast<ACL *>(rDacl.GetPACL());
	return 0 != ::SetTokenInformation(m_hToken, TokenDefaultDacl, &tdd, sizeof(tdd));
}

inline bool CAccessToken::CreateImpersonationToken(
	_Inout_ CAccessToken *pImp,
	_In_ SECURITY_IMPERSONATION_LEVEL sil /* = SecurityImpersonation */) const
{
	ATLASSERT(pImp);
	if(!pImp)
		return false;

	HANDLE hToken;
	if(!::DuplicateToken(m_hToken, sil, &hToken))
		return false;

	pImp->Clear();
	pImp->m_hToken = hToken;
	return true;
}

inline bool CAccessToken::CreatePrimaryToken(
	_Inout_ CAccessToken *pPri,
	_In_ DWORD dwDesiredAccess /* = MAXIMUM_ALLOWED */,
	_In_opt_ const CSecurityAttributes *pTokenAttributes /* = NULL */) const
{
	ATLASSERT(pPri);
	if(!pPri)
		return false;

	HANDLE hToken;
	if(!::DuplicateTokenEx(m_hToken, dwDesiredAccess,
		const_cast<CSecurityAttributes *>(pTokenAttributes),
		SecurityAnonymous, TokenPrimary, &hToken))
	{
		return false;
	}

	pPri->Clear();
	pPri->m_hToken = hToken;
	return true;
}

#if(_WIN32_WINNT >= 0x0500)

inline bool CAccessToken::CreateRestrictedToken(
	_Inout_ CAccessToken *pRestrictedToken,
	_In_ const CTokenGroups &SidsToDisable,
	_In_ const CTokenGroups &SidsToRestrict,
	_In_ const CTokenPrivileges &PrivilegesToDelete /* = CTokenPrivileges() */) const
{
	ATLASSERT(pRestrictedToken);
	if(!pRestrictedToken)
		return false;

	HANDLE hToken;
	SID_AND_ATTRIBUTES *pSidsToDisable;
	SID_AND_ATTRIBUTES *pSidsToRestrict;
	LUID_AND_ATTRIBUTES *pPrivilegesToDelete;

	UINT nDisableSidCount = SidsToDisable.GetCount();
	if(nDisableSidCount)
	{
		const TOKEN_GROUPS * pTOKEN_GROUPS = SidsToDisable.GetPTOKEN_GROUPS();

		ATLASSERT(pTOKEN_GROUPS != NULL);

		if(pTOKEN_GROUPS != NULL)
		{
			pSidsToDisable = const_cast<SID_AND_ATTRIBUTES *>
				(pTOKEN_GROUPS->Groups);
		}
		else
		{
			return false;
		}
	}
	else
	{
		pSidsToDisable = NULL;
	}

	UINT nRestrictedSidCount = SidsToRestrict.GetCount();
	if(nRestrictedSidCount)
	{
		const TOKEN_GROUPS * pTOKEN_GROUPS = SidsToRestrict.GetPTOKEN_GROUPS();

		ATLASSERT(pTOKEN_GROUPS != NULL);

		if(pTOKEN_GROUPS != NULL)
		{
			pSidsToRestrict = const_cast<SID_AND_ATTRIBUTES *>
				(pTOKEN_GROUPS->Groups);
		}
		else
		{
			return false;
		}

	}
	else
	{
		pSidsToRestrict = NULL;
	}

	UINT nDeletePrivilegesCount = PrivilegesToDelete.GetCount();
	if(nDeletePrivilegesCount)
	{
		const TOKEN_PRIVILEGES * pTOKEN_PRIVILEGES = PrivilegesToDelete.GetPTOKEN_PRIVILEGES();

		ATLASSERT(pTOKEN_PRIVILEGES != NULL);

		if(pTOKEN_PRIVILEGES != NULL)
		{
			pPrivilegesToDelete = const_cast<LUID_AND_ATTRIBUTES *>
				(pTOKEN_PRIVILEGES->Privileges);
		}
		else
		{
			return false;
		}

	}
	else
	{
		pPrivilegesToDelete = NULL;
	}

	if(!::CreateRestrictedToken(m_hToken, 0,
		(DWORD) nDisableSidCount, pSidsToDisable,
		(DWORD) nDeletePrivilegesCount, pPrivilegesToDelete,
		(DWORD) nRestrictedSidCount, pSidsToRestrict, &hToken))
	{
		return false;
	}

	pRestrictedToken->Clear();
	pRestrictedToken->m_hToken = hToken;
	return true;
}

#endif // _WIN32_WINNT >= 0x0500

inline bool CAccessToken::GetProcessToken(
	_In_ DWORD dwDesiredAccess,
	_In_opt_ HANDLE hProcess /* = NULL */) noexcept
{
	if(!hProcess)
		hProcess = ::GetCurrentProcess();

	HANDLE hToken;
	if(!::OpenProcessToken(hProcess, dwDesiredAccess, &hToken))
		return false;

	Clear();
	m_hToken = hToken;
	return true;
}

inline bool CAccessToken::GetThreadToken(
	_In_ DWORD dwDesiredAccess,
	_In_opt_ HANDLE hThread /* = NULL */,
	_In_ bool bOpenAsSelf /* = true */) noexcept
{
	if(!hThread)
		hThread = ::GetCurrentThread();

	HANDLE hToken;
	if(!::OpenThreadToken(hThread, dwDesiredAccess, bOpenAsSelf, &hToken))
		return false;

	Clear();
	m_hToken = hToken;

	return true;
}

inline bool CAccessToken::GetEffectiveToken(_In_ DWORD dwDesiredAccess) noexcept
{
	if(GetThreadToken(dwDesiredAccess))
		return true;

	// If the specified thread does not have an assigned token, then get the process token.
	if (::GetLastError() == ERROR_NO_TOKEN)
		return GetProcessToken(dwDesiredAccess);

	// ...otherwise OpenThreadToken has failed for other reasons.
	return false;
}

inline bool CAccessToken::OpenThreadToken(
	_In_ DWORD dwDesiredAccess,
	_In_ bool bImpersonate /* = false */,
	_In_ bool bOpenAsSelf /* = true */,
	_In_ SECURITY_IMPERSONATION_LEVEL sil /* = SecurityImpersonation */)
{
	// If bImpersonate == false the original impersonate level should be restored after getting token.
	SECURITY_IMPERSONATION_LEVEL silCurrent = SecurityAnonymous; 
	bool bNeedRestoreLevel = CheckImpersonation() && !bImpersonate;
	if(bNeedRestoreLevel)
	{
		// Calling thread is impersonating. The impersonation level should be recorded down.
		bNeedRestoreLevel = GetImpersonationLevel(&silCurrent);
	}

	if(!::ImpersonateSelf(sil))
		return false;

	HANDLE hToken;
	if(!::OpenThreadToken(::GetCurrentThread(), dwDesiredAccess, bOpenAsSelf, &hToken))
	{
		// The function call failed returning to original impersonation level.
		RevertToLevel(bNeedRestoreLevel ? &silCurrent : NULL);
		return false;
	}

	Clear();
	m_hToken = hToken;

	if(!bImpersonate)
	{
		// Restore the original impersonation level.
		RevertToLevel(bNeedRestoreLevel ? &silCurrent : NULL);
	}
	else
	{
		m_pRevert = _ATL_NEW CRevertToSelf;
		if(!m_pRevert)
		{
			::RevertToSelf();
			Clear();
			return false;
		}
	}
	return true;
}

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)
inline bool CAccessToken::OpenCOMClientToken(
	_In_ DWORD dwDesiredAccess,
	_In_ bool bImpersonate /* = false */,
	_In_ bool bOpenAsSelf /* = true */)
{
	CheckImpersonation();

	if(FAILED(::CoImpersonateClient()))
		return false;

	HANDLE hToken;
	if(!::OpenThreadToken(::GetCurrentThread(), dwDesiredAccess, bOpenAsSelf, &hToken))
		return false;

	Clear();
	m_hToken = hToken;

	if(!bImpersonate)
	{
		HRESULT hr = ::CoRevertToSelf();
		ATLASSERT(SUCCEEDED(hr));
		UNREFERENCED_PARAMETER(hr);
	}
	else
	{
		m_pRevert = _ATL_NEW CCoRevertToSelf;
		if(!m_pRevert)
		{
			HRESULT hr = ::CoRevertToSelf();
			ATLASSERT(SUCCEEDED(hr));
			UNREFERENCED_PARAMETER(hr);
			Clear();
			return false;
		}
	}
	return true;
}
#endif //(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)

inline bool CAccessToken::OpenNamedPipeClientToken(
	_In_ HANDLE hPipe,
	_In_ DWORD dwDesiredAccess,
	_In_ bool bImpersonate /* = false */,
	_In_ bool bOpenAsSelf /* = true */)
{
	CheckImpersonation();

	if(!::ImpersonateNamedPipeClient(hPipe))
		return false;

	HANDLE hToken;
	if(!::OpenThreadToken(::GetCurrentThread(), dwDesiredAccess, bOpenAsSelf, &hToken))
		return false;

	Clear();
	m_hToken = hToken;

	if(!bImpersonate)
		::RevertToSelf();
	else
	{
		m_pRevert = _ATL_NEW CRevertToSelf;
		if(!m_pRevert)
		{
			::RevertToSelf();
			Clear();
			return false;
		}
	}
	return true;
}

inline bool CAccessToken::OpenRPCClientToken(
	_In_ RPC_BINDING_HANDLE BindingHandle,
	_In_ DWORD dwDesiredAccess,
	_In_ bool bImpersonate /* = false */,
	_In_ bool bOpenAsSelf /* = true */)
{
	CheckImpersonation();

	if(RPC_S_OK != ::RpcImpersonateClient(BindingHandle))
		return false;

	HANDLE hToken;
	if(!::OpenThreadToken(::GetCurrentThread(), dwDesiredAccess, bOpenAsSelf, &hToken))
		return false;

	Clear();
	m_hToken = hToken;

	if(!bImpersonate)
	{
		RPC_STATUS rpcStatus = ::RpcRevertToSelfEx(BindingHandle);
		if( rpcStatus != RPC_S_OK )
		{
			Clear();
			return false;
		}
	}
	else
	{
		m_pRevert = _ATL_NEW CRpcRevertToSelfEx(BindingHandle);
		if(!m_pRevert)
		{
			RPC_STATUS rpcStatus = ::RpcRevertToSelfEx(BindingHandle);
		    if( rpcStatus != RPC_S_OK )
		    {
                // If we continue, arbitrary user code will run in the wrong context. Too dangerous to risk
                _ATL_FATAL_SHUTDOWN;
		    }
		}
	}
	return true;
}

inline bool CAccessToken::ImpersonateLoggedOnUser() const
{
	CheckImpersonation();

	ATLASSUME(m_hToken);
	if(m_hToken && ::ImpersonateLoggedOnUser(m_hToken))
	{
		ATLASSERT(!m_pRevert);
		delete m_pRevert;
		m_pRevert = _ATL_NEW CRevertToSelf;
		if (!m_pRevert)
		{
			::RevertToSelf();
			return false;
		}
		return true;
	}
	return false;
}

inline bool CAccessToken::Impersonate(
	_In_opt_ HANDLE hThread /* = NULL */) const
{
	CheckImpersonation();

	ATLASSUME(m_hToken);
	if(m_hToken)
		return 0 != ::SetThreadToken(hThread ? &hThread : NULL, m_hToken);
	return false;
}

inline bool CAccessToken::Revert(
	_In_opt_ HANDLE hThread /* = NULL */) const noexcept
{
	if (hThread || !m_pRevert)
		return 0 != ::SetThreadToken(hThread ? &hThread : NULL, NULL);

	bool bRet = m_pRevert->Revert();
	delete m_pRevert;
	m_pRevert = NULL;
	return bRet;
}

inline bool CAccessToken::LoadUserProfile()
{
	ATLASSUME(m_hToken && !m_hProfile);
	if(!m_hToken || m_hProfile)
		return false;

	sid UserSid;
	PROFILEINFO Profile;

	if(!GetUser(&UserSid))
		return false;

	memset(&Profile, 0x00, sizeof(PROFILEINFO));
	Profile.dwSize = sizeof(PROFILEINFO);
	Profile.lpUserName = const_cast<LPTSTR>(UserSid.account_name());
	if(!::LoadUserProfile(m_hToken, &Profile))
		return false;

	m_hProfile = Profile.hProfile;

	return true;
}

inline HANDLE CAccessToken::GetProfile() const noexcept
{
	return m_hProfile;
}

inline bool CAccessToken::LogonUser(
	_In_z_ LPCTSTR pszUserName,
	_In_z_ LPCTSTR pszDomain,
	_In_z_ LPCTSTR pszPassword,
	_In_ DWORD dwLogonType /* = LOGON32_LOGON_INTERACTIVE */,
	_In_ DWORD dwLogonProvider /* = LOGON32_PROVIDER_DEFAULT */) noexcept
{
	Clear();

	return 0 != ::LogonUser(
		const_cast<LPTSTR>(pszUserName),
		const_cast<LPTSTR>(pszDomain),
		const_cast<LPTSTR>(pszPassword),
		dwLogonType, dwLogonProvider, &m_hToken);
}

inline bool CAccessToken::CreateProcessAsUser(
	_In_opt_z_ LPCTSTR pApplicationName,
	_In_opt_z_ LPTSTR pCommandLine,
	_In_ LPPROCESS_INFORMATION pProcessInformation,
	_In_ LPSTARTUPINFO pStartupInfo,
	_In_ DWORD dwCreationFlags /* = NORMAL_PRIORITY_CLASS */,
	_In_ bool bLoadProfile /* = false */,
	_In_opt_ const CSecurityAttributes *pProcessAttributes /* = NULL */,
	_In_opt_ const CSecurityAttributes *pThreadAttributes /* = NULL */,
	_In_ bool bInherit /* = false */,
	_In_opt_z_ LPCTSTR pCurrentDirectory /* = NULL */) noexcept
{
	ATLASSERT(pProcessInformation != NULL);
	ATLASSERT(pStartupInfo != NULL);

	if(!pProcessInformation || !pStartupInfo)
	{
		return false;
	}

	LPVOID pEnvironmentBlock;
	PROFILEINFO Profile;
	sid UserSid;
	DWORD dwLastError;

	HANDLE hToken = m_hToken;

	// Straighten out impersonation problems...
	TOKEN_TYPE TokenType = TokenPrimary;
	if(!GetType(&TokenType))
	{
		return false;
	}

	if (TokenType != TokenPrimary)
	{
		hToken = NULL;
		if (!::DuplicateTokenEx(m_hToken, TOKEN_QUERY|TOKEN_DUPLICATE|TOKEN_ASSIGN_PRIMARY, NULL,
			SecurityImpersonation, TokenPrimary, &hToken))
		{
			return false;
		}
	}

	// Profile
	if(bLoadProfile && !m_hProfile)
	{
		if(!GetUser(&UserSid))
		{
			if(TokenType != TokenPrimary)
				::CloseHandle(hToken);
			return false;
		}
		memset(&Profile, 0x00, sizeof(PROFILEINFO));
		Profile.dwSize = sizeof(PROFILEINFO);
		Profile.lpUserName = const_cast<LPTSTR>(UserSid.account_name());
		if(::LoadUserProfile(hToken, &Profile))
			m_hProfile = Profile.hProfile;
	}

	// Environment block
	if(!::CreateEnvironmentBlock(&pEnvironmentBlock, hToken, bInherit))
		return false;

	BOOL bRetVal = ::CreateProcessAsUser(
		hToken,
		pApplicationName,
		pCommandLine,
		const_cast<CSecurityAttributes *>(pProcessAttributes),
		const_cast<CSecurityAttributes *>(pThreadAttributes),
		bInherit,
		dwCreationFlags,
		pEnvironmentBlock,
		pCurrentDirectory,
		pStartupInfo,
		pProcessInformation);

	 dwLastError = ::GetLastError();

	if(TokenType != TokenPrimary)
		::CloseHandle(hToken);

	::DestroyEnvironmentBlock(pEnvironmentBlock);

	::SetLastError(dwLastError);
	return bRetVal != 0;
}

inline _Success_(return != false) bool CAccessToken::EnableDisablePrivileges(
	_In_ const CAtlArray<LPCTSTR> &rPrivileges,
	_In_ bool bEnable,
	_Inout_opt_ CTokenPrivileges *pPreviousState,
	_Out_opt_ bool* pbErrNotAllAssigned/*=NULL*/)
{
	CTokenPrivileges NewState;
	for(UINT i = 0; i < rPrivileges.GetCount(); i++)
		NewState.Add(rPrivileges[i], bEnable);
	return EnableDisablePrivileges(NewState, pPreviousState,pbErrNotAllAssigned);
}

inline bool CAccessToken::CheckImpersonation() const noexcept
{
	// You should not be impersonating at this point.  Use GetThreadToken
	// instead of the OpenXXXToken functions or call Revert before
	// calling Impersonate.
	HANDLE hToken=INVALID_HANDLE_VALUE;
	if(!::OpenThreadToken(::GetCurrentThread(), 0, false, &hToken) &&
		::GetLastError() != ERROR_NO_TOKEN)
	{
		ATLTRACE(atlTraceSecurity, 2, _T("Caution: replacing thread impersonation token.\n"));
		return true;
	}
	if(hToken!=INVALID_HANDLE_VALUE)
	{
	    ::CloseHandle(hToken);
	}
	return false;
}

ATLPREFAST_SUPPRESS(28193)
inline bool CAccessToken::RevertToLevel(
	_In_opt_ SECURITY_IMPERSONATION_LEVEL *pSil) const noexcept
{
	BOOL bRet = ::RevertToSelf();
	if(bRet && pSil != NULL)
	{
		bRet = ::ImpersonateSelf(*pSil);
	}
	return !!bRet;
}
ATLPREFAST_UNSUPPRESS()

inline void CAccessToken::Clear() noexcept
{
	if(m_hProfile)
	{
		ATLASSUME(m_hToken);
		if(m_hToken)
			::UnloadUserProfile(m_hToken, m_hProfile);
		m_hProfile = NULL;
	}

	if(m_hToken)
	{
		::CloseHandle(m_hToken);
		m_hToken = NULL;
	}
	delete m_pRevert;
	m_pRevert = NULL;
}

//*******************************************
// CAutoRevertImpersonation implementation

inline CAutoRevertImpersonation::CAutoRevertImpersonation(
	_In_ const CAccessToken* pAT) noexcept
{
	m_pAT = pAT;
}

inline CAutoRevertImpersonation::~CAutoRevertImpersonation() noexcept
{
	if (m_pAT != NULL)
	{
		m_pAT->Revert();
	}
}

inline void CAutoRevertImpersonation::Attach(
	_In_ const CAccessToken* pAT) noexcept
{
	ATLASSUME(m_pAT == NULL);
	m_pAT = pAT;
}

inline const CAccessToken* CAutoRevertImpersonation::Detach() noexcept
{
	const CAccessToken* pAT = m_pAT;
	m_pAT = NULL;
	return pAT;
}

inline const CAccessToken* CAutoRevertImpersonation::GetAccessToken() noexcept
{
	return m_pAT;
}

//*******************************************
// CPrivateObjectSecurityDesc implementation

inline CPrivateObjectSecurityDesc::CPrivateObjectSecurityDesc() noexcept :
	m_bPrivate(false),
	CSecurityDesc()
{
}

inline CPrivateObjectSecurityDesc::~CPrivateObjectSecurityDesc() noexcept
{
	Clear();
}

inline bool CPrivateObjectSecurityDesc::Create(
	_In_opt_ const CSecurityDesc *pParent,
	_In_opt_ const CSecurityDesc *pCreator,
	_In_ bool bIsDirectoryObject,
	_In_ const CAccessToken &Token,
	_In_ PGENERIC_MAPPING GenericMapping) noexcept
{
	Clear();

	const SECURITY_DESCRIPTOR *pSDParent = pParent ? pParent->GetPSECURITY_DESCRIPTOR() : NULL;
	const SECURITY_DESCRIPTOR *pSDCreator = pCreator ? pCreator->GetPSECURITY_DESCRIPTOR() : NULL;

	if(!::CreatePrivateObjectSecurity(
		const_cast<SECURITY_DESCRIPTOR *>(pSDParent),
		const_cast<SECURITY_DESCRIPTOR *>(pSDCreator),
		reinterpret_cast<PSECURITY_DESCRIPTOR *>(&m_pSecurityDescriptor),
		bIsDirectoryObject, Token.GetHandle(), GenericMapping))
	{
		return false;
	}

	m_bPrivate = true;
	return true;
}

#if(_WIN32_WINNT >= 0x0500)
inline bool CPrivateObjectSecurityDesc::Create(
	_In_opt_ const CSecurityDesc *pParent,
	_In_opt_ const CSecurityDesc *pCreator,
	_In_opt_ GUID *ObjectType,
	_In_ bool bIsContainerObject,
	_In_ ULONG AutoInheritFlags,
	_In_ const CAccessToken &Token,
	_In_ PGENERIC_MAPPING GenericMapping) noexcept
{
	Clear();

	const SECURITY_DESCRIPTOR *pSDParent = pParent ? pParent->GetPSECURITY_DESCRIPTOR() : NULL;
	const SECURITY_DESCRIPTOR *pSDCreator = pCreator ? pCreator->GetPSECURITY_DESCRIPTOR() : NULL;

	if(!::CreatePrivateObjectSecurityEx(
		const_cast<SECURITY_DESCRIPTOR *>(pSDParent),
		const_cast<SECURITY_DESCRIPTOR *>(pSDCreator),
		reinterpret_cast<PSECURITY_DESCRIPTOR *>(&m_pSecurityDescriptor),
		ObjectType, bIsContainerObject, AutoInheritFlags, Token.GetHandle(), GenericMapping))
	{
		return false;
	}

	m_bPrivate = true;
	return true;
}
#endif

ATLPREFAST_SUPPRESS(6102)
inline _Success_(return != false) bool CPrivateObjectSecurityDesc::Get(
	_In_ SECURITY_INFORMATION si,
	_Inout_ CSecurityDesc *pResult) const noexcept
{
	ATLASSERT(pResult);
	if(!pResult)
		return false;

	if(!m_bPrivate)
		return false;

	DWORD dwLength = 0;
	SECURITY_DESCRIPTOR *pSDResult = NULL;

	if(!::GetPrivateObjectSecurity(m_pSecurityDescriptor, si, pSDResult, dwLength, &dwLength) &&
		::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		return false;
	}

	USES_ATL_SAFE_ALLOCA;
	pSDResult = static_cast<SECURITY_DESCRIPTOR *>(_ATL_SAFE_ALLOCA(dwLength, _ATL_SAFE_ALLOCA_DEF_THRESHOLD));
	if (pSDResult == NULL)
	{
		return false;
	}

	if(!::GetPrivateObjectSecurity(m_pSecurityDescriptor, si, pSDResult, dwLength, &dwLength))
		return false;

	*pResult = *pSDResult;

	return true;
}
ATLPREFAST_UNSUPPRESS()

inline bool CPrivateObjectSecurityDesc::Set(
	_In_ SECURITY_INFORMATION si,
	_In_ const CSecurityDesc &Modification,
	_In_ PGENERIC_MAPPING GenericMapping,
	_In_ const CAccessToken &Token) noexcept
{
	if(!m_bPrivate)
		return false;

	const SECURITY_DESCRIPTOR *pSDModification = Modification.GetPSECURITY_DESCRIPTOR();

	return 0 != ::SetPrivateObjectSecurity(si,
		const_cast<SECURITY_DESCRIPTOR *>(pSDModification),
		reinterpret_cast<PSECURITY_DESCRIPTOR *>(&m_pSecurityDescriptor),
		GenericMapping, Token.GetHandle());
}

#if(_WIN32_WINNT >= 0x0500)
inline bool CPrivateObjectSecurityDesc::Set(
	_In_ SECURITY_INFORMATION si,
	_In_ const CSecurityDesc &Modification,
	_In_ ULONG AutoInheritFlags,
	_In_ PGENERIC_MAPPING GenericMapping,
	_In_ const CAccessToken &Token) noexcept
{
	if(!m_bPrivate)
		return false;

	const SECURITY_DESCRIPTOR *pSDModification = Modification.GetPSECURITY_DESCRIPTOR();

	return 0 != ::SetPrivateObjectSecurityEx(si,
		const_cast<SECURITY_DESCRIPTOR *>(pSDModification),
		reinterpret_cast<PSECURITY_DESCRIPTOR *>(&m_pSecurityDescriptor),
		AutoInheritFlags, GenericMapping, Token.GetHandle());
}

inline bool CPrivateObjectSecurityDesc::ConvertToAutoInherit(
	_In_opt_ const CSecurityDesc *pParent,
	_In_opt_ GUID *ObjectType,
	_In_ bool bIsDirectoryObject,
	_In_ PGENERIC_MAPPING GenericMapping) noexcept
{
	if(!m_bPrivate)
		return false;

	const SECURITY_DESCRIPTOR *pSDParent = pParent ? pParent->GetPSECURITY_DESCRIPTOR() : NULL;
	SECURITY_DESCRIPTOR *pSD;

	if(!::ConvertToAutoInheritPrivateObjectSecurity(
		const_cast<SECURITY_DESCRIPTOR *>(pSDParent),
		m_pSecurityDescriptor,
		reinterpret_cast<PSECURITY_DESCRIPTOR *>(&pSD),
		ObjectType, bIsDirectoryObject, GenericMapping))
	{
		return false;
	}

	Clear();
	m_bPrivate = true;
	m_pSecurityDescriptor = pSD;

	return true;
}

inline void CPrivateObjectSecurityDesc::Clear() noexcept
{
	if(m_bPrivate)
	{
		ATLVERIFY(::DestroyPrivateObjectSecurity(reinterpret_cast<PSECURITY_DESCRIPTOR *>(&m_pSecurityDescriptor)));
		m_bPrivate = false;
		m_pSecurityDescriptor = NULL;
	}
	else
		CSecurityDesc::Clear();
}

#endif // _WIN32_WINNT >= 0x500

//*******************************************
// Global functions

inline bool AtlGetSecurityDescriptor(
	_In_z_ LPCTSTR pszObjectName,
	_In_ SE_OBJECT_TYPE ObjectType,
	_Inout_ CSecurityDesc *pSecurityDescriptor,
	_In_ SECURITY_INFORMATION requestedInfo =
		OWNER_SECURITY_INFORMATION |
		GROUP_SECURITY_INFORMATION |
		DACL_SECURITY_INFORMATION |
		SACL_SECURITY_INFORMATION,
	_In_ bool bRequestNeededPrivileges = true)
{
	ATLASSERT(pSecurityDescriptor);
	if(!pSecurityDescriptor)
		return false;

	SECURITY_DESCRIPTOR *pSD;
	DWORD dwErr;

	CAccessToken at;
	CTokenPrivileges TokenPrivileges;

	if (!(requestedInfo & SACL_SECURITY_INFORMATION))
		bRequestNeededPrivileges = false;

	if (bRequestNeededPrivileges)
	{
		if(!at.OpenThreadToken(TOKEN_IMPERSONATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
					false, false, SecurityImpersonation) ||
				!at.EnablePrivilege(SE_SECURITY_NAME, &TokenPrivileges) ||
				!at.Impersonate())
			return false;
	}

#pragma warning(push)
#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	dwErr = ::GetNamedSecurityInfo(const_cast<LPTSTR>(pszObjectName), ObjectType, requestedInfo, NULL, NULL, NULL, NULL, (PSECURITY_DESCRIPTOR *) &pSD);
#pragma warning(pop)

	if (bRequestNeededPrivileges)
	{
		if( !at.EnableDisablePrivileges(TokenPrivileges) )
			return false;
		if( !at.Revert() )
			return false;
	}

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}

	*pSecurityDescriptor = *pSD;
	::LocalFree(pSD);
	return true;
}

inline bool AtlGetSecurityDescriptor(
	_In_ HANDLE hObject,
	_In_ SE_OBJECT_TYPE ObjectType,
	_Inout_ CSecurityDesc *pSecurityDescriptor,
	_In_ SECURITY_INFORMATION requestedInfo =
		OWNER_SECURITY_INFORMATION |
		GROUP_SECURITY_INFORMATION |
		DACL_SECURITY_INFORMATION |
		SACL_SECURITY_INFORMATION,
	_In_ bool bRequestNeededPrivileges = true)
{
	ATLASSERT(pSecurityDescriptor);
	if(!pSecurityDescriptor)
		return false;

	SECURITY_DESCRIPTOR *pSD;
	DWORD dwErr;

	CAccessToken at;
	CTokenPrivileges TokenPrivileges;

	if (!(requestedInfo & SACL_SECURITY_INFORMATION))
		bRequestNeededPrivileges = false;

	if (bRequestNeededPrivileges)
	{
		if(!at.OpenThreadToken(TOKEN_IMPERSONATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
					false, false, SecurityImpersonation) ||
				!at.EnablePrivilege(SE_SECURITY_NAME, &TokenPrivileges) ||
				!at.Impersonate())
			return false;
	}

#pragma warning(push)
#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	dwErr = ::GetSecurityInfo(hObject, ObjectType, requestedInfo, NULL, NULL, NULL, NULL, reinterpret_cast<PSECURITY_DESCRIPTOR *>(&pSD));
#pragma warning(pop)

	if (bRequestNeededPrivileges)
	{
		if( !at.EnableDisablePrivileges(TokenPrivileges) )
			return false;
		if( !at.Revert() )
			return false;
	}

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}

	*pSecurityDescriptor = *pSD;
	::LocalFree(pSD);
	return true;
}

inline bool AtlGetOwnerSid(
	_In_ HANDLE hObject,
	_In_ SE_OBJECT_TYPE ObjectType,
	_Inout_ sid *sid)
{
	ATLASSERT(hObject && sid);
	if(!hObject || !sid)
		return false;

	SID *pOwner;
	PSECURITY_DESCRIPTOR pSD;

#pragma warning(push)
#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetSecurityInfo(hObject, ObjectType, OWNER_SECURITY_INFORMATION, (PSID *) &pOwner, NULL, NULL, NULL, &pSD);
#pragma warning(pop)

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}

	*sid = *pOwner;
	::LocalFree(pSD);
	return true;
}

inline bool AtlSetOwnerSid(
	_In_ HANDLE hObject,
	_In_ SE_OBJECT_TYPE ObjectType,
	_In_ const sid &rSid)
{
	ATLASSERT(hObject && rSid.is_valid());
	if(!hObject || !rSid.is_valid())
		return false;

	DWORD dwErr = ::SetSecurityInfo(hObject, ObjectType, OWNER_SECURITY_INFORMATION,
		const_cast<SID *>(rSid.get_sid()), NULL, NULL, NULL);

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool AtlGetOwnerSid(
	_In_z_ LPCTSTR pszObjectName,
	_In_ SE_OBJECT_TYPE ObjectType,
	_Inout_ sid *sid)
{
	ATLASSERT(pszObjectName && sid);
	if(!pszObjectName || !sid)
		return false;

	SID *pOwner;
	PSECURITY_DESCRIPTOR pSD;
#pragma warning(push)
#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetNamedSecurityInfo(const_cast<LPTSTR>(pszObjectName), ObjectType,	OWNER_SECURITY_INFORMATION, reinterpret_cast<PSID *>(&pOwner), NULL, NULL, NULL, &pSD);
#pragma warning(pop)

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}

	*sid = *pOwner;
	::LocalFree(pSD);
	return true;
}

inline bool AtlSetOwnerSid(
	_In_z_ LPCTSTR pszObjectName,
	_In_ SE_OBJECT_TYPE ObjectType,
	_In_ const sid &rSid)
{
	ATLASSERT(pszObjectName && rSid.is_valid());
	if(!pszObjectName || !rSid.is_valid())
		return false;

	DWORD dwErr = ::SetNamedSecurityInfo(const_cast<LPTSTR>(pszObjectName), ObjectType,
		OWNER_SECURITY_INFORMATION, const_cast<SID *>(rSid.get_sid()), NULL, NULL, NULL);

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool AtlGetGroupSid(
	_In_ HANDLE hObject,
	_In_ SE_OBJECT_TYPE ObjectType,
	_Inout_ sid *sid)
{
	ATLASSERT(hObject && sid);
	if(!hObject || !sid)
		return false;

	SID *pGroup;
	PSECURITY_DESCRIPTOR pSD;
#pragma warning(push)
#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetSecurityInfo(hObject, ObjectType, GROUP_SECURITY_INFORMATION, NULL, reinterpret_cast<PSID *>(&pGroup), NULL, NULL, &pSD);
#pragma warning(pop)

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}

	*sid = *pGroup;
	::LocalFree(pSD);
	return true;
}

inline bool AtlSetGroupSid(
	_In_ HANDLE hObject,
	_In_ SE_OBJECT_TYPE ObjectType,
	_In_ const sid &rSid)
{
	ATLASSERT(hObject && rSid.is_valid());
	if(!hObject || !rSid.is_valid())
		return false;

	DWORD dwErr = ::SetSecurityInfo(hObject, ObjectType, GROUP_SECURITY_INFORMATION,
		NULL, const_cast<SID *>(rSid.get_sid()), NULL, NULL);

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool AtlGetGroupSid(
	_In_z_ LPCTSTR pszObjectName,
	_In_ SE_OBJECT_TYPE ObjectType,
	_Inout_ sid *sid)
{
	ATLASSERT(pszObjectName && sid);
	if(!pszObjectName || !sid)
		return false;

	SID *pGroup;
	PSECURITY_DESCRIPTOR pSD;
#pragma warning(push)
#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetNamedSecurityInfo(const_cast<LPTSTR>(pszObjectName),	ObjectType, GROUP_SECURITY_INFORMATION, NULL, reinterpret_cast<PSID *>(&pGroup), NULL, NULL, &pSD);
#pragma warning(pop)

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}

	*sid = *pGroup;
	::LocalFree(pSD);
	return true;
}

inline bool AtlSetGroupSid(
	_In_z_ LPCTSTR pszObjectName,
	_In_ SE_OBJECT_TYPE ObjectType,
	_In_ const sid &rSid)
{
	ATLASSERT(pszObjectName && rSid.is_valid());
	if(!pszObjectName || !rSid.is_valid())
		return false;

	DWORD dwErr = ::SetNamedSecurityInfo(const_cast<LPTSTR>(pszObjectName), ObjectType,
		GROUP_SECURITY_INFORMATION, NULL, const_cast<SID *>(rSid.get_sid()), NULL, NULL);

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool AtlGetDacl(
	_In_ HANDLE hObject,
	_In_ SE_OBJECT_TYPE ObjectType,
	_Inout_ CDacl *pDacl)
{
	ATLASSERT(hObject && pDacl);
	if(!hObject || !pDacl)
		return false;

	ACL *pAcl;
	PSECURITY_DESCRIPTOR pSD;

#pragma warning(push)
#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetSecurityInfo(hObject, ObjectType, DACL_SECURITY_INFORMATION, NULL, NULL, &pAcl, NULL, &pSD);
#pragma warning(pop)

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}

	if(pAcl)
		*pDacl = *pAcl;
	::LocalFree(pSD);

	return NULL != pAcl;
}

inline bool AtlSetDacl(
	_In_ HANDLE hObject,
	_In_ SE_OBJECT_TYPE ObjectType,
	_In_ const CDacl &rDacl,
	_In_ DWORD dwInheritanceFlowControl = 0)
{
	ATLASSERT(hObject);
	if(!hObject)
		return false;

	ATLASSERT(
		dwInheritanceFlowControl == 0 ||
		dwInheritanceFlowControl == PROTECTED_DACL_SECURITY_INFORMATION ||
		dwInheritanceFlowControl == UNPROTECTED_DACL_SECURITY_INFORMATION);

	DWORD dwErr = ::SetSecurityInfo(hObject, ObjectType,
		DACL_SECURITY_INFORMATION | dwInheritanceFlowControl,
		NULL, NULL, const_cast<ACL *>(rDacl.GetPACL()), NULL);

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool AtlGetDacl(
	_In_z_ LPCTSTR pszObjectName,
	_In_ SE_OBJECT_TYPE ObjectType,
	_Inout_ CDacl *pDacl)
{
	ATLASSERT(pszObjectName && pDacl);
	if(!pszObjectName || !pDacl)
		return false;

	ACL *pAcl;
	PSECURITY_DESCRIPTOR pSD;

#pragma warning(push)
#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetNamedSecurityInfo(const_cast<LPTSTR>(pszObjectName), ObjectType,	DACL_SECURITY_INFORMATION, NULL, NULL, &pAcl, NULL, &pSD);
#pragma warning(pop)

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}

	if(pAcl)
		*pDacl = *pAcl;
	::LocalFree(pSD);

	return NULL != pAcl;
}

inline bool AtlSetDacl(
	_In_z_ LPCTSTR pszObjectName,
	_In_ SE_OBJECT_TYPE ObjectType,
	_In_ const CDacl &rDacl,
	_In_ DWORD dwInheritanceFlowControl = 0)
{
	ATLASSERT(pszObjectName);
	if(!pszObjectName)
		return false;

	ATLASSERT(
		dwInheritanceFlowControl == 0 ||
		dwInheritanceFlowControl == PROTECTED_DACL_SECURITY_INFORMATION ||
		dwInheritanceFlowControl == UNPROTECTED_DACL_SECURITY_INFORMATION);

	DWORD dwErr = ::SetNamedSecurityInfo(const_cast<LPTSTR>(pszObjectName), ObjectType,
		DACL_SECURITY_INFORMATION | dwInheritanceFlowControl,
		NULL, NULL, const_cast<ACL *>(rDacl.GetPACL()), NULL);

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool AtlGetSacl(
	_In_ HANDLE hObject,
	_In_ SE_OBJECT_TYPE ObjectType,
	_Inout_ CSacl *pSacl,
	_In_ bool bRequestNeededPrivileges = true)
{
	ATLASSERT(hObject && pSacl);
	if(!hObject || !pSacl)
		return false;

	ACL *pAcl;
	PSECURITY_DESCRIPTOR pSD;
	CAccessToken at;
	CTokenPrivileges TokenPrivileges;

	if (bRequestNeededPrivileges)
	{
		if(!at.OpenThreadToken(TOKEN_IMPERSONATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
					false, false, SecurityImpersonation) ||
				!at.EnablePrivilege(SE_SECURITY_NAME, &TokenPrivileges) ||
				!at.Impersonate())
			return false;
	}

#pragma warning(push)
#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetSecurityInfo(hObject, ObjectType, SACL_SECURITY_INFORMATION, NULL, NULL, NULL, &pAcl, &pSD);
#pragma warning(pop)

	if (bRequestNeededPrivileges)
	{
        if( !at.EnableDisablePrivileges(TokenPrivileges) )
			return false;
		if( !at.Revert() )
			return false;
	}

	if(dwErr != ERROR_SUCCESS)
	{
		::SetLastError(dwErr);
		return false;
	}

	if(pAcl)
		*pSacl = *pAcl;
	::LocalFree(pSD);

	return NULL != pAcl;
}

inline bool AtlSetSacl(
	_In_ HANDLE hObject,
	_In_ SE_OBJECT_TYPE ObjectType,
	_In_ const CSacl &rSacl,
	_In_ DWORD dwInheritanceFlowControl = 0,
	_In_ bool bRequestNeededPrivileges = true)
{
	ATLASSERT(hObject);
	if (!hObject)
		return false;

	CAccessToken at;
	CTokenPrivileges TokenPrivileges;

	ATLASSERT(
		dwInheritanceFlowControl == 0 ||
		dwInheritanceFlowControl == PROTECTED_SACL_SECURITY_INFORMATION ||
		dwInheritanceFlowControl == UNPROTECTED_SACL_SECURITY_INFORMATION);

	if (bRequestNeededPrivileges)
	{
		if(!at.OpenThreadToken(TOKEN_IMPERSONATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
					false, false, SecurityImpersonation) ||
				!at.EnablePrivilege(SE_SECURITY_NAME, &TokenPrivileges) ||
				!at.Impersonate())
			return false;
	}

	DWORD dwErr = ::SetSecurityInfo(hObject, ObjectType,
		SACL_SECURITY_INFORMATION | dwInheritanceFlowControl,
		NULL, NULL, NULL, const_cast<ACL *>(rSacl.GetPACL()));

	if (bRequestNeededPrivileges)
	{
		if( !at.EnableDisablePrivileges(TokenPrivileges) )
			return false;
		if( !at.Revert() )
			return false;
	}

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

inline bool AtlGetSacl(
	_In_z_ LPCTSTR pszObjectName,
	_In_ SE_OBJECT_TYPE ObjectType,
	_Inout_ CSacl *pSacl,
	_In_ bool bRequestNeededPrivileges = true)
{
	ATLASSERT(pszObjectName && pSacl);
	if(!pszObjectName || !pSacl)
		return false;

	ACL *pAcl;
	PSECURITY_DESCRIPTOR pSD;
	CAccessToken at;
	CTokenPrivileges TokenPrivileges;

	if (bRequestNeededPrivileges)
	{
		if(!at.OpenThreadToken(TOKEN_IMPERSONATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
					false, false, SecurityImpersonation) ||
				!at.EnablePrivilege(SE_SECURITY_NAME, &TokenPrivileges) ||
				!at.Impersonate())
			return false;
	}

#pragma warning(push)
#pragma warning(disable: 6309 6387)
	/* psdk prefast noise 498088 */
	DWORD dwErr = ::GetNamedSecurityInfo(const_cast<LPTSTR>(pszObjectName), ObjectType,	SACL_SECURITY_INFORMATION, NULL, NULL, NULL, &pAcl, &pSD);
#pragma warning(pop)

	if (bRequestNeededPrivileges)
	{
		at.EnableDisablePrivileges(TokenPrivileges);
		at.Revert();
	}

	::SetLastError(dwErr);
	if(dwErr != ERROR_SUCCESS)
		return false;

	if(pAcl)
		*pSacl = *pAcl;
	::LocalFree(pSD);

	return NULL != pAcl;
}

inline bool AtlSetSacl(
	_In_z_ LPCTSTR pszObjectName,
	_In_ SE_OBJECT_TYPE ObjectType,
	_In_ const CSacl &rSacl,
	_In_ DWORD dwInheritanceFlowControl = 0,
	_In_ bool bRequestNeededPrivileges = true)
{
	ATLASSERT(pszObjectName);
	if (!pszObjectName)
		return false;

	CAccessToken at;
	CTokenPrivileges TokenPrivileges;

	ATLASSERT(
		dwInheritanceFlowControl == 0 ||
		dwInheritanceFlowControl == PROTECTED_SACL_SECURITY_INFORMATION ||
		dwInheritanceFlowControl == UNPROTECTED_SACL_SECURITY_INFORMATION);

	if(bRequestNeededPrivileges)
	{
		if (!at.OpenThreadToken(TOKEN_IMPERSONATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
					false, false, SecurityImpersonation) ||
				!at.EnablePrivilege(SE_SECURITY_NAME, &TokenPrivileges) ||
				!at.Impersonate())
			return false;
	}

	DWORD dwErr = ::SetNamedSecurityInfo(const_cast<LPTSTR>(pszObjectName), ObjectType,
		SACL_SECURITY_INFORMATION | dwInheritanceFlowControl,
		NULL, NULL, NULL, const_cast<ACL *>(rSacl.GetPACL()));

	if (bRequestNeededPrivileges)
	{
		at.EnableDisablePrivileges(TokenPrivileges);
		at.Revert();
	}

	::SetLastError(dwErr);
	return ERROR_SUCCESS == dwErr;
}

#pragma endregion

} // namespace ATL

#pragma warning(pop)

#pragma pack(pop)
