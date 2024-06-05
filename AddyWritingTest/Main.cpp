#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD GetProcessID(const wchar_t* processName)
{
    DWORD processID = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(processEntry);
        if (Process32First(snapshot, &processEntry))
        {
            do
            {
                if (wcscmp(processEntry.szExeFile, processName) == 0)
                {
                    processID = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }
    return processID;
}

bool WriteToMemory(DWORD processID, LPVOID address, int newValue)
{
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (hProcess == NULL)
    {
        std::cerr << "Failed to open process.\n";
        return false;
    }

    if (!WriteProcessMemory(hProcess, address, &newValue, sizeof(newValue), NULL))
    {
        std::cerr << "Failed to write memory.\n";
        CloseHandle(hProcess);
        return false;
    }

    std::cout << "Memory written successfully.\n";
    CloseHandle(hProcess);
    return true;
}

int main()
{
    const wchar_t* processName = L"Starthis.exe";
    DWORD processID = GetProcessID(processName);

    if (processID == 0)
    {
        std::cerr << "Process not found.\n";
        return 1;
    }

    LPVOID address = (LPVOID)0xDEADBEEF;
    int newValue = 1234;

    if (WriteToMemory(processID, address, newValue))
    {
        std::cout << "Value changed successfully.\n";
    }
    else
    {
        std::cerr << "Failed to change value.\n";
    }

    return 0;
}