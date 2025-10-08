#include "hypercall.h"
#include "utils.h"

VOID Unload(PDRIVER_OBJECT pDrivObject) {
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\DosDevices\\hypercall");

	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(pDrivObject->DeviceObject);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDrivObject, PUNICODE_STRING pRegPath) {
	UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\hypercall");
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\DosDevices\\hypercall");
	PDEVICE_OBJECT pDevObject;
	NTSTATUS status;

	UNREFERENCED_PARAMETER(pRegPath);

	status = IoCreateDevice(pDrivObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN,
		FALSE, &pDevObject);

	if (!NT_SUCCESS(status)) return status;

	status = IoCreateSymbolicLink(&symLink, &deviceName);

	if (!NT_SUCCESS(status)) {
		IoDeleteDevice(pDevObject);
		return status;
	}

	pDrivObject->MajorFunction[IRP_MJ_CREATE] = DeviceCreate;
	pDrivObject->MajorFunction[IRP_MJ_CLOSE] = DeviceClose;
	pDrivObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceController;
	pDrivObject->DriverUnload = Unload;

	SendMessage("*** Hypercall Started ***\n");
	return STATUS_SUCCESS;
}