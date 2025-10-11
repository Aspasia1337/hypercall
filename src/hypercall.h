#pragma once
#include <ntddk.h>
#include <ntstrsafe.h>

#include "Queue/queue.h"
#include "Device/device.h"
#include "Features/modEnum.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);

VOID Unload(PDRIVER_OBJECT pDriverObject);
