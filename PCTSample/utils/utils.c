#include "utils.h"

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

HANDLE GetProcessId(PEPROCESS eprocess)
{
	// EPROCESS->UniqueProcessId
	return *(PHANDLE)((uintptr_t)eprocess + 0x440);
}

PDRIVER_OBJECT GetDriverObject(WCHAR* DeviceName)
{
	UNICODE_STRING uniDriverName;
	PVOID  pDriverObj;
	NTSTATUS status;

	RtlInitUnicodeString(&uniDriverName, DeviceName);
	status = ObReferenceObjectByName(
		&uniDriverName,    // IN PUNICODE_STRING ObjectName
		OBJ_CASE_INSENSITIVE,  // IN ULONG Attributes
		NULL,      // IN PACCESS_STATE PassedAccessState OPTIONAL, 
		(ACCESS_MASK)0L,
		*IoDriverObjectType,
		KernelMode,
		NULL,
		&pDriverObj
	);

	if (NT_SUCCESS(status))
	{
		ObDereferenceObject(pDriverObj);
		return (PDRIVER_OBJECT)pDriverObj;
	}

	return (PDRIVER_OBJECT)0;
}

void* GetKernelBaseAddr()
{
	PDRIVER_OBJECT driver_object = GetDriverObject(L"\\Driver\\disk");

	if (driver_object)
		driver_object = driver_object->DeviceObject->DriverObject;
	else
		return 0;

	PLDR_DATA_TABLE_ENTRY entry = (PLDR_DATA_TABLE_ENTRY)driver_object->DriverSection;
	PLDR_DATA_TABLE_ENTRY first = entry;
	while ((PLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink != first)
	{
		if (_strcmpi_w(entry->BaseDllName.Buffer, L"ntoskrnl.exe") == 0)
		{
			return entry->DllBase;
		}
		entry = (PLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink;
	}
	return 0;
}

__forceinline wchar_t locase_w(wchar_t c)
{
	if ((c >= 'A') && (c <= 'Z'))
		return c + 0x20;
	else
		return c;
}

int _strcmpi_w(const wchar_t* s1, const wchar_t* s2)
{
	wchar_t c1, c2;

	if (s1 == s2)
		return 0;

	if (s1 == 0)
		return -1;

	if (s2 == 0)
		return 1;

	do {
		c1 = locase_w(*s1);
		c2 = locase_w(*s2);
		s1++;
		s2++;
	} while ((c1 != 0) && (c1 == c2));

	return (int)(c1 - c2);
}

PVOID HideMmSearchMemory(PVOID Buffer, SIZE_T SizeOfBuffer, PVOID Signature, SIZE_T SizeOfSignature)
{
	// Sanity check...
	//
	if (SizeOfSignature > SizeOfBuffer)
	{
		return NULL;
	}

	PCHAR Memory = (PCHAR)Buffer;
	for (size_t i = 0; i < (SizeOfBuffer - SizeOfSignature); ++i)
	{

		if (!memcmp(&Memory[i], Signature, SizeOfSignature))
		{
			return &Memory[i];
		}

	}

	return NULL;
}

unsigned __int64 ExpLookupHandleTableEntry(unsigned int* a1, __int64 a2)
{
	unsigned __int64 v2; // rdx
	__int64 v3; // r8
	__int64 v4; // rax
	__int64 v5; // rax

	v2 = a2 & 0xFFFFFFFFFFFFFFFCui64;
	if (v2 >= *a1)
		return 0i64;
	v3 = *((ULONG64*)a1 + 1);
	v4 = *((ULONG64*)a1 + 1) & 3i64;
	if ((ULONG)v4 == 1)
	{
		v5 = *(ULONG64*)(v3 + 8 * (v2 >> 10) - 1);
		return v5 + 4 * (v2 & 0x3FF);
	}
	if ((ULONG)v4)
	{
		v5 = *(ULONG64*)(*(ULONG64
			*)(v3 + 8 * (v2 >> 19) - 2) + 8 * ((v2 >> 10) & 0x1FF));
		return v5 + 4 * (v2 & 0x3FF);
	}
	return v3 + 4 * v2;
}
