#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

#include "httpd.h"
#include "http_main.h"
#include "http_log.h"
#include "http_request.h"
#include "ap_mpm.h"
#include "mod_core.h"

#define TASK_SWITCH_SLEEP 10000  /* microseconds */

static int abort_on_oom(int retcode){
    ap_abort_on_oom();
    return retcode;
}

static void destroy_and_exit_process(process_rec *process, int process_exit_value){
    const char *name = process->short_name? process->short_name: "httpd";
    apr_sleep(TASK_SWITCH_SLEEP);
    ap_main_state = AP_SQ_MS_EXITING;
    apr_pool_destroy(process->pool);
    apr_terminate();
    if((process_exit_value != 0) && isatty(fileno(stderr))){
        fprintf(stderr, "%s: abnormal exit %d \n", name, process_exit_value);
    }
    exit(process_exit_value);
}

static apr_status_t deregister_all_hooks(void *unused){
    (void) unused;
    ap_clear_auth_internal();
    apr_hook_deregister_all();
    return APR_SUCCESS;
}

static void reset_process_pconf(process_rec* process){
    if(process->pconf){
        apr_pool_clear(process->pconf);
        ap_server_conf = NULL;
    }else{
        apr_pool_create(&process->pconf, process->pool);
        apr_pool_tag(process->pconf,"pconf");
    }
    apr_pool_pre_cleanup_register(process->pconf, NULL, deregister_all_hooks);
}

static process_rec* init_process(int* argc, const char* const ** argv){
    process_rec *process;
    apr_pool_t *cntx;
    apr_status_t  stat;
    const char *failed = "apr_app_initialize()";

    stat = apr_app_initialize(argc, argv, NULL);
    if(stat == APR_SUCCESS){
        failed = "apr_pool_create()";
        stat = apr_pool_create(&cntx, NULL);
    }
    if(stat != APR_SUCCESS){
        /** this means apr initialization failed **/
        char ctimebuff[APR_CTIME_LEN];
        apr_ctime(ctimebuff, apr_time_now());
        fprintf(stderr, "[%s] [crit] (%d) %s: %s failed"
                        "to initial context, exiting\n",
                        ctimebuff, stat, (*argv)[0], failed);
        apr_terminate();
        exit(1);
    }
    apr_pool_abort_set(abort_on_oom, cntx);
    apr_pool_tag(cntx, "process");
    ap_open_stderr_log(cntx);

    process = apr_palloc(cntx,sizeof(process_rec));
    process->pool = cntx;
    process->pconf = NULL;
    reset_process_pconf(process);

    process->argc = *argc;
    process->argv = *argv;
    process->short_name = apr_filepath_name_get((*argv) [0]);
    //printf("debug: process short_name: %s\n", process->short_name);

#if AP_HAS_THREAD_LOCAL
{
    apr_status_t rv;
    apr_thread_t *thd = NULL;
    if((rv = ap_thread_main_create(&thd, process->pool))){
        char ctimebuff[APR_CTIME_LEN];
        apr_ctime(ctimebuff, apr_time_now());
        fprintf(stderr, "[%s] [crit] (%d) %s: failed"
                        "to initialize thread context, exiting\n",
                        ctimebuff, rv, (*argv)[0]);
        apr_terminate();
        exit(1);
    }
}
#endif

    return process;
}

static void usage(process_rec *process){
    const char *bin =process->argv[0];
    int pad_len = strlen(bin);

    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, "Usage: %s [-D name ] [-d directory] [-f file]", bin);
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL, "       %*s [-C \"directive\"] [-c \"directive\"]", pad_len, " ");

#ifdef WIN32
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "       %*s [-w] [-k start|restart|stop|shutdown] [-n service_name]",
                 pad_len, " ");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "       %*s [-k install|config|uninstall] [-n service_name]",
                 pad_len, " ");
#else
    /* XXX not all MPMs support signaling the server in general or graceful-stop in particular */
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "       %*s [-k start|restart|graceful|graceful-stop|stop]", pad_len, " ");
#endif
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "       %*s [-v] [-V] [-h] [-l] [-L] [-t] [-T] [-S] [-X]", pad_len, " ");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "Options:");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -D name            : define a name for use in "
                 "<IfDefine name> directives");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -d directory       : specify an alternate initial "
                 "ServerRoot");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -f file            : specify an alternate ServerConfigFile");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -C \"directive\"     : process directive before reading "
                 "config files");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -c \"directive\"     : process directive after reading "
                 "config files");
#ifdef NETWARE
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -n name            : set screen name");
#endif
#ifdef WIN32
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -n name            : set service name and use its "
                 "ServerConfigFile and ServerRoot");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -k start           : tell Apache to start");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -k restart         : tell running Apache to do a graceful "
                 "restart");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -k stop|shutdown   : tell running Apache to shutdown");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -k install         : install an Apache service");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -k config          : change startup Options of an Apache "
                 "service");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -k uninstall       : uninstall an Apache service");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -w                 : hold open the console window on error");
