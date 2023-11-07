#include "utils.h"

BOOL FindProcessByName(CHAR* process_name, HANDLE* pid)
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
				// EPROCESS->UniqueProcessId
				*pid = *(PHANDLE)((uintptr_t)cur_entry + 0x440);

				return TRUE;
			}
		}

		// EPROCESS->ActiveProcessLinks
		PLIST_ENTRY list = (PLIST_ENTRY)((uintptr_t)(cur_entry)+0x448);
		cur_entry = (PEPROCESS)((uintptr_t)list->Flink - 0x448);

	} while (cur_entry != sys_process);

	return FALSE;
}