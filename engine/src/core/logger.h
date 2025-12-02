#pragma once
#include "defines.h"

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

// Desativas o debug e trece em builds de release, talves eu tire depois
#if KRELEASE == 1
#define LOG_DEGUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#endif

typedef enum log_level {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_INFO = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
} log_level;

b8 initialize_logging();
void shutdown_logging();

KAPI void log_output(log_level level, const char* message, ...);

// Logs de erros "fatais"
#define KFATAL(message,...) log_output(LOG_LEVEL_NONE, message, ##__VA_ARGS__);

// Logs de erros
#ifndef KERROR
#define KERROR(message,...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);
#endif

// Logs de avisos
#if LOG_WARN_ENABLED == 1
#define KWARN(message,...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__);
#else
#define KWARN(message,...)
#endif

// Logs de informações
#if LOG_INFO_ENABLED == 1
#define KINFO(message,...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
#define KINFO(message,...)
#endif

// Logs de debug
#if LOG_DEBUG_ENABLED == 1
#define KDEBUG(message,...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
#define KDEBUG(message,...)
#endif

// Logs de trace
#if LOG_TRACE_ENABLED == 1
#define KTRACE(message,...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
#define KTRACE(message,...)
#endif
