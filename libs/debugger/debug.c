/// @file debug.c
/// @brief Debugger/ErrorHandler header
///
/// This is a light debugger and error handler for any project
///

#include "debug.h"
#include <stdio.h>
#include <stdlib.h>

/// @func Debugger_t initDebugger(LogLevel_t, void (*)(LogLevel_t))
/// @brief Inits a debugger and returns it
///
/// @param logLevel The minimum log level to be logged (max 3)
/// @param uptimeHandle Function pointer to a uptime timer (For timestamps)
/// @param printHandle Function pointer to a logging method
/// @param priorityTakeHandle Function pointer to a priority manager (opt)
/// @param priorityGiveHandle Function pointer to a priority manager (opt)
/// @param externPanicFatal Function pointer to a panic handler
///
/// @return The debugger struct descriptor
Debugger_t initDebugger(LogLevel_t logLevel, unsigned long (*uptimeHandle)(),
                        void (*printHandle)(char *),
                        void (*priorityTakeHandle)(),
                        void (*priorityGiveHandle)(),
                        void (*externPanicFatal)()) {
  Debugger_t debugger;
  debugger.logLevel = (logLevel >= 3) ? 3 : logLevel;
  debugger.uptimeHandle = uptimeHandle;
  debugger.printHandle = printHandle;
  debugger.priorityTakeHandle = priorityTakeHandle;
  debugger.priorityGiveHandle = priorityGiveHandle;
  debugger.externPanicFatal = externPanicFatal;

  debugLog(debugger, Info, "Debugger started");

  return debugger;
}

/// @func void debugLog(Debugger_t, LogLevel_t, char*)
/// @brief It logs a debug message using a debugger depending on a log level
///
/// @param debugger The debugger descriptor
/// @param logLevel The log level of the message
/// @param message The message to log
void debugLog(Debugger_t debugger, LogLevel_t logLevel, char *message) {
  _priorityTake(debugger);
  if (debugger.logLevel <= logLevel) {
    char msg[1000];
    sprintf(msg, "<%lu> [%s] : %s", _timeStamp(debugger), _logMessage(logLevel),
            message);
    _printLog(debugger, msg);
  }

  if (logLevel == Fatal)
    _panicFatal(debugger);

  _priorityGive(debugger);
}

/// @func char *_logMessage(LogLevel_t)
/// @brief Gets the log level message
///
/// @param logLevel The log level
///
/// @return The log level message
char *_logMessage(LogLevel_t logLevel) {
  if (logLevel == Info)
    return "INFO";
  if (logLevel == Warn)
    return "WARN";
  if (logLevel == Error)
    return "ERROR";
  if (logLevel == Fatal)
    return "FATAL";
  // Default
  return "WARN !UNSUPPORTED LOG LEVEL!";
}

/// @func unsigned long _timeStamp(Debugger_t)
/// @brief Wrapper for uptimeHandle
///
/// @param debugger The debugger descriptor
///
/// @return The uptime
unsigned long _timeStamp(Debugger_t debugger) {
  if (!debugger.uptimeHandle)
    return 0;
  return debugger.uptimeHandle();
}

/// @func void _printLog(Debugger_t, char *)
/// @brief Wrapper for the printHandle
///
/// @param debugger The debugger descriptor
/// @param message The message to log
void _printLog(Debugger_t debugger, char *message) {
  if (!debugger.printHandle)
    return;
  debugger.printHandle(message);
}

/// @func void _priorityTake(Debugger_t)
/// @brief Wrapper for priorityTakeHandle
///
/// @param debugger The debugger descriptor
void _priorityTake(Debugger_t debugger) {
  if (!debugger.priorityTakeHandle)
    return;
  debugger.priorityTakeHandle();
}

/// @func void _priorityGive(Debugger_t)
/// @brief Wrapper for priorityGiveHandle
///
/// @param debugger The debugger descriptor
void _priorityGive(Debugger_t debugger) {
  if (!debugger.priorityGiveHandle)
    return;
  debugger.priorityGiveHandle();
}

/// @func void _panicFatal(Debugger_t)
/// @brief Wrapper for the panic handler
///
/// @param debugger The debugger descriptor
void _panicFatal(Debugger_t debugger) {
  if (debugger.externPanicFatal)
    debugger.externPanicFatal();
  while (1) {
    // noop
  }
}
