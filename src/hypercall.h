#pragma once
#include <ntddk.h>
#include <ntstrsafe.h>

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);

VOID Unload(PDRIVER_OBJECT pDriverObject);
