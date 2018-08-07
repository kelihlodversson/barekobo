// Simple logging macros that wrap circle's logging functions
#include <circle/logger.h>

#define DEBUG(...) CLogger::Get()->Write(__FILE__, LogDebug, __VA_ARGS__)
#define INFO(...)  CLogger::Get()->Write(__FILE__, LogNotice, __VA_ARGS__)
#define WARN(...)  CLogger::Get()->Write(__FILE__, LogWarning, __VA_ARGS__)
#define ERROR(...) CLogger::Get()->Write(__FILE__, LogError, __VA_ARGS__)
#define PANIC(...) CLogger::Get()->Write(__FILE__, LogPanic, __VA_ARGS__)
