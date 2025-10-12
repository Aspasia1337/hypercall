#include "modEnum.h"

SYSTEM_MODULE_ENTRY gModuleList[MAXMODULES] = { 0 };

void moduleEnumeration(void) {
	NTSTATUS status;
	ULONG size = 0;
	PVOID buffer = 0;

	status = ZwQuerySystemInformation(SystemModuleInformation, 0, 0, &size);
	
	// Trick for getting the size of the buffer

	if(status != STATUS_INFO_LENGTH_MISMATCH) {
		EnqueueMessage("Error getting the size of the buffer in ZwQuerySystemInformation\n");
		return;
	}

	buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, size, 'buff');

	status = ZwQuerySystemInformation(SystemModuleInformation, buffer, size, &size);

	if(!NT_SUCCESS(status)){
		EnqueueMessage("Error getting System Information from the ZwQuerySystemInformation call\n");
		ExFreePoolWithTag(buffer, 'buff');
		return;
	}

	PSYSTEM_MODULE_INFORMATION modInfo = (PSYSTEM_MODULE_INFORMATION)buffer;

	for(ULONG i = 0; i < modInfo->Count; i++) {
		gModuleList[i] = modInfo->Module[i];
		const char* modName = (const char*)((gModuleList[i]).FullPathName + (gModuleList[i]).OffsetToFileName);
		EnqueueMessage(modName);
	}

	ExFreePoolWithTag(buffer, 'buff');
}
