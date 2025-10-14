#include <Windows.h>
#include <stdio.h>

#define IOCTL_GET_MESSAGE CTL_CODE(0x8000, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define MAXMSG 256

DWORD WINAPI LoggerThread(LPVOID LpParam);

BOOL APIENTRY DllMain(HMODULE Hmodule, DWORD FdwReason, LPVOID LpREserved) {
	switch (FdwReason) {
	case DLL_PROCESS_ATTACH:
	{
		MessageBox(NULL, L"USERCALL DLL_PROCESS_ATTACH", L"INFO", MB_OK);

		HANDLE hThread = CreateThread(NULL, 0, LoggerThread, NULL, 0, NULL);
		if (hThread) {
			CloseHandle(hThread);
		}
		break;
	}
	case DLL_PROCESS_DETACH:
		FreeConsole();
		break;
	}

	return TRUE;
}


DWORD WINAPI LoggerThread(LPVOID LpParam) {
	
	HANDLE hDevice;
	char messageBuffer[MAXMSG];
	DWORD bytesReturned;

	if (AllocConsole()) {
		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		freopen_s(&fp, "CONIN$", "r", stdin);

		SetConsoleTitle(L"USERCALL LOGGER");
	}
	
	const wchar_t *deviceName = L"\\\\.\\hypercall";

	hDevice = CreateFile(deviceName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		printf("Error opening the device\n");
		return -1;
	}

	printf("\t \tSTARTING USERCALL LISTENER:\n\n");

	while (1) {
		memset(messageBuffer, 0, sizeof(messageBuffer));
		if (DeviceIoControl(hDevice, IOCTL_GET_MESSAGE, NULL, 0, messageBuffer, sizeof(messageBuffer), &bytesReturned, NULL)) {
			if (bytesReturned > 0 && messageBuffer[0] != '\0') {
				printf("- USERCALL : %s\n", messageBuffer);
				fflush(stdout);
			}
		}
		else {
			printf("Error reading Device\n");
			break;
		}
	}
	CloseHandle(hDevice);
}
