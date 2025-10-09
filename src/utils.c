#include "utils.h"

char gMessageBuffer[MAXMSG];
BOOLEAN gHasMessage;

void SendMessage(const char* message) {
	RtlStringCchCopyA(gMessageBuffer, MAXMSG, message);
	gHasMessage = 1;
}

NTSTATUS DeviceCreate(PDEVICE_OBJECT pDevObject, PIRP pIrp) {
	UNREFERENCED_PARAMETER(pDevObject);

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DeviceClose(PDEVICE_OBJECT pDevObject, PIRP pIrp) {
	UNREFERENCED_PARAMETER(pDevObject);

	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DeviceController(PDEVICE_OBJECT pDevObject, PIRP pIrp) {
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG bytesReturned = 0;
	ULONG ioctl = stack->Parameters.DeviceIoControl.IoControlCode;

	UNREFERENCED_PARAMETER(pDevObject);

	if (ioctl == IOCTL_GET_MESSAGE) {
		PVOID buffer = pIrp->AssociatedIrp.SystemBuffer;
		ULONG outSize = stack->Parameters.DeviceIoControl.OutputBufferLength;

		if (gHasMessage && outSize >= MAXMSG) {
			RtlCopyMemory(buffer, gMessageBuffer, MAXMSG);
			bytesReturned = MAXMSG;
			gHasMessage = 0;
		}
	}

	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = bytesReturned;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;
}