//
// Created by kevin on 2022/10/14.
//

#ifndef HTTPSERVER_MPM_COMMON_H
#define HTTPSERVER_MPM_COMMON_H

#include "ap_config.h"
#include "ap_hooks.h"
#include "httpd.h"

#include "apr/apr.h"

AP_DECLARE_DATA extern apr_uint32_t ap_max_mem_free;

/** query a property of current mpm */
AP_DECLARE(apr_status_t) ap_mpm_query(int query_code, int *result);

AP_DECLARE_HOOK(int, mpm_query, (int query_code, int *result, apr_status_t *rv))

void ap_mpm_rewrite_args(process_rec *);

#endif //HTTPSERVER_MPM_COMMON_H
