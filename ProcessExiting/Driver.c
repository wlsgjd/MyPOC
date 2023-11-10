#include "utils/utils.h"

void SetProcessExiting(PEPROCESS eprocess, BOOL bExiting)
{
	EprocessFlags*	Flags = (EprocessFlags*)((BYTE*)eprocess + 0x464);
	EprocessFlags	org_flags = *Flags;

	if (bExiting)
	{
		Flags->ProcessExiting		= TRUE;
		Flags->ProcessDelete		= TRUE;
		Flags->ProcessInSession		= FALSE;
		Flags->ProcessRundown		= TRUE;
		Flags->ProcessSelfDelete	= TRUE;
	}
	else
	{
		IfReverseFlag(Flags->ProcessExiting,	FALSE);
		IfReverseFlag(Flags->ProcessDelete,		FALSE);
		IfReverseFlag(Flags->ProcessInSession,	TRUE);
		IfReverseFlag(Flags->ProcessRundown,	FALSE);
		IfReverseFlag(Flags->ProcessSelfDelete, FALSE);
	}
		
	DbgPrint("flags : 0x%08X => 0x%08X\n", org_flags, *Flags);
}
	
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPat)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPat);

	PEPROCESS eprocess;
	HANDLE pid;
	if (FindProcessByName("notepad.exe", &eprocess))
	{
		pid = GetProcessId(eprocess);

		DbgPrint("PID : %d\n", pid);
		DbgPrint("EPROCESS : 0x%p\n", eprocess);

		SetProcessExiting(eprocess, TRUE);
	}
	return STATUS_UNSUCCESSFUL;
}