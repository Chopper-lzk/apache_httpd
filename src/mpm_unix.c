//
// Created by kevin on 2022/11/4.
//
#include "mpm_common.h"
#include "http_main.h"

static const char *dash_k_arg = NULL;
static const char *dash_k_arg_noarg = "noarg";

void ap_mpm_rewrite_args(process_rec *process){
    apr_array_header_t *mpm_new_argv;
    char optbuf[3];
    apr_getopt_t *opt;
    apr_status_t rv;
    const char *optarg;

    mpm_new_argv = apr_array_make(process->pool, process->argc, sizeof (const char **));
                   /* could be confusing, actually here should be "sizeof (const char *)" */
    *(const char **)apr_array_push(mpm_new_argv) = process->argv[0];
    apr_getopt_init(&opt, process->pool, process -> argc, process->argv);
    opt->errfn = NULL;
    optbuf[0] = '-';
    /* option char returned by apr_getopt() will be stored in optbuf[1] */
    optbuf[2] = '\0'; /* mark the end */
    while((rv = apr_getopt(opt, "k:"AP_SERVER_BASEARGS, optbuf+1, &optarg))==APR_SUCCESS){
        switch (optbuf[1]) {
            case 'k':
                if(!dash_k_arg){
                    if(!strcmp(optarg, "start") || !strcmp(optarg, "stop") || !strcmp(optarg, "restart") ||
                       !strcmp(optarg, "graceful") || !strcmp(optarg, "graceful-stop")){
                        dash_k_arg = optarg;
                        break;
                    }
                }
            default:
                *(char**)apr_array_push(mpm_new_argv) = apr_pstrdup(process->pool, optbuf);
                if(optarg){
                    *(const char **) apr_array_push(mpm_new_argv) = optarg;
                }
        }
    }
    /* back up to capture bad argument */
    if(rv == APR_BADCH || rv == APR_BADARG){
        opt->ind--;
    }
    while(opt->ind < opt->argc){
        *(const char **) apr_array_push(mpm_new_argv) = apr_pstrdup(process->pool, opt->argv[opt->ind++]);
    }
    process->argc = mpm_new_argv ->nelts;
    process ->argv = (const char **) mpm_new_argv->elts;
    if(NULL == dash_k_arg) dash_k_arg = dash_k_arg_noarg;
}