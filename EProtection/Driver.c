#include "utils/utils.h"

void Protect(PEPROCESS eprocess, BOOL bActive)
{
	/*
		0: kd> dt_eprocess
		ntdll!_EPROCESS
		   +0x000 Pcb              : _KPROCESS
		   +0x438 ProcessLock      : _EX_PUSH_LOCK
		   +0x440 UniqueProcessId  : Ptr64 Void
		   +0x448 ActiveProcessLinks : _LIST_ENTRY
		   +0x458 RundownProtect   : _EX_RUNDOWN_REF
		   +0x460 Flags2           : Uint4B

		   ...
		   +0x87a Protection       : _PS_PROTECTION
	*/

	BYTE* protection = (BYTE*)eprocess + 0x87A;
	*protection = bActive ? 0x61 : 0x00;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	HANDLE ProcessId;
	if (FindProcessByName("notepad.exe", &ProcessId))
	{
		PEPROCESS eprocess;
		if (NT_SUCCESS(PsLookupProcessByProcessId(ProcessId, &eprocess)))
		{
			Protect(eprocess, TRUE);
			ObDereferenceObject(eprocess);
		}
	}
	return STATUS_UNSUCCESSFUL;
}