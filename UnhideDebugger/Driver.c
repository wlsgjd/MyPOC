/*********************************************************
				Name:			UnhideDebugger
				Author:			Jinhyeong,Choe (wlsgjd)
				Date:			2024.04.22
				Description:	특정 스레드에 대한 디버깅 보호를 해제합니다.
**********************************************************/

#include <ntdef.h>
#include <ntifs.h>
#include <ntimage.h>
#include <minwindef.h>
#include <stdlib.h>
#pragma comment(lib, "ntdll.lib")

typedef struct
{
	ULONG Terminated : 1;
	ULONG ThreadInserted : 1;
	ULONG HideFromDebugger : 1;
	ULONG ActiveImpersonationInfo : 1;
	ULONG HardErrorsAreDisabled : 1;
	ULONG BreakOnTermination : 1;
	ULONG SkipCreationMsg : 1;
	ULONG SkipTerminationMsg : 1;
	ULONG CopyTokenOnOpen : 1;
	ULONG ThreadIoPriority : 3;
	ULONG ThreadPagePriority : 3;
	ULONG RundownFail : 1;
	ULONG UmsForceQueueTermination : 1;
	ULONG IndirectCpuSets : 1;
	ULONG DisableDynamicCodeOptOut : 1;
	ULONG ExplicitCaseSensitivity : 1;
	ULONG PicoNotifyExit : 1;
	ULONG DbgWerUserReportActive : 1;
	ULONG ForcedSelfTrimActive : 1;
	ULONG SamplingCoverage : 1;
	ULONG ReservedCrossThreadFlags : 8;
}CrossThreadFlags;

BOOL FindProcessByName(CHAR* process_name, PEPROCESS* eprocess)
{
	PEPROCESS sys_process = PsInitialSystemProcess;
	PEPROCESS cur_entry = sys_process;

	CHAR image_name[15];

	do
	{
		// EPROCESS->ImageFileName
		RtlCopyMemory((PVOID)(&image_name), (PVOID)((uintptr_t)cur_entry + 0x5a8), sizeof(image_name));

		if (strstr(image_name, process_name))
		{
			DWORD active_threads; 
			RtlCopyMemory((PVOID)&active_threads, (PVOID)((uintptr_t)cur_entry + 0x5f0), sizeof(active_threads));

			// EPROCESS->ActiveThreads
			if (active_threads)
			{
				*eprocess = cur_entry;
				return TRUE;
			}
		}

		// EPROCESS->ActiveProcessLinks
		PLIST_ENTRY list = (PLIST_ENTRY)((uintptr_t)(cur_entry)+0x448);
		cur_entry = (PEPROCESS)((uintptr_t)list->Flink - 0x448);

	} while (cur_entry != sys_process);

	return FALSE;
}

BOOL UnhideFromDebugger(PEPROCESS eprocess)
{
	// EPROCESS->ThreadListHead
	PLIST_ENTRY thread_list_head = (PVOID)((uintptr_t)eprocess + 0x5e0); 
	PLIST_ENTRY cur_list = thread_list_head;

	DbgPrint("UnhideFromDebugger\n");

	do
	{
		// ETHREAD->ThreadListEntry
		PETHREAD ethread = (PETHREAD)((uintptr_t)cur_list - 0x538);

		// ETHREAD->CrossThreadFlags
		CrossThreadFlags* flags = (CrossThreadFlags*)((uintptr_t)ethread + 0x560);

		if (flags->HideFromDebugger)
		{
			DbgPrint("- 0x%p: TRUE -> FALSE\n", ethread);
			flags->HideFromDebugger = FALSE;
		}

		cur_list = cur_list->Blink;
	} while (cur_list != thread_list_head);

	return TRUE;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);
	
	PEPROCESS eprocess;
	if (FindProcessByName("notepad.exe", &eprocess))
	{
		UnhideFromDebugger(eprocess);
		ObDereferenceObject(eprocess);
	}
	
	return STATUS_UNSUCCESSFUL;
}