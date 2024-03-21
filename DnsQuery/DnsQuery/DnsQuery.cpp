#include "DnsQuery.h"

int _tmain(int argc, TCHAR* argv[])
{
    PDNS_RECORD record_entries = NULL;

    if (!DnsGetCacheDataTable(&record_entries))
        return MAIN_ERROR_GET_DNS_CACHE;

    for (PDNS_RECORD record = record_entries; record != NULL; record = record->pNext)
    {
        _tprintf(TEXT("%s\n"), record->pName);
    }

    DnsRecordListFree(record_entries, DnsFreeRecordList);
    return MAIN_ERROR_SUCCESS;
}