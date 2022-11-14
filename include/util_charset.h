//
// Created by kevin on 2022/10/27.
//

#ifndef HTTPSERVER_UTIL_CHARSET_H
#define HTTPSERVER_UTIL_CHARSET_H

#include "apr/apr.h"
#include "apr/apr_errno.h"
#include "apr/apr_pools.h"

#if APR_CHARSET_EBCDIC || defined(DOXYGEN)
#include "apr/apr_xlate.h"

extern apr_xlate_t *ap_hdrs_to_ascii;

apr_status_t ap_init_ebcdic(apr_pool_t *pool);

#endif //HTTPSERVER_UTIL_CHARSET_H
