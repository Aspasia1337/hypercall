#pragma warning (disable : 4100)

#include "hypercall.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	pDriverObject->DriverUnload = UnloadDriver;
	DbgPrintEx(0, 0, "hello from kernel!");
	return STATUS_SUCCESS;
}


NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "bye from kernel!");
	return STATUS_SUCCESS;
}