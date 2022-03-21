#include "mtfcore.hpp"
#include "windows.h"

HWND hwnd;

BOOL WINAPI Hook_WriteProcessMemory(HANDLE hProcess, LPVOID lpBaseAddress, LPCVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesWritten)
{
	Suspend_HOOK();
	if (!IsWindow(hwnd))
		hwnd = FindWindow(NULL, L"MsgMonitor");
	if (IsWindow(hwnd))
	{
		COPYDATASTRUCT data;
		data.dwData = GetProcessId(hProcess);
		data.cbData = nSize + 8;

		byte* datas = (byte*)malloc(nSize + 8);

		if (datas)
		{
			memcpy(datas, lpBuffer, nSize);

			*(long long*)(&datas[nSize]) = (long long)lpBaseAddress;

			data.lpData = (PVOID)datas;
			SendMessage(hwnd, WM_COPYDATA, NULL, (LPARAM)&data);

			free(datas);
		}
		
		/*long long* buffer = (long long*)((long long)lpBuffer + nSize);
		*buffer = (long long)lpBaseAddress;*/
		/*data.lpData = (PVOID)lpBuffer;*/

		BOOL result = WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
		Recovery_HOOK();
		return result;
	}
	else
		return WriteProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		hwnd = FindWindow(NULL, L"MsgMonitor");
		if (hwnd)Install_HOOK(WriteProcessMemory, Hook_WriteProcessMemory);
	}
    return TRUE;
}

