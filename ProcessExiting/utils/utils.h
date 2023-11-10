#pragma once
#include <ntdef.h>
#include <ntifs.h>
#include <ntimage.h>
#include <minwindef.h>
#pragma comment(lib, "ntdll.lib")

extern POBJECT_TYPE* IoDriverObjectType;

NTSTATUS
ObReferenceObjectByName(
	IN PUNICODE_STRING ObjectName,
	IN ULONG Attributes,
	IN PACCESS_STATE PassedAccessState OPTIONAL,
	IN ACCESS_MASK DesiredAccess OPTIONAL,
	IN POBJECT_TYPE ObjectType,
	IN KPROCESSOR_MODE AccessMode,
	IN OUT PVOID ParseContext OPTIONAL,
	OUT PVOID* Object
);

PIMAGE_NT_HEADERS RtlImageNtHeader(_In_ PVOID Bas);

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY HashLinks;
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct EprocessFlags
{
	ULONG CreateReported : 1;
	ULONG NoDebugInherit : 1;
	ULONG ProcessExiting : 1;
	ULONG ProcessDelete : 1;
	ULONG ManageExecutableMemoryWrites : 1;
	ULONG VmDeleted : 1;
	ULONG OutswapEnabled : 1;
	ULONG Outswapped : 1;
	ULONG FailFastOnCommitFail : 1;
	ULONG Wow64VaSpace4Gb : 1;
	ULONG AddressSpaceInitialized : 2;
	ULONG SetTimerResolution : 1;
	ULONG BreakOnTermination : 1;
	ULONG DeprioritizeViews : 1;
	ULONG WriteWatch : 1;
	ULONG ProcessInSession : 1;
	ULONG OverrideAddressSpace : 1;
	ULONG HasAddressSpace : 1;
	ULONG LaunchPrefetched : 1;
	ULONG Background : 1;
	ULONG VmTopDown : 1;
	ULONG ImageNotifyDone : 1;
	ULONG PdeUpdateNeeded : 1;
	ULONG VdmAllowed : 1;
	ULONG ProcessRundown : 1;
	ULONG ProcessInserted : 1;
	ULONG DefaultIoPriority : 3;
	ULONG ProcessSelfDelete : 1;
	ULONG SetTimerResolutionLink : 1;
}EprocessFlags, * PEprocessFlags;

#define IfReverseFlag(p, b) if(p != b) { p = !p; }

BOOL FindProcessByName(CHAR* process_name, PEPROCESS* eprocess);
HANDLE GetProcessId(PEPROCESS eprocess);
PDRIVER_OBJECT GetDriverObject(WCHAR* DeviceName);
void* GetKernelBaseAddr();
__forceinline wchar_t locase_w(wchar_t c);
int _strcmpi_w(const wchar_t* s1, const wchar_t* s2);
PVOID HideMmSearchMemory(PVOID Buffer, SIZE_T SizeOfBuffer, PVOID Signature, SIZE_T SizeOfSignature);
unsigned __int64 ExpLookupHandleTableEntry(unsigned int* a1, __int64 a2);