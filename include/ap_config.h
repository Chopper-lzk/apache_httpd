//
// Created by kevin on 2022/9/29.
//

#ifndef HTTPSERVER_AP_CONFIG_H
#define HTTPSERVER_AP_CONFIG_H

#ifdef DOXYGEN

#define  AP_DECLARE_STATIC

#define  AP_DECLARE_EXPORT

#endif

#if !defined(WIN32)

#define AP_DECLARE(type) type
#define AP_DECLARE_NONSTD(type)  type
#define AP_DECLARE_DATA

#elif defined(AP_DECLARE_STATIC)
#define AP_DECLARE(type) type __stdcall
#define AP_DECLARE_DATA
#elif defined(AP_DECLARE_EXPORT)
#define AP_DECLARE(type)  __declspec(dllexport) type __stdcall
#define AP_DECLARE_DATA   __declspec(dllexport)
#else
#define AP_DECLARE(type)  __declspec(dllimport) type __stdcall
#define AP_DECLARE_DATA   __declspec(dllimport)
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define AP_HAVE_C99
#endif

#if (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 3 && __GNUC__MINOR__ >= 3))) || __has_attribute(nonnull)
/* all pointer arguments */
#define AP_FN_ATTR_NONNULL_ALL  __attribute__((nonnull))
/* listed arguments */
#define AP_FN_ATTR_NONNULL(x)   __attribute__((nonnull x))
#else
#define AP_FN_ATTR_NONNULL_ALL
#define AP_FN_ATTR_NONNULL(x)
#endif

#if (defined(__GNUC__) && (__GNUC__ >= 4 && __GNUC_MINOR__ >= 3)) || __has_attribute(alloc_size)
#define AP_FN_ATTR_ALLOC_SIZE(x)        __attribute__((alloc_size(x)))
#define AP_FN_ATTR_ALLOC_SIZE2(x, y)    __attribute__((alloc_size(x, y)))
#else
#define AP_FN_ATTR_ALLOC_SIZE(x)
#define AP_FN_ATTR_ALLOC_SIZE2(x, y)
#endif

#if !defined(WIN32) || defined(AP_MODULE_DECLARE_STATIC)
#if defined(WIN32)
#define AP_MODULE_DECLARE(type)    type __stdcall
#else
#define AP_MODULE_DECLARE(type)    type
#endif
#define AP_MODULE_DECLARE_NONSTD(type) type
#define AP_MODULE_DECLARE_DATA
#else
#define AP_MODULE_DECLARE_EXPORT
#define AP_MODULE_DECLARE(type)          __declspec(dllexport) type __stdcall
#define AP_MODULE_DECLARE_NONSTD(type)   __declspec(dllexport) type
#define AP_MODULE_DECLARE_DATA           __declspec(dllexport)
#endif

#endif //HTTPSERVER_AP_CONFIG_H
