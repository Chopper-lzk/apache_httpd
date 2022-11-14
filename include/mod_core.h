//
// Created by kevin on 2022/10/17.
//

#ifndef HTTPSERVER_MOD_CORE_H
#define HTTPSERVER_MOD_CORE_H

#include "httpd.h"

/* Init RNG at startup */
AP_CORE_DECLARE(void) ap_init_rng(apr_pool_t *p);

#endif //HTTPSERVER_MOD_CORE_H
