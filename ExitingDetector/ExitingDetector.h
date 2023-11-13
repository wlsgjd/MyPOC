#pragma once
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

#include "utils/utils.h"

using namespace std;

#ifdef _WIN64
typedef ULONG PVOID32;
typedef PVOID NATIVE_PVOID;
typedef SIZE_T SIZE_T64;
typedef SIZE_T64* PSIZE_T64;
#else
typedef PVOID PVOID32;
typedef UINT64 NATIVE_PVOID;
typedef UINT64 SIZE_T64;
typedef SIZE_T64* PSIZE_T64;
#endif

typedef struct _PROCESS_BASIC_INFORMATION32
{
	NTSTATUS ExitStatus;
	PVOID32 PebBaseAddress;
	ULONG AffinityMask;
	ULONG BasePriority;
	HANDLE UniqueProcessId;
	HANDLE InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION32, * PPROCESS_BASIC_INFORMATION32;

typedef struct _PROCESS_BASIC_INFORMATION64 {
	NTSTATUS ExitStatus;
	ULONG Pad1;
	PVOID64 PebBaseAddress;
	ULONGLONG AffinityMask;
	ULONGLONG BasePriority;
	PVOID64 UniqueProcessId;
	PVOID64 InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION64, * PPROCESS_BASIC_INFORMATION64;

#ifdef _WIN64
#define PROCESS_BASIC_INFORMATION PROCESS_BASIC_INFORMATION64
#define PPROCESS_BASIC_INFORMATION PPROCESS_BASIC_INFORMATION64
#else
#define PROCESS_BASIC_INFORMATION PROCESS_BASIC_INFORMATION32
#define PPROCESS_BASIC_INFORMATION PPROCESS_BASIC_INFORMATION32
#endif

typedef struct _PROCESS_EXTENDED_BASIC_INFORMATION
{
	SIZE_T Size;
	PROCESS_BASIC_INFORMATION BasicInfo;
	union
	{
		ULONG Flags;
		struct
		{
			ULONG IsProtectedProcess : 1;
			ULONG IsWow64Process : 1;
			ULONG IsProcessDeleting : 1;
			ULONG IsCrossSessionCreate : 1;
			ULONG IsFrozen : 1;
			ULONG IsBackground : 1;
			ULONG IsStronglyNamed : 1;
			ULONG IsSecureProcess : 1;
			ULONG IsSubsystemProcess : 1;
			ULONG SpareBits : 23;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} PROCESS_EXTENDED_BASIC_INFORMATION, * PPROCESS_EXTENDED_BASIC_INFORMATION;

typedef enum _PROCESSINFOCLASS {
	ProcessBasicInformation = 0,
	ProcessQuotaLimits = 1,
	ProcessIoCounters = 2,
	ProcessVmCounters = 3,
	ProcessTimes = 4,
	ProcessBasePriority = 5,
	ProcessRaisePriority = 6,
	ProcessDebugPort = 7,
	ProcessExceptionPort = 8,
	ProcessAccessToken = 9,
	ProcessLdtInformation = 10,
	ProcessLdtSize = 11,
	ProcessDefaultHardErrorMode = 12,
	ProcessIoPortHandlers = 13,   // Note: this is kernel mode only
	ProcessPooledUsageAndLimits = 14,
	ProcessWorkingSetWatch = 15,
	ProcessUserModeIOPL = 16,
	ProcessEnableAlignmentFaultFixup = 17,
	ProcessPriorityClass = 18,
	ProcessWx86Information = 19,
	ProcessHandleCount = 20,
	ProcessAffinityMask = 21,
	ProcessPriorityBoost = 22,
	ProcessDeviceMap = 23,
	ProcessSessionInformation = 24,
	ProcessForegroundInformation = 25,
	ProcessWow64Information = 26,
	ProcessImageFileName = 27,
	ProcessLUIDDeviceMapsEnabled = 28,
	ProcessBreakOnTermination = 29,
	ProcessDebugObjectHandle = 30,
	ProcessDebugFlags = 31,
	ProcessHandleTracing = 32,
	ProcessIoPriority = 33,
	ProcessExecuteFlags = 34,
	ProcessTlsInformation = 35,
	ProcessCookie = 36,
	ProcessImageInformation = 37,
	ProcessCycleTime = 38,
	ProcessPagePriority = 39,
	ProcessInstrumentationCallback = 40,
	ProcessThreadStackAllocation = 41,
	ProcessWorkingSetWatchEx = 42,
	ProcessImageFileNameWin32 = 43,
	ProcessImageFileMapping = 44,
	ProcessAffinityUpdateMode = 45,
	ProcessMemoryAllocationMode = 46,
	ProcessGroupInformation = 47,
	ProcessTokenVirtualizationEnabled = 48,
	ProcessOwnerInformation = 49,
	ProcessWindowInformation = 50,
	ProcessHandleInformation = 51,
	ProcessMitigationPolicy = 52,
	ProcessDynamicFunctionTableInformation = 53,
	ProcessHandleCheckingMode = 54,
	ProcessKeepAliveCount = 55,
	ProcessRevokeFileHandles = 56,
	ProcessWorkingSetControl = 57,
	ProcessHandleTable = 58,
	ProcessCheckStackExtentsMode = 59,
	ProcessCommandLineInformation = 60,
	ProcessProtectionInformation = 61,
	ProcessMemoryExhaustion = 62,
	ProcessFaultInformation = 63,
	ProcessTelemetryIdInformation = 64,
	ProcessCommitReleaseInformation = 65,
	ProcessReserved1Information = 66,
	ProcessReserved2Information = 67,
	ProcessSubsystemProcess = 68,
	ProcessInPrivate = 70,
	ProcessRaiseUMExceptionOnInvalidHandleClose = 71,
	MaxProcessInfoClass                             // MaxProcessInfoClass should always be the last enum

} PROCESSINFOCLASS;

typedef NTSTATUS(WINAPI* NtQueryInformationProcessT)
(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);