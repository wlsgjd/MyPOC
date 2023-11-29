#pragma once
#include <ntdef.h>
#include <ntifs.h>
#include <ntimage.h>
#include <minwindef.h>

typedef enum _OB_OPEN_REASON
{
	ObCreateHandle,
	ObOpenHandle,
	ObDuplicateHandle,
	ObInheritHandle,
	ObMaxOpenReason
} OB_OPEN_REASON;

typedef struct _OBJECT_TYPE_INITIALIZER
{
	WORD Length;
	UCHAR ObjectTypeFlags;
	ULONG CaseInsensitive : 1;
	ULONG UnnamedObjectsOnly : 1;
	ULONG UseDefaultObject : 1;
	ULONG SecurityRequired : 1;
	ULONG MaintainHandleCount : 1;
	ULONG MaintainTypeList : 1;
	ULONG ObjectTypeCode;
	ULONG InvalidAttributes;
	GENERIC_MAPPING GenericMapping;
	ULONG ValidAccessMask;
	POOL_TYPE PoolType;
	ULONG DefaultPagedPoolCharge;
	ULONG DefaultNonPagedPoolCharge;
	PVOID DumpProcedure;
	PVOID OpenProcedure;
	PVOID CloseProcedure;
	PVOID DeleteProcedure;
	LONG * ParseProcedure;
	LONG * SecurityProcedure;
	LONG * QueryNameProcedure;
	UCHAR * OkayToCloseProcedure;
	ULONG WaitObjectFlagMask;
	USHORT WaitObjectFlagOffset;
	USHORT WaitObjectPointerOffset;
} OBJECT_TYPE_INITIALIZER, *POBJECT_TYPE_INITIALIZER;

typedef struct _OBJECT_TYPE
{
	LIST_ENTRY					TypeList;
	UNICODE_STRING				Name;
	PVOID						DefaultObject;
	UCHAR						Index;
	ULONG						TotalNumberOfObjects;
	ULONG						TotalNumberOfHandles;
	ULONG						HighWaterNumberOfObjects;
	ULONG						HighWaterNumberOfHandles;
	OBJECT_TYPE_INITIALIZER		TypeInfo;
	EX_PUSH_LOCK				TypeLock;
	ULONG						Key;
	LIST_ENTRY					CallbackList;
} OBJECT_TYPE, *POBJECT_TYPE;

NTSTATUS typedef (*OB_OPEN_METHOD)(
	IN OB_OPEN_REASON OpenReason,
	IN BYTE AccessMode,
	IN PEPROCESS Process,
	IN PVOID Object,
	IN ACCESS_MASK GrantedAccess,
	IN ULONG HandleCount
	);