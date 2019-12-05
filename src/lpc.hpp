#pragma once

#include <dumbnose/native.hpp>


namespace Nt {
	extern "C" {


//
// Data Structures for LPC
//

//
// Uniquely identifies a thread on a system
//
typedef struct _CLIENT_ID {
	DWORD ProcessId;
	DWORD ThreadId;
}CLIENT_ID,*PCLIENT_ID;

//
// Header for all messages sent through a port
//
typedef struct _PORT_MESSAGE {
	USHORT DataSize;
	USHORT MessageSize;
	USHORT MessageType;
	USHORT VirtualRangesOffset;
    CLIENT_ID ClientId;
	ULONG MessageId;
	ULONG SectionSize;
}PORT_MESSAGE, *PPORT_MESSAGE;

//
// 
//
typedef struct _PORT_SECTION_WRITE {
	ULONG Length;
	HANDLE SectionHandle;
	ULONG SectionOffset;
	ULONG ViewSize;
	PVOID ViewBase;
	PVOID TargetViewBase;
} PORT_SECTION_WRITE, *PPORT_SECTION_WRITE;

//
//
//
typedef struct _PORT_SECTION_READ {
	ULONG Length;
	ULONG ViewSize;
	ULONG ViewBase;
}PORT_SECTION_READ,*PPORT_SECTION_READ;

typedef struct LpcSectionMapInfo
{
	DWORD Length;
	DWORD SectionSize;
	DWORD ServerBaseAddress;
} LPCSECTIONMAPINFO, *PLPCSECTIONMAPINFO;

typedef enum _LPC_TYPE {
	LPC_NEW_MESSAGE,
	LPC_REQUEST,
	LPC_REPLY,
	LPC_DATAGRAM,
	LPC_LOST_REPLY,
	LPC_PORT_CLOSED,
	LPC_CLIENT_DIED,
	LPC_EXCEPTION,
	LPC_DEBUG_EVENT,
	LPC_ERROR_EVENT,
	LPC_CONNECTION_REQUEST,
	LPC_CONNECTION_REFUSED,
  LPC_MAXIMUM
} LPC_TYPE;

//
//
//
typedef NTSYSAPI NTSTATUS 
(NTAPI *NtCreatePortFunc)(OUT PHANDLE PortHandle, IN POBJECT_ATTRIBUTES ObjectAttributes, 
			 IN LONG MaxDataSize, IN LONG MaxMessageSize, IN ULONG Reserved);
extern NtCreatePortFunc NtCreatePort;

typedef NTSYSAPI NTSTATUS 
(NTAPI *NtAcceptConnectPortFunc)(PHANDLE PortHandle,ULONG port_id,PPORT_MESSAGE pLpcMessage,
					BOOLEAN accept,PPORT_SECTION_WRITE psw, PPORT_SECTION_READ psr);
extern NtAcceptConnectPortFunc NtAcceptConnectPort;

typedef NTSYSAPI NTSTATUS 
(NTAPI *NtConnectPortFunc)(PHANDLE PortHandle,PUNICODE_STRING PortName,PSECURITY_QUALITY_OF_SERVICE qos,
			  PPORT_SECTION_WRITE psw,PPORT_SECTION_READ psr,
			  PULONG MaxMessageSize,PVOID ConnectData,PULONG ConnectDataLength);
extern NtConnectPortFunc NtConnectPort;

typedef NTSYSAPI NTSTATUS
(NTAPI *NtCompleteConnectPortFunc)(HANDLE PortHandle);
extern NtCompleteConnectPortFunc NtCompleteConnectPort;

typedef NTSYSAPI NTSTATUS 
(NTAPI *NtRequestWaitReplyPortFunc)(HANDLE PortHandle,PPORT_MESSAGE pLpcMessageIn,
					   PPORT_MESSAGE pLpcMessageOut);
extern NtRequestWaitReplyPortFunc NtRequestWaitReplyPort;

typedef NTSYSAPI NTSTATUS 
(NTAPI *NtListenPortFunc)(HANDLE PortHandle,PPORT_MESSAGE pLpcMessage);
extern NtListenPortFunc NtListenPort;

typedef NTSYSAPI NTSTATUS 
(NTAPI *NtRequestPortFunc)(HANDLE PortHandle,PPORT_MESSAGE pLpcMessage);
extern NtRequestPortFunc NtRequestPort;

typedef NTSYSAPI NTSTATUS 
(NTAPI *NtReplyPortFunc)(HANDLE PortHandle,PPORT_MESSAGE pLpcMessage);
extern NtReplyPortFunc NtReplyPort;

typedef NTSYSAPI NTSTATUS 
(NTAPI *NtRegisterThreadTerminatePortFunc)(HANDLE PortHandle);
extern NtRegisterThreadTerminatePortFunc NtRegisterThreadTerminatePort;

typedef NTSYSAPI NTSTATUS 
(NTAPI *NtReplyWaitReceivePortFunc)(IN HANDLE  PortHandle, OUT PULONG  PortIdentifier  OPTIONAL,
					   IN PPORT_MESSAGE  ReplyMessage  OPTIONAL, OUT PPORT_MESSAGE  Message);
extern NtReplyWaitReceivePortFunc NtReplyWaitReceivePort;

	} // extern "C"

//
// Helper template
//
template<int len> struct PORT_MESSAGEX : Nt::PORT_MESSAGE {
	UCHAR Data[len];
};


} // namespace NT

