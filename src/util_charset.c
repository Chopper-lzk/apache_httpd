//
// Created by kevin on 2022/10/27.
//

#include "ap_config.h"

#if APR_CHARSET_EBCDIC
#include "util_charset.h"

apr_xlate_t *ap_hdrs_to_ascii, *ap_hdrs_from_ascii;

apr_status_t ap_init_ebcdic(apr_pool_t *pool){
    /** todo add function body */
}

#endif