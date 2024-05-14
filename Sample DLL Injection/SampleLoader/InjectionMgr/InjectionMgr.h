/*********************************************************
				Name:			InjectionMgr
				Author:			Jinhyeong,Choe (cshelldll)
				Date:			2024.04.17
				Description:	DLL Injection Manager.
**********************************************************/

#pragma once
#include <Windows.h>
#include <tchar.h>

static const TCHAR REG_INJECTION_DISPLAY_NAME[] = _T("SampleDLL");
static const BYTE REG_INJECTION_PROVIDER_ID[]	= { 0x3A, 0x24, 0x42, 0x66, 0xA8, 0x38, 0xA6, 0x4A, 0xBA, 0xA5, 0x2E, 0x0B, 0xD7, 0x1F, 0xDD, 0x83 };

static const TCHAR REG_PATH_WINSOCK2_CATALOG5[] = _T("SYSTEM\\CurrentControlSet\\Services\\WinSock2\\Parameters\\NameSpace_Catalog5");
static const TCHAR REG_PATH_WINSOCK2_CATALOG9[] = _T("SYSTEM\\CurrentControlSet\\Services\\WinSock2\\Parameters\\Protocol_Catalog9");

typedef enum
{
	INJECTION_TYPE_NULL,
	INJECTION_TYPE_WINSOCK2_RESET,
	INJECTION_TYPE_WINSOCK2_CATALOG5,
	INJECTION_TYPE_WINSOCK2_CATALOG9
}INJECTION_TYPE;

typedef struct
{
	TCHAR path[MAX_PATH];
	INJECTION_TYPE type;
}InjectionInfo;

typedef struct
{
	const TCHAR* DisplayString;
	DWORD Enabled;
	const TCHAR* LibraryPath;
	const BYTE* ProviderId;
	BYTE* ProviderInfo;
	DWORD StoresServiceClassInfo;
	DWORD SupportedNameSpace;
	DWORD Version;
}RegCatalogEntry64;

class InjectionMgr
{
private:
	inline void GetCatalogEntryName(TCHAR* buf, DWORD buf_count, DWORD num_catalog_entries64);

	BOOL ReadRegistryValue(HKEY key, const TCHAR* name, DWORD type, void* buf, DWORD size);
	BOOL ReadRegistryValue(HKEY key, const TCHAR* name, DWORD type, void** buf, DWORD* size);

	BOOL WriteRegistryValue(HKEY key, const TCHAR* name, DWORD type, const void* value, DWORD size);

	void InitCatalogEntry64(RegCatalogEntry64& catalog, const TCHAR* path);

private:
	BOOL CheckPreparedCatalog5(HKEY key, DWORD num_catalog_entries64);

	BOOL InjectCatalog5(const TCHAR* dll_path);
	BOOL InjectCatalog9(const CHAR* dll_path);

	void ResetCatalog5();
	void ResetCatalog9();

public:
	BOOL InjectDLL(INJECTION_TYPE type, const TCHAR* dll_path);
	void Reset();
};