#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <vector>
#include <stdlib.h>
#include <sstream>

using namespace std;

//get the modulebase address
DWORD GetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
    DWORD dwModuleBaseAddress = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); // make snapshot of all modules within process
    MODULEENTRY32 ModuleEntry32 = { 0 };
    ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
    {
        do {
            if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) // if Found Module matches Module we look for -> done!
            {
                dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32


    }
    CloseHandle(hSnapshot);
    return dwModuleBaseAddress;
}

//get the pointer addresss
DWORD GetPointerAddress(HWND hwnd, DWORD gameBaseAddr, DWORD address, vector<DWORD> offsets)
{
    DWORD pID = NULL; // Game process ID
    GetWindowThreadProcessId(hwnd, &pID);
    HANDLE phandle = NULL;
    phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
    if (phandle == INVALID_HANDLE_VALUE || phandle == NULL);

    DWORD offset_null = NULL;
    ReadProcessMemory(phandle, (LPVOID*)(gameBaseAddr + address), &offset_null, sizeof(offset_null), 0);
    DWORD pointeraddress = offset_null; // the address we need
    for (int i = 0; i < offsets.size() - 1; i++) // we dont want to change the last offset value so we do -1
    {
        ReadProcessMemory(phandle, (LPVOID*)(pointeraddress + offsets.at(i)), &pointeraddress, sizeof(pointeraddress), 0);
    }
    return pointeraddress += offsets.at(offsets.size() - 1); // adding the last offset
}


//write memory function
void WriteMemo(HANDLE aphandle, DWORD AammoPtrAddr, string Aanswer)
{
    WriteProcessMemory(aphandle, (void*)(AammoPtrAddr), Aanswer.c_str(), Aanswer.length() + 10, 0);
}

//Windows with no scroll bars
void remove_scrollbar()
{
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(handle, &info);
    COORD new_size =
    {
        info.srWindow.Right - info.srWindow.Left + 1,
        info.srWindow.Bottom - info.srWindow.Top + 1
    };
    SetConsoleScreenBufferSize(handle, new_size);
}

int main()
{

    //console colors
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 3);

    //set console title
    SetConsoleTitle("Disconected C++");

    //set console rectangule
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    MoveWindow(console, r.left, r.top, 300, 150, TRUE);
    //no scrollbar func
    remove_scrollbar();


    //////////////////////////////////////////////////////////////////////////////////////

    cout << "Gw Disconecter v1.0, c++ edition" << endl;

    HWND hwnd_AC = FindWindowA(NULL, "GodsWar Online [USA] - Atlanta");
    if (hwnd_AC != FALSE)
    {
        DWORD pID = NULL;
        GetWindowThreadProcessId(hwnd_AC, &pID);
        HANDLE phandle = NULL;
        phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
        if (phandle == INVALID_HANDLE_VALUE || phandle == NULL);

        char gamemodule1[] = "GodsWar.exe";
        DWORD gamebaseaddress1 = GetModuleBaseAddress(_T(gamemodule1), pID);

        //ammo
        DWORD ammoAddr = 0x01176244;
        vector<DWORD> ammoOffsets{ 0x50, 0x33C, 0x0, 0x40, 0x4 };
        DWORD ammoPtrAddr = GetPointerAddress(hwnd_AC, gamebaseaddress1, ammoAddr, ammoOffsets);

        string answer;
        int i = 0;
        do
        {
            cout << "Victims name: ";
            getline(cin, answer);
            WriteMemo(phandle, ammoPtrAddr, answer);


        } while (i < 20);
    }
}
