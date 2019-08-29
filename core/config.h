#ifndef KN_CONFIG_H
#define KN_CONFIG_H

#ifdef _WIN32
#ifdef BUILDING_DLL
#define KN_API    __declspec(dllexport)
#else
#define KN_API    __declspec(dllimport)
#endif
#else
#define KN_API    extern
#endif

#ifndef KN_64_BIT
#if defined(__amd64) || defined(__x86_64) || defined(_WIN64) || defined(_Wp64) || defined(__LP64__) || defined(__PPC64__) || defined(__mips64__) || defined(__sparc64__)
#define KN_64_BIT 1
#else
#define KN_64_BIT 0
#endif
#endif

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__DragonFly__) || defined(__OpenBSD__)
#define KN_BSD 1
#else
#define KN_BSD 0
#if ! defined(_GNU_SOURCE) && ! defined(_WIN32) && ! defined(PLAN9)
#define _GNU_SOURCE
#endif
#endif

#endif