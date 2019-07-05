#ifndef KON_LOG_H
#define KON_LOG_H

#if 0
#define kon_verbose(msg, ...) _kon_log->verbose(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define kon_info(msg, ...)    _kon_log->info(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define kon_trace(msg, ...)   _kon_log->trace(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define kon_warn(msg, ...)    _kon_log->warn(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define kon_error(msg, ...)   _kon_log->error(NULL, _kon_context->get_id(), msg, ##__VA_ARGS__)
#endif

// use __FUNCTION__ to print c method
#if 0
#define kon_verbose(msg, ...) _kon_log->verbose(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define kon_info(msg, ...)    _kon_log->info(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define kon_trace(msg, ...)   _kon_log->trace(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define kon_warn(msg, ...)    _kon_log->warn(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define kon_error(msg, ...)   _kon_log->error(__FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#endif

// use __PRETTY_FUNCTION__ to print c++ class:method
#if 0
#define kon_verbose(msg, ...) _kon_log->verbose(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define kon_info(msg, ...)    _kon_log->info(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define kon_trace(msg, ...)   _kon_log->trace(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define kon_warn(msg, ...)    _kon_log->warn(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#define kon_error(msg, ...)   _kon_log->error(__PRETTY_FUNCTION__, _kon_context->get_id(), msg, ##__VA_ARGS__)
#endif

// #if 1
// #include <string.h>
// #include <errno.h>
// #define kon_verbose(msg, ...) \
// fprintf(stdout, "[V][%s] ", __FUNCTION__);\
// fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
// #define kon_debug(msg, ...) \
// fprintf(stdout, "[D][%s] ", __FUNCTION__);\
// fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
// #define kon_info(msg, ...) \
// fprintf(stdout, "[I][%s] ", __FUNCTION__);\
// fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
// #define kon_trace(msg, ...) \
// fprintf(stdout, "[T][%s] ", __FUNCTION__);\
// fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
// #define kon_warn(msg, ...) \
// fprintf(stdout, "[W][%s] ", __FUNCTION__);\
// fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
// #define kon_error(msg, ...) \
// fprintf(stdout, "[E][%s] ", __FUNCTION__);\
// fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
// #endif


#if 1
#include <string.h>
#include <errno.h>
#define kon_verbose(msg, ...) \
fprintf(stdout, "[V][%s][%s] ", KON_HumanFormatTime(), __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define kon_debug(msg, ...) \
fprintf(stdout, "[D][%s][%s] ", KON_HumanFormatTime(), __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define kon_info(msg, ...) \
fprintf(stdout, "[I][%s][%s] ", KON_HumanFormatTime(), __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define kon_trace(msg, ...) \
fprintf(stdout, "[T][%s][%s] ", KON_HumanFormatTime(), __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define kon_warn(msg, ...) \
fprintf(stdout, "[W][%s][%s] ", KON_HumanFormatTime(), __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#define kon_error(msg, ...) \
fprintf(stdout, "[E][%s][%s] ", KON_HumanFormatTime(), __FUNCTION__);\
fprintf(stdout, msg, ##__VA_ARGS__); fprintf(stdout, "\n")
#endif


#ifndef KON_AUTO_VERBOSE
#undef kon_verbose
#define kon_verbose(msg, ...) (void)0
#endif
#ifndef KON_AUTO_INFO
#undef kon_info
#define kon_info(msg, ...) (void)0
#endif
#ifndef KON_AUTO_TRACE
#undef kon_trace
#define kon_trace(msg, ...) (void)0
#endif

#endif