#pragma once
#include "process_manager.h"
#include "import_inspector.h"
#include "../integrity/integrity_checker.h"
#include "logger.h"

class UtilityClass {
public:
    ProcessManager process;
    ImportInspector inspector;
    IntegrityChecker integrity;
};

inline UtilityClass m_Utility;