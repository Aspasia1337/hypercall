#include <windows.h>
#include <stdio.h>

#define IOCTL_GET_MESSAGE CTL_CODE(0x8000, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define MAXMSG 256

int main() {
	HANDLE hDevice;
	DWORD bytesReturned;
	char message[MAXMSG];
	DWORD error;

	const wchar_t* name = L"\\\\.\\hypercall";

	hDevice = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if(hDevice == INVALID_HANDLE_VALUE) {
		printf("Error on CreateFile");
		CloseHandle(hDevice);
		return -1;
	}

	printf("\n\t UserCall listening! \n");
	while(1) {
		memset(message, 0, sizeof(message));
		if (DeviceIoControl(hDevice, IOCTL_GET_MESSAGE, NULL, 0, message, sizeof(message), &bytesReturned, NULL)) {
			if (!bytesReturned && message[0] != '\0')
				printf("[HYPERCALL] : %s\n", message);
		}
		else {
			error = GetLastError();
			printf("Error reading : %d", error);
			return -1;
		}
	}
}