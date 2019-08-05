#ifndef KON_LOG_H
#define KON_LOG_H

extern int ENABLE_DEBUG;

#if 0
#define KON_VERBOSE(msg, ...) _kon_log->verbose(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KON_INFO(msg, ...)    _kon_log->info(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KON_TRACE(msg, ...)   _kon_log->trace(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KON_WARN(msg, ...)    _kon_log->warn(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KON_ERROR(msg, ...)   _kon_log->error(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#endif

// use __FUNCTION__ to print c method
#if 0
#define KON_VERBOSE(msg, ...) _kon_log->verbose(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KON_INFO(msg, ...)    _kon_log->info(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KON_TRACE(msg, ...)   _kon_log->trace(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KON_WARN(msg, ...)    _kon_log->warn(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KON_ERROR(msg, ...)   _kon_log->error(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#endif

// use __PRETTY_FUNCTION__ to print c++ class:method
#if 0
#define KON_VERBOSE(msg, ...) _kon_log->verbose(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KON_INFO(msg, ...)    _kon_log->info(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KON_TRACE(msg, ...)   _kon_log->trace(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KON_WARN(msg, ...)    _kon_log->warn(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KON_ERROR(msg, ...)   _kon_log->error(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#endif

// no time info
#if 0
#include <string.h>
#include <errno.h>
#define KON_VERBOSE(msg, ...) \
fprintf(stdout, "[V][%s] ", __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define KON_DEBUG(msg, ...) \
fprintf(stdout, "[D][%s] ", __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define KON_INFO(msg, ...) \
fprintf(stdout, "[I][%s] ", __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define KON_TRACE(msg, ...) \
fprintf(stdout, "[T][%s] ", __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define KON_WARN(msg, ...) \
fprintf(stdout, "[W][%s] ", __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define KON_ERROR(msg, ...) \
fprintf(stdout, "[E][%s] ", __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#endif


#if 1
#include <string.h>
#include <errno.h>
#define KON_VERBOSE(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[V][%s][%s] ", KON_HumanFormatTime(), __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KON_DEBUG(msg, ...) \
do {    \
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[D][%s][%s] ", KON_HumanFormatTime(), __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KON_INFO(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[I][%s][%s] ", KON_HumanFormatTime(), __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KON_TRACE(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[T][%s][%s] ", KON_HumanFormatTime(), __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KON_WARN(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[W][%s][%s] ", KON_HumanFormatTime(), __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KON_ERROR(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[E][%s][%s] ", KON_HumanFormatTime(), __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#endif


#ifndef KON_AUTO_VERBOSE
#undef KON_VERBOSE
#define KON_VERBOSE(msg, ...) (void)0
#endif

#endif