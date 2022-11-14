//
// Created by kevin on 2022/9/29.
//

#ifndef HTTPSERVER_HTTP_LOG_H
#define HTTPSERVER_HTTP_LOG_H

#include "ap_config.h"
#include "httpd.h"
#include "ap_hooks.h"
#include "http_core.h"

#include "apr/apr_pools.h"

#define APLOG_NO_MODULE -1

#ifdef __cplusplus
#else
static int * const aplog_module_index;
#endif

#ifdef __cplusplus
#define APLOG_MODULE_INDEX (*aplog_module_index)
#else
#define APLOG_MODULE_INDEX \
       (aplog_module_index? *aplog_module_index:APLOG_NO_MODULE)
#endif

#ifdef HAVE_SYSLOG
#include "syslog.h"

#define APLOG_NOTICE     LOG_NOTICE
#define APLOG_WARNING    LOG_WARNING
#define APLOG_CRIT       LOG_CRIT

#define APLOG_LEVELMASK 15 /* mask off the level value */

#else

#define APLOG_EMERG      0     /* system is unusable */
#define APLOG_ALERT      1     /* action must be taken immediately */
#define APLOG_CRIT       2     /* critical conditions */
#define APLOG_ERR        3     /* error conditions */
#define APLOG_WARNING    4     /* warning conditions */
#define APLOG_NOTICE     5     /* normal but significant condition */
#define APLOG_INFO       6     /* informational */
#define APLOG_DEBUG      7     /* debug-level messages */
#define APLOG_TRACE1     8     /* trace-level 1 messages */
#define APLOG_TRACE2     9     /* trace-level 2 messages */
#define APLOG_TRACE3    10     /* trace-level 3 messages */
#define APLOG_TRACE4    11     /* trace-level 4 messages */
#define APLOG_TRACE5    12     /* trace-level 5 messages */
#define APLOG_TRACE6    13     /* trace-level 6 messages */
#define APLOG_TRACE7    14     /* trace-level 7 messages */
#define APLOG_TRACE8    15     /* trace-level 8 messages */

#define APLOG_LEVELMASK 15 /* mask off the level value */

#endif

#define APLOG_STARTUP      ((APLOG_LEVELMASK +1)*4)

#ifdef DOXYGEN
#define APLOG_MAX_LOGLEVEL
#endif
#ifndef APLOG_MAX_LOGLEVEL
#define APLOG_MODULE_IS_LEVEL(s, module_index, level)     \
        ( (((level)& APLOG_LEVELMASK) <= APLOG_NOTICE) || \
        (s == NULL) ||                                    \
        (ap_get_server_module_loglevel(s,module_index)    \
        >= ((level) & APLOG_LEVELMASK)))

#else

#define APLOG_MODULE_IS_LEVEL(s, module_index, level)     \
        ( (((level)& APLOG_LEVELMASK) <= APLOG_MAX_LOGLEVEL) && \
        ( (((level)& APLOG_LEVELMASK) <= APLOG_NOTICE) || \
        (s == NULL) ||                                    \
        (ap_get_server_module_loglevel(s,module_index)    \
        >= ((level) & APLOG_LEVELMASK))))
#endif

AP_DECLARE_DATA extern int ap_default_loglevel;

#define APLOG_MARK __FILE__, __LINE__, APLOG_MODULE_INDEX

#ifndef DEFAULT_LOGLEVEL
#define DEFAULT_LOGLEVEL     APLOG_WARNING
#endif

#ifdef DOXYGEN
AP_DECLARE(void) ap_log_error(const char *file, int line, int module_index,
                              int level, apr_status_t status,
                              const server_rec *s, const char *fmt, ...);
#else
#ifdef AP_HAVE_C99
#define ap_log_error(...) ap_log_error__(__VA_ARGS__)
#define ap_log_error__(file, line, mi, level, status, s,...) \
        do { const server_rec *sr__ = s; if(APLOG_MODULE_IS_LEVEL(sr__, mi, level)) \
                  ap_log_error_(file, line, mi, level, status, sr__, __VA_ARGS__); \
        }while(0)  /* this means only execute once */
#else
#define ap_log_error ap_log_error_
#endif
AP_DECLARE(void) ap_log_error_(const char *file, int line, int module_index,
                               int level, apr_status_t status,
                               const server_rec *s, const char *fmt, ...)
                               __attribute__((format(printf, 7, 8)));
#endif

/** ap_log_perror() - log messages which are not related to a particular request, connection, or virtual server.*/
#ifdef DOXYGEN
AP_DECLARE(void) ap_log_perror(const char *file, int line, int module_index,
                              int level, apr_status_t status,apr_pool_t *p, const char *fmt, ...);
#else
#if defined(AP_HAVE_C99) && defined(APLOG_MAX_LOGLEVEL)
#define ap_log_perror(...) ap_log_perror__(__VA_ARGS__)
#define ap_log_perror__(file, line, mi, level, status, p,...) \
do { if ((level)) <= APLOG_MAX_LOGLEVEL \
ap_log_perror_(file, line, mi, level, status, p, __VA_ARGS__); \
}while(0)  /* this means only execute once */
#else
#define ap_log_perror ap_log_perror_
#endif
AP_DECLARE(void) ap_log_perror_(const char *file, int line, int module_index,
                               int level, apr_status_t status,
                               apr_pool_t *p, const char *fmt, ...)
                               __attribute__((format(printf, 7, 8)));
#endif


#define APLOGNO(n)   "AH" #n ":"

AP_DECLARE(void) ap_open_stderr_log(apr_pool_t* p);

AP_DECLARE_HOOK(int, generate_log_id, (const conn_rec *c, const request_rec *r, const char **id))

#endif //HTTPSERVER_HTTP_LOG_H
