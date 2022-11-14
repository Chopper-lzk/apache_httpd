//
// Created by kevin on 2022/10/14.
//
#include "apr/apr.h"
#include "apr/apr_errno.h"
#include "apr/apr_hooks.h"

#include "mpm_common.h"
#include "ap_hooks.h"
#include "httpd.h"

#define DEFAULT_HOOK_LINKS \
        APR_HOOK_LINK(monitor) \
        APR_HOOK_LINK(drop_privileges) \
        APR_HOOK_LINK(mpm) \
        APR_HOOK_LINK(mpm_query) \
        APR_HOOK_LINK(mpm_register_timed_callback) \
        APR_HOOK_LINK(mpm_register_poll_callback) \
        APR_HOOK_LINK(mpm_register_poll_callback_timeout) \
        APR_HOOK_LINK(mpm_get_name) \
        APR_HOOK_LINK(mpm_resume_suspended) \
        APR_HOOK_LINK(end_generation) \
        APR_HOOK_LINK(child_status) \
        APR_HOOK_LINK(output_pending) \
        APR_HOOK_LINK(input_pending) \
        APR_HOOK_LINK(suspend_connection) \
        APR_HOOK_LINK(resume_connection) \
        APR_HOOK_LINK(child_stopping) \
        APR_HOOK_LINK(child_stopped)

#if AP_ENABLE_EXCEPTION_HOOK
APR_HOOK_STRUCT(
        APR_HOOK_LINK(fatal_exception)
        DEFAULT_HOOK_LINKS
        )
AP_IMPLEMENT_HOKK_RUN_ALL(int, fatal_exception, (ap_exception_info_t *et), (ei), OK, DECLINED)
#else
APR_HOOK_STRUCT(
        DEFAULT_HOOK_LINKS
        )
#endif
#define ALLOCATOR_MAX_FREE_DEFAULT (2048*1024)
AP_DECLARE_DATA apr_uint32_t ap_max_mem_free = ALLOCATOR_MAX_FREE_DEFAULT;

AP_IMPLEMENT_HOOK_RUN_FIRST(int, mpm_query,
                            (int query_code, int *result, apr_status_t *_rv),
                            (query_code, result, _rv), DECLINED)


AP_DECLARE(apr_status_t) ap_mpm_query(int query_code, int *result){
    apr_status_t rv;
    if(ap_run_mpm_query(query_code, result, &rv) == DECLINED){
        rv = APR_EGENERAL;
    }
    return rv;
}