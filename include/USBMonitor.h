#pragma once

#ifdef _WIN32
#include "win/winUSBMonitor.h"
#elif defined(__linux__)
#include "unix/unixUSBMonitor.h"
#endif

