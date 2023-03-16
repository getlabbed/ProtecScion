/// @file debug.h
/// @brief Debugger/ErrorHandler header
///
/// This is a light debugger and error handler for any project
///

#ifndef DEBUG_H
#define DEBUG_H

typedef enum { None, Info, Warn, Error, Fatal } LogLevel_t;

typedef struct {
  LogLevel_t logLevel;
  void (*panicHandle)(LogLevel_t);
  void (*printHandle)(char *);
  unsigned long (*uptimeHandle)();
  void (*priorityTakeHandle)();
  void (*priorityGiveHandle)();
  void (*externPanicFatal)();
} Debugger_t;

Debugger_t initDebugger(LogLevel_t, unsigned long (*uptimeHandle)(),
                        void (*printHandle)(char *),
                        void (*priorityTakeHandle)(),
                        void (*priorityGiveHandle)(),
                        void (*externPanicFatal)());

/// @func debugLog(debugger_t, logLevel, char *)
/// @brief send a log
void debugLog(Debugger_t, LogLevel_t, char *);

char *_logMessage(LogLevel_t);
void _printLog(Debugger_t, char *);
unsigned long _timeStamp(Debugger_t);
void _priorityTake(Debugger_t);
void _priorityGive(Debugger_t);
void _panicFatal(Debugger_t);

#endif // !DEBUG
