#ifndef KN_LOG_H
#define KN_LOG_H

extern int ENABLE_DEBUG;

#if 0
#define KN_VERBOSE(msg, ...) _kon_log->verbose(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KN_INFO(msg, ...)    _kon_log->info(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KN_TRACE(msg, ...)   _kon_log->trace(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KN_WARN(msg, ...)    _kon_log->warn(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KN_ERROR(msg, ...)   _kon_log->error(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#endif

// use __FUNCTION__ to print c method
#if 0
#define KN_VERBOSE(msg, ...) _kon_log->verbose(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KN_INFO(msg, ...)    _kon_log->info(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KN_TRACE(msg, ...)   _kon_log->trace(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KN_WARN(msg, ...)    _kon_log->warn(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KN_ERROR(msg, ...)   _kon_log->error(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#endif

// use __PRETTY_FUNCTION__ to print c++ class:method
#if 0
#define KN_VERBOSE(msg, ...) _kon_log->verbose(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KN_INFO(msg, ...)    _kon_log->info(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KN_TRACE(msg, ...)   _kon_log->trace(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KN_WARN(msg, ...)    _kon_log->warn(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define KN_ERROR(msg, ...)   _kon_log->error(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#endif

// no time info
#if 0
#include <string.h>
#include <errno.h>
#define KN_VERBOSE(msg, ...) \
fprintf(stdout, "[V][%s] ", __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define KN_DEBUG(msg, ...) \
fprintf(stdout, "[D][%s] ", __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define KN_INFO(msg, ...) \
fprintf(stdout, "[I][%s] ", __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define KN_TRACE(msg, ...) \
fprintf(stdout, "[T][%s] ", __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define KN_WARN(msg, ...) \
fprintf(stdout, "[W][%s] ", __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define KN_ERROR(msg, ...) \
fprintf(stdout, "[E][%s] ", __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#endif


#if 0
#include <string.h>
#include <errno.h>
#define KN_VERBOSE(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[V][%s][%s] ", KN_HumanFormatTime(), __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KN_DEBUG(msg, ...) \
do {    \
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[D][%s][%s] ", KN_HumanFormatTime(), __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KN_INFO(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[I][%s][%s] ", KN_HumanFormatTime(), __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KN_TRACE(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[T][%s][%s] ", KN_HumanFormatTime(), __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KN_WARN(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[W][%s][%s] ", KN_HumanFormatTime(), __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KN_ERROR(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[E][%s][%s] ", KN_HumanFormatTime(), __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#endif


#if 1
#include <string.h>
#include <errno.h>
#define KN_VERBOSE(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[V][%s] ", __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KN_DEBUG(msg, ...) \
do {    \
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[D][%s] ", __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KN_INFO(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[I][%s] ", __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KN_TRACE(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[T][%s] ", __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KN_WARN(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[W][%s] ", __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#define KN_ERROR(msg, ...) \
do {\
    if (ENABLE_DEBUG) {\
        fprintf(stdout, "[E][%s] ", __FUNCTION__);\
        fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n");\
    }\
} while (0)
#endif

#ifndef KN_AUTO_VERBOSE
#undef KN_VERBOSE
#define KN_VERBOSE(msg, ...) (void)0
#endif

#endif