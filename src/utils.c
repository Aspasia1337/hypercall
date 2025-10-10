#include "utils.h"

char gMessageBuffer[MAXMSG];
BOOLEAN gHasMessage = FALSE;
KSPIN_LOCK gMessageLock; 
BOOLEAN gLockInitialized = FALSE;  

void InitializeMessageSystem(void) {
    if (!gLockInitialized) {
        KeInitializeSpinLock(&gMessageLock);
        gLockInitialized = 1;
    }
}

void SendMessage(const char* message) {
    KIRQL oldIrql;

    if (!gLockInitialized) {
        InitializeMessageSystem();
    }

    KeAcquireSpinLock(&gMessageLock, &oldIrql);
    RtlStringCchCopyA(gMessageBuffer, MAXMSG, message);
    gHasMessage = 1;
    KeReleaseSpinLock(&gMessageLock, oldIrql);
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
    KIRQL oldIrql;

    UNREFERENCED_PARAMETER(pDevObject);

    if (ioctl == IOCTL_GET_MESSAGE) {
        PVOID buffer = pIrp->AssociatedIrp.SystemBuffer;
        ULONG outSize = stack->Parameters.DeviceIoControl.OutputBufferLength;

        if (outSize >= MAXMSG) {
            KeAcquireSpinLock(&gMessageLock, &oldIrql);

            if (gHasMessage) {
                RtlCopyMemory(buffer, gMessageBuffer, MAXMSG);
                bytesReturned = MAXMSG;
                gHasMessage = FALSE;
            }
            else {
                RtlZeroMemory(buffer, MAXMSG);
                bytesReturned = MAXMSG;
            }

            KeReleaseSpinLock(&gMessageLock, oldIrql);
        }
        else {
            status = STATUS_BUFFER_TOO_SMALL;
        }
    }
    else {
        status = STATUS_INVALID_DEVICE_REQUEST;
    }

    pIrp->IoStatus.Status = status;
    pIrp->IoStatus.Information = bytesReturned;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    return status;
}