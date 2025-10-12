#pragma once

#include "../common.h"
#include "../Device/device.h"
#include "../Queue/queue.h"

#define MAXMODULES 2048

extern SYSTEM_MODULE_ENTRY gModuleList[MAXMODULES];

void moduleEnumeration(void);


