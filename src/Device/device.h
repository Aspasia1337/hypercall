#pragma once
#include <ntddk.h>
#include <ntstrsafe.h>

#include "../Queue/queue.h"

#define IOCTL_GET_MESSAGE CTL_CODE(0x8000, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

extern KSPIN_LOCK gMessageLock;       
extern BOOLEAN gLockInitialized;       

NTSTATUS DeviceCreate(PDEVICE_OBJECT pDevObject, PIRP pIrp);

NTSTATUS DeviceClose(PDEVICE_OBJECT pDevObject, PIRP pIrp);

NTSTATUS DeviceController(PDEVICE_OBJECT pDevObject, PIRP pIrp);