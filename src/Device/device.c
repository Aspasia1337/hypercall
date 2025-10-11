#include "device.h"

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
    ULONG ioctl = stack->Parameters.DeviceIoControl.IoControlCode;

    UNREFERENCED_PARAMETER(pDevObject);

    if (ioctl == IOCTL_GET_MESSAGE) {
        PVOID buffer = pIrp->AssociatedIrp.SystemBuffer;
        ULONG outSize = stack->Parameters.DeviceIoControl.OutputBufferLength;

        if (outSize >= MAXMSG) {
            if (!DequeueMessage(buffer, outSize)) {
                RtlZeroMemory(buffer, MAXMSG);
            }
            pIrp->IoStatus.Information = MAXMSG;
        }
        else {
            status = STATUS_BUFFER_TOO_SMALL;
            pIrp->IoStatus.Information = 0;
        }
    }
    else {
        status = STATUS_INVALID_DEVICE_REQUEST;
        pIrp->IoStatus.Information = 0;
    }

    pIrp->IoStatus.Status = status;
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);
    return status;
}


