// https://gist.github.com/mattn/253013
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <comdef.h>
#include <iostream>

using namespace std;


DWORD getppid(DWORD pid)
{
    HANDLE hSnapshot;
    PROCESSENTRY32 pe32;
    DWORD ppid = 0;
    //DWORD pid = GetCurrentProcessId();
    //DWORD pid = 7892;

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    __try {
        if (hSnapshot == INVALID_HANDLE_VALUE) __leave;

        ZeroMemory(&pe32, sizeof(pe32));
        pe32.dwSize = sizeof(pe32);
        if (!Process32First(hSnapshot, &pe32)) __leave;

        do {
            if (pe32.th32ProcessID == pid) {
                ppid = pe32.th32ParentProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));

    }
    __finally {
        if (hSnapshot != INVALID_HANDLE_VALUE) CloseHandle(hSnapshot);
    }
    return ppid;
}


DWORD GetProcessIdByName(const char* processName)
{
    DWORD processId = 0;

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32 pe32 = { 0 };
    pe32.dwSize = sizeof(pe32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            // wchar to char
            _bstr_t b(pe32.szExeFile);
            //cout << pe32.szExeFile << endl;
            const char* c = b;

            if (strcmp(c, processName) == 0) {
                processId = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }

    CloseHandle(hSnapshot);

    return processId;
}

BOOL checkServicesEXE(DWORD currentPid) {
    
    DWORD servicesPid = GetProcessIdByName("services.exe");

    cout << "services.exe PID: " << servicesPid << endl;

    //DWORD currentPid = 5784;

    printf("Start: %d", currentPid);
    while (currentPid != servicesPid) {
        currentPid = getppid(currentPid);
        printf(" -> %d", currentPid);
        if (currentPid == 0) {
            printf("\nThis Process is NOT created from services.exe!\n");
            return FALSE;
        }
    }

    printf("\nThis Process is created from services.exe!!!!!!!!!!!!!!\n");
    return TRUE;
}





int main() {
    //printf("%lx\n", getppid());
    //printf("%d\n", getppid(460));
    
    
    DWORD currentPid = 2540;
    
    //checkServicesEXE(currentPid);
    //checkServicesEXE(argv[1]);


    cout << (checkServicesEXE(currentPid) == 1 ? "Yes" : "No") << endl;

    return 0;

}
