//
// Created by kevin on 2022/10/10.
//

#ifndef HTTPSERVER_HTTP_MAIN_H
#define HTTPSERVER_HTTP_MAIN_H

#include "ap_config.h"


/** name of apache executable */
APR_DECLARE_DATA extern const char *ap_server_argv0;
/** the global src's server_rec **/
AP_DECLARE_DATA  extern server_rec *ap_server_conf;
/** global pool, for access prior to creation of server_rec */
AP_DECLARE_DATA extern apr_pool_t *ap_pglobal;

/** run mode (normal, config test, config dump, e.t.) */
AP_DECLARE_DATA extern int ap_run_mode;
/** run mode (normal, config test, config dump, ...) */
AP_DECLARE_DATA extern int ap_config_generation;


/** an array of all -c directives, these are processed before server's config file */
AP_DECLARE_DATA extern apr_array_header_t *ap_server_pre_read_config;
/** an array of all -c directives, these are processed after server's config file */
AP_DECLARE_DATA extern apr_array_header_t *ap_server_post_read_config;
/** an array of all -d defines on command line this allows user to affect server based on command line options */
AP_DECLARE_DATA extern apr_array_header_t *ap_server_config_defines;

/** AP_SERVER_BASEARGS is command argument list parsed by http_main.c in apr_getopt() format */
#define AP_SERVER_BASEARGS "C:c:D:d:E:e:f:vVlLtTSMh?X"

#endif //HTTPSERVER_HTTP_MAIN_H
