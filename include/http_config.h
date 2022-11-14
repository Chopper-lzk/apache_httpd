//
// Created by kevin on 2022/10/10.
//

#ifndef HTTPSERVER_HTTP_CONFIG_H
#define HTTPSERVER_HTTP_CONFIG_H

#include "apr/apr_tables.h"

#include "util_cfgtree.h"
#include "httpd.h"
#include "ap_mmn.h"

#ifdef __cplusplus
extern "C"{
#endif

    enum cmd_how{
        RAW_ARGS, /** cmd_func parses command line itself */
        TAKE1,    /** one argument only */
        TAKE2,    /** two arguments */
        ITERATE,  /** one argument, occurring multiple times */
        ITERATE2, /** two argument, 2nd occurs multiple times */
        FLAG,     /** one of 'on' or 'off' */
        NO_ARGS,  /** no args at all */
        TAKE12,   /** one or two arguments */
        TAKE3,    /** three arguments only */
        TAKE23,   /** two of three arguments */
        TAKE123,  /** one, two or three arguments */
        TAKE13,   /** one or three arguments */
        TAKE_ARGV /** an argc and argv are passed */
    };

    typedef struct ap_configfile_t ap_configfile_t;

    struct ap_configfile_t{
        apr_status_t (*getch) (char *ch, void *param);
        apr_status_t (*getstr) (void *buf, apr_size_t bufsiz, void * param);
        apr_status_t (*close) (void *param);
        /** argument passed to getch/getstr/close */
        void *param;
        /** filename / description */
        const char *name;
        /** current line_number, starting at 1 */
        unsigned line_number;
    };
    /** configuration vector structure */
    typedef struct ap_conf_vector_t ap_conf_vector_t;

    typedef const char *(*cmd_func)();

    typedef struct command_struct command_rec;
    struct command_struct{
        /** name of this command */
        const char *name;
        /** function to be called when directive is parsed */
        cmd_func func;
        /** extra data */
        void *cmd_data;
        /** what overrides need to be allowed to enable this command. */
        int req_override;
        /** what command expects as arguments */
        enum cmd_how args_how;

        /** usage message, in case of syntax error */
        const char *errmsg;
    };

    typedef struct cmd_params_struct cmd_parms;

#if defined(AP_HAVE_DESIGNATED_INITIALIZER) || defined(DOXYGEN)
#define AP_RAW_ARGS   func.raw_args
#else
#define AP_RAW_ARGS   func
#endif

    struct cmd_params_struct{
        /** argument to command */
        void *info;
        /** which allow-override bits are set */
        int override;
        /** which allow-override-opts bits are set */
        int override_opts;
        /** table of AllowOverrideList */
        apr_table_t *override_list;
        /** which methods are */
        ap_method_mask_t limited;
        /** methods which are limited */
        apr_array_header_t *limited_xmethods;
        /** methods which are xlimited */
        ap_method_list_t *xlimited;

        /** config file structure */
        ap_configfile_t *config_file;

        /** the directive specifying this command */
        ap_directive_t *directive;

        /** pool to allocate new storage */
        apr_pool_t *pool;

        /** pool for scratch memory , wiped before first request is served */
        apr_pool_t *temp_pool;

        /** server_rec being configured for */
        server_rec *server;

        /** if configuring for a directory, pathname of that directory */
        char *path;

        /** configuration command */
        const command_rec *cmd;

        /** per_dir_config vector passed to handle command */
        struct ap_conf_vector_t *context;

        /** directive with syntax error */
        const ap_directive_t *err_directive;

        /** if current directive is exec_on_read, this is last enclosing directive */
        ap_directive_t *parent;

    };

    /** module structures */
    typedef struct module_struct module;
    struct module_struct{
        /** API version, *not* module version */
        int version;
        /** API minor version */
        int minor_version;
        /** index to modules structures in config vectors */
        int module_index;
        /** name of module's C file */
        const char *name;
        /** handle for DSO */
        void *dynamic_load_handle;

        /** pointer to next module */
        struct module_struct *next;

        /** magic cookie */
        unsigned long magic;

        /** function to allow MPMs to re-write command line arguments */
        void (*rewrite_args)(process_rec *process);

        /** function to allow all modules to create per directory configuration structures */
        void *(*create_dir_config) (apr_pool_t *p, char *dir);

        /** function to allow all modules to merge per directory configuration */
        void *(*merge_dir_config) (apr_pool_t *p, void *base_conf, void *new_conf);

        /** function to allow all modules to create per server configuration */
        void *(*create_server_config) (apr_pool_t *p, server_rec *s);

        /** function to allow all modules to merge per server configuration */
        void *(*merge_server_config) (apr_pool_t *p, void *base_conf, void *new_conf);

        /** a command_rec table that describes all of directives this module defines */
        const command_rec *cmds;

        /** a hook to allow modules to hook other points in request processing */
        void (*register_hooks) (apr_pool_t *p);

        /** a bitmask of AP_MODULE_FLAG */
        int flags;
    };

    /**
     * generic accessor to get module-specific loglevel
     */
    AP_DECLARE(int) ap_get_request_module_loglevel(const request_rec *r, int index);

    /**
     * generic accessor to get module-specific loglevel
    */
    AP_DECLARE(int) ap_get_conn_server_module_loglevel(const conn_rec *c,const server_rec *s, int index);

    /** find name of specified module */
    AP_DECLARE(const char *) ap_find_module_name(module *m);

    /** find short name of specified module */
    AP_DECLARE(const char *) ap_find_module_short_name(int module_index);


#if !defined(AP_DEBUG)

#define ap_get_module_loglevel(l, i)                                            \
        (((i)<0 ||(l)->module_levels == NULL || (l)->module_levels[i] < 0)?     \
        (l)->level:                                                             \
        (l)->module_levels[i])

#define ap_get_request_logconf(r)                        \
        ((r) -> log ?    r -> log:                       \
        (r) -> connection -> log ? r->connection ->log : \
        &(r) -> server -> log)
#define ap_get_conn_server_logconf(c, s)                                \
        ((c) -> log != &(c) -> base_server -> log && c -> log !=NULL ?  \
        (c) -> log :                                                    \
        &(s) -> log)
#define ap_get_server_module_loglevel(s, i)                             \
        ap_get_module_loglevel(&(s)->log,i)

#define ap_get_request_module_loglevel(r, i)                            \
        (ap_get_module_loglevel(ap_get_request_logconf(r),i))

#define ap_get_conn_server_module_loglevel(c, s, i)                     \
        (ap_get_module_loglevel(ap_get_conn_server_logconf(c, s), i))

#endif /* AP_DEBUG */

    /**
     * @brief this structure is used to assign symbol names to module pointers
     */
     typedef struct{
         const char *name;
         module *modp;
     } ap_module_symbol_t;

    /** add all of pre-linked modules into loaded module list */
    AP_DECLARE(const char *) ap_setup_prelinked_modules(process_rec *process);

    /** a generic pool cleanup that reset a pointer to null */
    AP_DECLARE_NONSTD(apr_status_t) ap_pool_cleanup_set_null(void* data);

    /** add a module to server */
    AP_DECLARE(const char *) ap_add_module(module *m, apr_pool_t *p, const char *s);

    /** array of all statically linked modules */
    AP_DECLARE_DATA extern module *ap_prelinked_modules[];

    /** array of all preloaded modules */
    AP_DECLARE_DATA extern module *ap_preloaded_modules[];

    /** array of all statically linked module-names */
    AP_DECLARE_DATA extern ap_module_symbol_t ap_prelinked_module_symbols[];

    /** array of all loaded modules */
    AP_DECLARE_DATA extern module **ap_loaded_modules;

    /** topmost module in list */
    AP_DECLARE_DATA extern module *ap_top_module;

    /** run register hooks function for a specified module */
    AP_DECLARE(void) ap_register_hooks(module *m, apr_pool_t *p);

    /** run all rewrite args hooks for loaded modules */
    AP_DECLARE(void) ap_run_rewrite_args(process_rec *process);

    /** parsing function for log level
     * @param str string to be parsed
     * @param val pared log level
     * @return an error string or null on success
     */
    AP_DECLARE(const char *) ap_parse_log_level(const char *str, int *val);

#ifdef AP_MAYBE_UNUSED
#elif defined(__GNUC__)
#define AP_MAYBE_UNUSED(x) x __attribute__((unused))
#elif defined(__LCLINT__)
#define AP_MAYBE_UNUSED(x) x
#else
#define AP_MAYBE_UNUSED(x) x
#endif

    /** APLOG_USE_MODULE macro is used to choose which module a file belongs to */
#define APLOG_USE_MODULE(foo) \
        extern module AP_MODULE_DECLARE_DATA foo##_module; \
        AP_MAYBE_UNUSED(static int *const aplog_module_index) = &(foo##_module.module_index)
    /** AP_DECLARE_MODULE is a convenience macro which combines a call of APLOG_USE_MODULE with definition of mdoule
     * symbol */
#define AP_DECLARE_MODULE(foo) APLOG_USE_MODULE(foo);  \
                               module AP_MODULE_DECLARE_DATA foo##_module


    /** use this only in mpm */
#define MPM20_MODULE_STUFF    MODULE_MAGIC_NUMBER_MAJOR, \
                              MODULE_MAGIC_NUMBER_MINOR, \
                              -1,                        \
                              __FILE__,                  \
                              NULL,                      \
                              NULL,                      \
                              MODULE_MAGIC_COOKIE


    /**
   * @defgroup ConfigDirectives Allowed locations for configuration directives.
   *
   * The allowed locations for a configuration directive are the union of
   * those indicated by each set bit in the req_override mask.
   *
   * @{
   */
#define OR_NONE      0           /**< *.conf is not available anywhere in this override */
#define OR_LIMIT     1           /**< *.conf inside &lt;Directory&gt; or &lt;Location&gt;
                                 and .htaccess when AllowOverride Limit */
#define OR_OPTIONS   2           /**< *.conf anywhere
                                 and .htaccess when AllowOverride Options */
#define OR_FILEINFO  4           /**< *.conf anywhere
                                 and .htaccess when AllowOverride FileInfo */
#define OR_AUTHCFG   8           /**< *.conf inside &lt;Directory&gt; or &lt;Location&gt;
                                 and .htaccess when AllowOverride AuthConfig */
#define OR_INDEXES   16          /**< *.conf anywhere
                                 and .htaccess when AllowOverride Indexes */
#define OR_UNSET     32          /**< bit to indicate that AllowOverride has not been set */
#define ACCESS_CONF  64          /**< *.conf inside &lt;Directory&gt; or &lt;Location&gt; */
#define RSRC_CONF    128         /**< *.conf outside &lt;Directory&gt; or &lt;Location&gt; */
#define EXEC_ON_READ 256         /**< force directive to execute a command
                                  which would modify the configuration (like including another
                                  file, or IFModule */
/* Flags to determine whether syntax errors in .htaccess should be
 * treated as nonfatal (log and ignore errors)
 */
#define NONFATAL_OVERRIDE 512    /* Violation of AllowOverride rule */
#define NONFATAL_UNKNOWN 1024    /* Unrecognised directive */
#define NONFATAL_ALL (NONFATAL_OVERRIDE|NONFATAL_UNKNOWN)

#define PROXY_CONF 2048          /**< *.conf inside &lt;Proxy&gt; only */

    /** this directive can be placed anywhere */
#define OR_ALL (OR_LIMIT|OR_OPTIONS|OR_FILEINFO|OR_AUTHCFG|OR_INDEXES)

    /** @} */

    /** allocate new ap_logconf and make copy of old ap_logconf */
    AP_DECLARE(struct ap_logconf *) ap_new_log_config(apr_pool_t *p, const struct ap_logconf *old);

    /** merge old ap_logconf into new ap_logconf */
    AP_DECLARE(void ) ap_merge_log_config(const struct ap_logconf *old_conf, struct ap_logconf *new_conf);

    /** setup config vector for per dir module configs */
    AP_CORE_DECLARE(ap_conf_vector_t *) ap_create_per_dir_config(apr_pool_t *p);

#if defined(AP_HAVE_DESIGNATED_INITIALIZER) || defined(DOXYGEN)
/* todo add definition when  `defined(AP_HAVE_DESIGNATED_INITIALIZER) || defined(DOXYGEN)` is true */
#else
typedef const char *(*cmd_func) ();

#define AP_INIT_RAW_ARGS(directive, func, mconfig, where, help) {directive, func, mconfig,where, RAW_ARGS, help}
#endif

/** check context a command is used in */
AP_DECLARE(const char *) ap_check_cmd_context(cmd_parms *cmd, unsigned forbidden);

#define  NOT_IN_VIRTUALHOST     0x01 /**< Forbidden in &lt;VirtualHost&gt; */
#define  NOT_IN_LIMIT           0x02 /**< Forbidden in &lt;Limit&gt; */
#define  NOT_IN_DIRECTORY       0x04 /**< Forbidden in &lt;Directory&gt; */
#define  NOT_IN_LOCATION        0x08 /**< Forbidden in &lt;Location&gt; */
#define  NOT_IN_FILES           0x10 /**< Forbidden in &lt;Files&gt; or &lt;If&gt;*/
#define  NOT_IN_HTACCESS        0x20 /**< Forbidden in .htaccess files */
#define  NOT_IN_PROXY           0x40 /**< Forbidden in &lt;Proxy&gt; */
/** Forbidden in &lt;Directory&gt;/&lt;Location&gt;/&lt;Files&gt;&lt;If&gt;*/
#define  NOT_IN_DIR_LOC_FILE    (NOT_IN_DIRECTORY|NOT_IN_LOCATION|NOT_IN_FILES)
/** Forbidden in &lt;Directory&gt;/&lt;Location&gt;/&lt;Files&gt;&lt;If&gt;&lt;Proxy&gt;*/
#define  NOT_IN_DIR_CONTEXT     (NOT_IN_LIMIT|NOT_IN_DIR_LOC_FILE|NOT_IN_PROXY)
/** Forbidden in &lt;VirtualHost&gt;/&lt;Limit&gt;/&lt;Directory&gt;/&lt;Location&gt;/&lt;Files&gt;/&lt;If&gt;&lt;Proxy&gt;*/
#define  GLOBAL_ONLY            (NOT_IN_VIRTUALHOST|NOT_IN_DIR_CONTEXT)

/** ask a module to create per-server and per-section (dir/loc/file) configs */
AP_CORE_DECLARE(void *) ap_set_config_vectors(server_rec *server, ap_conf_vector_t *section_vector, const char *section,
                                              module *mod, apr_pool_t *pconf);

/** generic accessors for other modules to get at their own module-specific data */
AP_DECLARE(void *) ap_get_module_config(const ap_conf_vector_t *cv, const module *m);

#if !defined(AP_DEBUG)
#define ap_get_module_config(v, m)  ((void **)(v))[m->module_index]
#define ap_set_module_config(v, m, val) ((void **)(v))[m->module_index] = val
#endif

/** walk a config tree and setup server's internal structure */
AP_DECLARE(const char *) ap_walk_config(ap_directive_t *conftree, cmd_parms *parms,
                                              ap_conf_vector_t *section_vector);

#ifdef __cplusplus
}
#endif
#endif //HTTPSERVER_HTTP_CONFIG_H
