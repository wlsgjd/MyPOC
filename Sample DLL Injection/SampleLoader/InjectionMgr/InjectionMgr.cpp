#include "InjectionMgr.h"

inline void InjectionMgr::GetCatalogEntryName(TCHAR* buf, DWORD buf_count, DWORD num_catalog_entries64)
{
    _stprintf_s(buf, buf_count, _T("%012d"), num_catalog_entries64);
}

BOOL InjectionMgr::ReadRegistryValue(HKEY key, const TCHAR* name, DWORD type, void* buf, DWORD size)
{
    DWORD dwType = type;
    DWORD dwSize = 0;

    if (ERROR_SUCCESS != RegQueryValueEx(key, name, NULL, &dwType, NULL, &dwSize))
    {
        return FALSE;
    }

    if (dwSize > size)
    {
        return FALSE;
    }

    if (ERROR_SUCCESS != RegQueryValueEx(key, name, NULL, &dwType, (BYTE*)buf, &dwSize))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL InjectionMgr::ReadRegistryValue(HKEY key, const TCHAR* name, DWORD type, void** buf, DWORD* size)
{
    DWORD dwType = type;
    DWORD dwSize = 0;

    if (ERROR_SUCCESS != RegQueryValueEx(key, name, NULL, &dwType, NULL, &dwSize))
    {
        return FALSE;
    }

    BYTE* temp_buf = (BYTE*)malloc(dwSize);
    if (NULL == temp_buf)
    {
        return FALSE;
    }

    if (ERROR_SUCCESS != RegQueryValueEx(key, name, NULL, &dwType, temp_buf, &dwSize))
    {
        free(temp_buf);
        return FALSE;
    }

    *buf = temp_buf;
    *size = dwSize;
    return TRUE;
}

BOOL InjectionMgr::WriteRegistryValue(HKEY key, const TCHAR* name, DWORD type, const void* value, DWORD size)
{
    if (ERROR_SUCCESS != RegSetValueEx(key, name, NULL, type, (const BYTE*)value, size))
    {
        return FALSE;
    }
    
    return TRUE;
}

void InjectionMgr::InitCatalogEntry64(RegCatalogEntry64& catalog, const TCHAR* path)
{
    catalog.DisplayString = REG_INJECTION_DISPLAY_NAME;
    catalog.Enabled = TRUE;
    catalog.LibraryPath = path;
    catalog.ProviderId = REG_INJECTION_PROVIDER_ID;
    catalog.ProviderInfo = NULL;
    catalog.StoresServiceClassInfo = 0;
    catalog.SupportedNameSpace = 15;
    catalog.Version = 0;
}

BOOL InjectionMgr::CheckPreparedCatalog5(HKEY key, DWORD num_catalog_entries64)
{
    BOOL b_prepared = FALSE;

    TCHAR key_name[16];
    GetCatalogEntryName(key_name, _countof(key_name), num_catalog_entries64);

    HKEY hkey_catalog_entry64;
    if (!RegOpenKey(key, key_name, &hkey_catalog_entry64))
    {
        TCHAR* display_name;
        DWORD size;
        if (ReadRegistryValue(hkey_catalog_entry64, _T("DisplayString"), REG_SZ, (void**)&display_name, &size))
        {
            if (_tcsncmp(display_name, REG_INJECTION_DISPLAY_NAME, _countof(REG_INJECTION_DISPLAY_NAME)) == 0)
            {
                b_prepared = TRUE;
            }
            free(display_name);
        }

        RegCloseKey(hkey_catalog_entry64);
    }

    return b_prepared;
}

BOOL InjectionMgr::InjectCatalog5(const TCHAR* dll_path)
{
    BOOL b_injected = FALSE;

    HKEY hkey_namespace_catalog5;
    if (!RegOpenKey(HKEY_LOCAL_MACHINE, REG_PATH_WINSOCK2_CATALOG5, &hkey_namespace_catalog5))
    {
        DWORD num_catalog_entries64;
        if (ReadRegistryValue(hkey_namespace_catalog5, _T("Num_Catalog_Entries64"), REG_DWORD, &num_catalog_entries64, sizeof(num_catalog_entries64)))
        {
            HKEY hkey_catalog_entries64;
            if (!RegOpenKey(hkey_namespace_catalog5, _T("Catalog_Entries64"), &hkey_catalog_entries64))
            {
                if (!CheckPreparedCatalog5(hkey_catalog_entries64, num_catalog_entries64))
                {
                    // 중복된게 없으면 새로운 인젝션~~
                    TCHAR key_name[16];
                    GetCatalogEntryName(key_name, _countof(key_name), ++num_catalog_entries64);

                    HKEY key_inject_entry64;
                    if (!RegCreateKey(hkey_catalog_entries64, key_name, &key_inject_entry64))
                    {
                        RegCatalogEntry64 entry64;
                        InitCatalogEntry64(entry64, dll_path);

                        // 기본적인 인젝션 정보들!!
                        WriteRegistryValue(key_inject_entry64, _T("DisplayString"), REG_SZ, entry64.DisplayString, sizeof(REG_INJECTION_DISPLAY_NAME));
                        WriteRegistryValue(key_inject_entry64, _T("Enabled"), REG_DWORD, &entry64.Enabled, sizeof(entry64.Enabled));
                        WriteRegistryValue(key_inject_entry64, _T("LibraryPath"), REG_SZ, entry64.LibraryPath, ((_tcslen(dll_path) + 1) * sizeof(TCHAR)));
                        WriteRegistryValue(key_inject_entry64, _T("ProviderId"), REG_BINARY, entry64.ProviderId, sizeof(REG_INJECTION_PROVIDER_ID));
                        WriteRegistryValue(key_inject_entry64, _T("ProviderInfo"), REG_BINARY, entry64.ProviderInfo, 0);
                        WriteRegistryValue(key_inject_entry64, _T("StoresServiceClassinfo"), REG_DWORD, &entry64.StoresServiceClassInfo, sizeof(entry64.StoresServiceClassInfo));
                        WriteRegistryValue(key_inject_entry64, _T("SupportedNameSpace"), REG_DWORD, &entry64.SupportedNameSpace, sizeof(entry64.SupportedNameSpace));
                        WriteRegistryValue(key_inject_entry64, _T("Version"), REG_DWORD, &entry64.Version, sizeof(entry64.Version));

                        // num_catalog_entries64을 업데이트 해주면 DLL 실행~
                        WriteRegistryValue(hkey_namespace_catalog5, _T("Num_Catalog_Entries64"), REG_DWORD, &num_catalog_entries64, sizeof(num_catalog_entries64));

                        // 인젝션 완료.
                        b_injected = TRUE;

                        RegCloseKey(key_inject_entry64);
                    }
                }
                else
                {
                    // 이미 인젝션이 완료된 경우.
                    b_injected = TRUE;
                }

                RegCloseKey(hkey_catalog_entries64);
            }
        }

        RegCloseKey(hkey_namespace_catalog5);
    }

    return b_injected;
}

BOOL InjectionMgr::InjectCatalog9(const CHAR* dll_path)
{
    BOOL b_injected = FALSE;

    HKEY hkey_protocol_catalog9;
    if (!RegOpenKey(HKEY_LOCAL_MACHINE, REG_PATH_WINSOCK2_CATALOG9, &hkey_protocol_catalog9))
    {
        DWORD num_catalog_entries64;
        if (ReadRegistryValue(hkey_protocol_catalog9, _T("Num_Catalog_Entries64"), REG_DWORD, &num_catalog_entries64, sizeof(num_catalog_entries64)))
        {
            // 1개 이상의 엔트리가 존재하면~~
            if (num_catalog_entries64 > 0)
            {
                HKEY hkey_catalog_entries64;
                if (!RegOpenKey(hkey_protocol_catalog9, _T("Catalog_Entries64"), &hkey_catalog_entries64))
                {
                    // 첫번째 데이터에 덮어쓰기~
                    TCHAR key_name[16];
                    GetCatalogEntryName(key_name, _countof(key_name), 1);

                    HKEY hkey_inject_entry64;
                    if (!RegOpenKey(hkey_catalog_entries64, key_name, &hkey_inject_entry64))
                    {
                        BYTE* packed_catalog_item;
                        DWORD size;
                        if (ReadRegistryValue(hkey_inject_entry64, _T("PackedCatalogItem"), REG_BINARY, (void**)&packed_catalog_item, &size))
                        {
                            if (memcpy_s(packed_catalog_item, size, dll_path, (strlen(dll_path) + 1)) == 0)
                            {
                                b_injected = WriteRegistryValue(hkey_inject_entry64, _T("PackedCatalogItem"), REG_BINARY, packed_catalog_item, size);
                            }

                            free(packed_catalog_item);
                        }
                        RegCloseKey(hkey_inject_entry64);
                    }

                    RegCloseKey(hkey_catalog_entries64);
                }
            }
        }

        RegCloseKey(hkey_protocol_catalog9);
    }

    return b_injected;
}

void InjectionMgr::ResetCatalog5()
{
    HKEY hkey_namespace_catalog5;
    if (!RegOpenKey(HKEY_LOCAL_MACHINE, REG_PATH_WINSOCK2_CATALOG5, &hkey_namespace_catalog5))
    {
        DWORD num_catalog_entries64;
        if (ReadRegistryValue(hkey_namespace_catalog5, _T("Num_Catalog_Entries64"), REG_DWORD, &num_catalog_entries64, sizeof(num_catalog_entries64)))
        {
            HKEY hkey_catalog_entries64;
            if (!RegOpenKey(hkey_namespace_catalog5, _T("Catalog_Entries64"), &hkey_catalog_entries64))
            {
                if (CheckPreparedCatalog5(hkey_catalog_entries64, num_catalog_entries64))
                {
                    // 인젝션 정보가 존재하면 제거~
                    TCHAR key_name[16];
                    GetCatalogEntryName(key_name, _countof(key_name), num_catalog_entries64--);

                    // 혹시 모를 오류가 발생할수도 있으니... num_catalog_entries64부터 제거!
                    if (WriteRegistryValue(hkey_namespace_catalog5, _T("Num_Catalog_Entries64"), REG_DWORD, &num_catalog_entries64, sizeof(num_catalog_entries64)))
                    {
                        // 키 제거는 이후에.. ㅜㅡㅜ
                        RegDeleteKey(hkey_catalog_entries64, key_name);
                    }
                }

                RegCloseKey(hkey_catalog_entries64);
            }
        }

        RegCloseKey(hkey_namespace_catalog5);
    }
}

void InjectionMgr::ResetCatalog9()
{
    // 콘솔창 팝업되면 보기 안좋으니깐 ...조용히 실행...
    system("start /b netsh winsock reset");
}

BOOL InjectionMgr::InjectDLL(INJECTION_TYPE type, const TCHAR* dll_path)
{
    if (type == INJECTION_TYPE_WINSOCK2_CATALOG5)
    {
        return InjectCatalog5(dll_path);
    }
    else if (type == INJECTION_TYPE_WINSOCK2_CATALOG9)
    {
        // PackedCatalogItem의 파일 경로가 ASCII로 저장돼서 변환해야함 ㅠㅠ...
        CHAR path[MAX_PATH];
        if (!WideCharToMultiByte(CP_ACP, 0, dll_path, -1, path, sizeof(path), NULL, NULL))
        {
            return FALSE;
        }

        return InjectCatalog9(path);
    }

    return FALSE;
}

void InjectionMgr::Reset()
{
    // 실행 시 관리자 권한 필요~~
    ResetCatalog5();
    ResetCatalog9();
}
