#pragma once

#ifdef _WIN32
#include "win/winUSBMonitor.h"
#elif defined(__linux__)
#include "unix/unixUSBMonitor.h"
#elif defined(__APPLE__) && defined(__MACH__)
#include "mac/macUSBMonitor.h"
#else
#error "Unsupported platform"
#endif

