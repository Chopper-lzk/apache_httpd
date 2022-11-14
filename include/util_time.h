//
// Created by kevin on 2022/10/27.
//

#ifndef HTTPSERVER_UTIL_TIME_H
#define HTTPSERVER_UTIL_TIME_H

#include "apr/apr_time.h"
#include "apr/apr_errno.h"
#include "ap_config.h"

/* maximum delta from current time */
#define AP_TIME_RECENT_THRESHOLD  15
/* options for ap_recent_ctime_ex */
/* no extension */
#define AP_CTIME_OPTION_NONE    0x0
/* add sub second timestamp with micro second resolution */
#define AP_CTIME_OPTION_USEC    0x1
/* use more compact iso 8601 format */
#define AP_CTIME_OPTION_COMPACT 0x2

/** convert a recent time to its human readable components in local timezone */
AP_DECLARE(apr_status_t) ap_explode_recent_localtime(apr_time_exp_t *tm,apr_time_t t);

/** format a recent timestamp in an extended ctime() format */
AP_DECLARE(apr_status_t) ap_recent_ctime_ex(char *date_str, apr_time_t t, int option, int *len);


#endif //HTTPSERVER_UTIL_TIME_H