#endif
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -e level           : show startup errors of level "
                 "(see LogLevel)");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -E file            : log startup errors to file");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -v                 : show version number");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -V                 : show compile settings");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -h                 : list available command line options "
                 "(this page)");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -l                 : list compiled in modules");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -L                 : list available configuration "
                 "directives");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -t -D DUMP_VHOSTS  : show parsed vhost settings");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -t -D DUMP_RUN_CFG : show parsed run settings");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -S                 : a synonym for -t -D DUMP_VHOSTS -D DUMP_RUN_CFG");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -t -D DUMP_MODULES : show all loaded modules ");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -M                 : a synonym for -t -D DUMP_MODULES");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -t -D DUMP_INCLUDES: show all included configuration files");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -t                 : run syntax check for config files");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -T                 : start without DocumentRoot(s) check");
    ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, NULL,
                 "  -X                 : debug mode (only one worker, do not detach)");

    destroy_and_exit_process(process, 1); /* invoke usage() causes exit() finally */
}

int main(int argc, const char* const* argv){
    process_rec *process;
    apr_pool_t *pconf;
    apr_pool_t *pcommands;
    apr_getopt_t *opt;
    apr_status_t rv;
    const char *opt_arg;
    char c;
    const char *error;
    const char *def_server_root = HTTPD_ROOT;
    const char *temp_error_log = NULL;
    const char *confname = SERVER_CONFIG_FILE; /* path to server config file */

    /* this is an empty statement */
    AP_MONCONTROL(0);

    process = init_process(&argc, &argv);
    ap_pglobal = process->pool;
    pconf = process->pconf;
    ap_server_argv0 = process -> short_name;
    ap_init_rng(ap_pglobal);

    /* set up oom callback */
    apr_pool_abort_set(abort_on_oom, apr_pool_parent_get(process->pool));

#if APR_CHARSET_EBCDIC
    if(ap_init_ebcdic(ap_pglobal)!=APR_SUCCESS){
        destroy_and_exit_process(process, -1);
    }
#endif

    pcommands = ap_pglobal;
    ap_server_pre_read_config = apr_array_make(pcommands, 1, sizeof (const char *));
    ap_server_post_read_config = apr_array_make(pcommands, 1, sizeof(const char *));
    ap_server_config_defines = apr_array_make(pcommands, 1, sizeof (const char * ));

    error = ap_setup_prelinked_modules(process);
    if(error){
        ap_log_error(APLOG_MARK, APLOG_STARTUP | APLOG_EMERG, 0, NULL, APLOGNO(00012)
                     "%s:%s", ap_server_argv0, error);
        destroy_and_exit_process(process, 1);
    }

    ap_run_rewrite_args(process);

    /* maintain AP_SERVER_BASEARGS list in http_main.h to allow mpm to safety pass on our args from it's rewrite_args()
     * handler */
    apr_getopt_init(&opt, pcommands, process->argc, process->argv);

    while((rv = apr_getopt(opt, AP_SERVER_BASEARGS, &c, &opt_arg)) == APR_SUCCESS){
        const char **new;
        switch (c) {
            case 'c':
                new = (const char **) apr_array_push(ap_server_post_read_config);
                *new = apr_pstrdup(pcommands, opt_arg);
                break;
            case 'C':
                new = (const char **) apr_array_push(ap_server_pre_read_config);
                *new = apr_pstrdup(pcommands, opt_arg);
                break;
            case 'd':
                def_server_root = opt_arg; /* this changes default server root */
                break;
            case 'D':
                new = (const char **) apr_array_push(ap_server_config_defines);
                *new = apr_pstrdup(pcommands, opt_arg);
                /* setting -D DUMP_VHOSTS should work like setting -S */
                if(strcmp(opt_arg, "DUMP_VHOSTS") == 0)
                    ap_run_mode = AP_SQ_RM_CONFIG_DUMP;
                /* setting -D DUMP_RUN_CFG should work like setting -S */
                else if(strcmp(opt_arg, "DUMP_RUN_CFG")==0)
                    ap_run_mode = AP_SQ_RM_CONFIG_DUMP;
                /* setting -D DUMP_MODULES is equivalent to setting -H */
                else if(strcmp(opt_arg, "DUMP_MODULES") == 0)
                    ap_run_mode = AP_SQ_RM_CONFIG_DUMP;
                /* setting -D DUMP_INCLUDES is a type of configuration dump */
                else if(strcmp(opt_arg, "DUMP_INCLUDES") == 0)
                    ap_run_mode = AP_SQ_RM_CONFIG_DUMP;
                break;
            case 'e':
                if(ap_parse_log_level(opt_arg, &ap_default_loglevel) != NULL)
                    usage(process); /* error, print usage and exit process */
                break;
            case 'E':
                temp_error_log = apr_pstrdup(process->pool, opt_arg);
                break;
            case 'X':
                new = (const char **) apr_array_push(ap_server_config_defines);
                *new = "DEBUG";
                break;
            case 'f':
                confname = opt_arg;
                break;
            case 'v':
                printf("Server version: %s\n", ap_get_server_description());
                printf("Server built:   %s\n"), ap_get_server_built();
                destroy_and_exit_process(process, 0);

        }
    }

    return 0;
}