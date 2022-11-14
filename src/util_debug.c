//
// Created by kevin on 2022/10/18.
//

#include "http_config.h"
#include "httpd.h"
#include "http_core.h"

#if defined(ap_get_request_module_loglevel)
#undef ap_get_request_module_loglevel
APR_DECLARE(int) ap_get_request_module_loglevel(const request_rec *r, int module_index);
#endif

APR_DECLARE(int) ap_get_request_module_loglevel(const request_rec *r, int module_index){
    const struct ap_logconf *l = r -> log ? r -> log:
                                 r -> connection -> log ? r -> connection ->log:
                                 &r -> server -> log;
    if (module_index < 0 || l ->module_levels == NULL || l -> module_levels[module_index] < 0){
        return l ->level;
    }
    return l -> module_levels[module_index];
}

#if defined(ap_get_conn_server_module_loglevel)
#undef ap_get_conn_server_module_loglevel
AP_DECLARE(int) ap_get_conn_server_module_loglevel(const conn_rec *c,
                                                   const server_rec *s,
                                                   int module_index);
#endif

AP_DECLARE(int) ap_get_conn_server_module_loglevel(const conn_rec *c,
                                                   const server_rec *s,
                                                   int module_index){
    const struct ap_logconf *l = (c->log && c->log != &c->base_server->log) ?
            c->log : &s->log;
    if (module_index < 0 || l->module_levels == NULL ||
    l->module_levels[module_index] < 0)
    {
        return l->level;
    }

    return l->module_levels[module_index];
}

#if defined(ap_get_core_module_config)
#undef ap_get_core_module_config
AP_DECLARE(void *) ap_get_core_module_config(const ap_conf_vector_t *cv);
#endif

AP_DECLARE(void *) ap_get_core_module_config(const ap_conf_vector_t *cv){
    return ((void**)(cv))[AP_CORE_MODULE_INDEX];
}