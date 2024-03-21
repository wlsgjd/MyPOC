#pragma once
#include <stdio.h>
#include <Windows.h>
#include <Windns.h>
#include "ConsoleMgr/ConsoleMgr.h"

#pragma comment(lib, "DnsAPI.lib")

extern "C" int WINAPI DnsGetCacheDataTable(PDNS_RECORD*);

ConsoleMgr consoleMgr(TEXT("DnsQuery"));

// Main Error Code
#define MAIN_ERROR_SUCCESS			0x00000000
#define MAIN_ERROR_GET_DNS_CACHE	0xFFFFFFFF