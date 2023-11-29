#pragma once
#include "utils/utils.h"
#include "mystruct.h"

#define HOOK_TABLE_SIG		(ULONG64)0x123456789
#define HOOK_FUNC_SIZE		0x1000
#define THIS_PTR(p)			p->this_ptr

#define GET_PROCESS_ID(p)	(*(ULONG64*)((BYTE*)p + 0x440))				// EPROCESS->UniqueProcessId

typedef struct _MY_HOOK_TABLE MY_HOOK_TABLE, * PMY_HOOK_TABLE;
typedef struct _MY_HOOK_TABLE
{
	MY_HOOK_TABLE*	this_ptr;
	HANDLE			dwm_pid;
	OB_OPEN_METHOD	old_OpenProcedure;
} MY_HOOK_TABLE, * PMY_HOOK_TABLE;

static NTSTATUS MyPspProcessOpen(OB_OPEN_REASON OpenReason, BYTE AccessMode, PEPROCESS Process, PVOID Object, ACCESS_MASK GrantedAccess, ULONG HandleCount)
{
	PMY_HOOK_TABLE hook_table = HOOK_TABLE_SIG;

	if (Process != Object)
	{
		if (OpenReason != ObCreateHandle)
		{
			if (GET_PROCESS_ID(Object) == THIS_PTR(hook_table)->dwm_pid)
			{
				return STATUS_UNSUCCESSFUL;
			}
		}
	}

	// Disabled cfg(Control Flow Guard)
	return THIS_PTR(hook_table)->old_OpenProcedure(OpenReason, AccessMode, Process, Object, GrantedAccess, HandleCount);
};

PVOID SearchHookSignature64(PVOID addr, ULONG64 sig, SIZE_T size)
{
	SIZE_T max_size = size - sizeof(ULONG64);

	for (int i = 0; i <= max_size; ++i)
	{
		ULONG64* addr64 = (BYTE*)addr + i;
		if (*addr64 == sig)
			return addr64;
	}

	return 0;
};

int PatchHookTableAddress64(PVOID HookFunc, ULONG64 sig, PMY_HOOK_TABLE HookTable, SIZE_T Size)
{
	int result = 0;

	while (1)
	{
		PULONG64 my_sig = SearchHookSignature64(HookFunc, HOOK_TABLE_SIG, Size);

		if (my_sig)
		{
			*my_sig = HookTable;
			result++;
		}
		else
		{
			break;
		}

	}
	return result;
};

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPath);

	PEPROCESS eprocess;
	if (FindProcessByName("notepad.exe", &eprocess))
	{
		POBJECT_TYPE processType = *PsProcessType;

		PMY_HOOK_TABLE hook_table = ExAllocatePool(NonPagedPool, sizeof(MY_HOOK_TABLE));
		hook_table->this_ptr = hook_table;
		hook_table->old_OpenProcedure = processType->TypeInfo.OpenProcedure;
		hook_table->dwm_pid = GetProcessId(eprocess);

		DbgPrint("Target ProcessId : %d\n", hook_table->dwm_pid);

		OB_OPEN_METHOD fpMyPspProcessOpen = ExAllocatePool(NonPagedPool, HOOK_FUNC_SIZE);
		RtlCopyMemory(fpMyPspProcessOpen, MyPspProcessOpen, HOOK_FUNC_SIZE);
		if (PatchHookTableAddress64(fpMyPspProcessOpen, HOOK_TABLE_SIG, hook_table, HOOK_FUNC_SIZE) > 0)
		{
			processType->TypeInfo.OpenProcedure = fpMyPspProcessOpen;

			DbgPrint("ProcessOpenProcedure : 0x%p => 0x%p\n",
				hook_table->old_OpenProcedure,
				processType->TypeInfo.OpenProcedure);
		}
		else
		{
			DbgPrint("Failed PatchHookTableAddress64\n");
		}
	}
	else
	{
		DbgPrint("Process Not Found.\n");
	}

	return STATUS_UNSUCCESSFUL;
}