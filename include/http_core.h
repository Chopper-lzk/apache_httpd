//
// Created by kevin on 2022/10/10.
//

#ifndef HTTPSERVER_HTTP_CORE_H
#define HTTPSERVER_HTTP_CORE_H

#include "http_config.h"
#include "ap_hooks.h"
#include "ap_regex.h"
#include "ap_expr.h"


/** No directives */
#define OPT_NONE 0
/** Indexes directive */
#define OPT_INDEXES 1
/** SSI is enabled without exec= permission  */
#define OPT_INCLUDES 2  /* 1 << 1 */
/**  FollowSymLinks directive */
#define OPT_SYM_LINKS 4 /* 1 << 2 */
/**  ExecCGI directive */
#define OPT_EXECCGI 8   /* 1 << 3 */
/**  directive unset */
#define OPT_UNSET 16
/**  SSI exec= permission is permitted, iff OPT_INCLUDES is also set */
#define OPT_INC_WITH_EXEC 32
/** SymLinksIfOwnerMatch directive */
#define OPT_SYM_OWNER 64
/** MultiViews directive */
#define OPT_MULTI 128
/**  All directives */
#define OPT_ALL (OPT_INDEXES|OPT_INCLUDES|OPT_INC_WITH_EXEC|OPT_SYM_LINKS|OPT_EXECCGI)
/** @} */


/* --------------------------------------------------------------------
 *
 * error log formats
 */
typedef struct ap_errorlog_info{
     /** current server_rec. */
     const server_rec *s;

     /** current conn_rec; */
     const conn_rec *c;

     /** current request_rec */
     const request_rec *r;

     /** r->main if r is a subrequest, otherwise equal to r */
     const request_rec *rmain;

     /** pool passed to ap_log_perror; **/
     apr_pool_t  *pool;

     /** name of source file */
     const char *file;

     /** line number in source file */
     int line;

     /** module index of module */
     int module_index;
     /** log level of error message */
     int level;

     /** apr error status */
     apr_status_t status;
     /** 1 if APLOG_STARTUP was set for log message */
     int using_provider;
     /** 1 if APLOG_STARTUP was set for log message */
     int startup;

     /** message format */
     const char *format;

} ap_errorlog_info;

/** add APR_EOL_STR to end of log message */
#define AP_ERRORLOG_PROVIDER_ADD_EOL_STR    1

typedef struct ap_errorlog_provider ap_errorlog_provider;

struct ap_errorlog_provider{

    void * (*init) (apr_pool_t *p, server_rec *s);

    apr_status_t (*writer)(const struct ap_errorlog_info *info, void *handle,
                           const char *errstr, apr_size_t len);

    const char *(*parse_errorlog_arg)(cmd_parms *cmd, const char *arg);

    unsigned int flags;
};

typedef struct{
    char *gprof_dir;

    /* name translations */
    const char *ap_document_root;

    /* access control */
    char *access_name;
    apr_array_header_t *sec_dir;
    apr_array_header_t *sec_url;

    /* recursion back-stopper */
    int redirect_limit;      /* maximum number of internal redirects */
    int subreq_limit;        /* maximum nesting level of sub-requests */

    const char *protocol;
    apr_table_t *accf_map;

    /* array of ap_errorlog_format_item for error log format string */
    apr_array_header_t *error_log_format;
    /* two arrays of ap_errorlog_format_item for additional information */
    apr_array_header_t *error_log_conn;
    apr_array_header_t *error_log_req;

    /* TRACE CONTROL */
#define AP_TRACE_UNSET   -1
#define AP_TRACE_DISABLE  0
#define AP_TRACE_ENABLE   1
#define AP_TRACE_EXTENDED 2
    int trace_enable;
#define AP_MERGE_TRAILERS_UNSET   0
#define AP_MERGE_TRAILERS_ENABLE  1
#define AP_MERGE_TRAILERS_DIABLE  2
    int merge_trailers;

    apr_array_header_t *conn_log_level;

#define AP_HTTP09_UNSET    0
#define AP_HTTP09_ENABLE   1
#define AP_HTTP09_DISABLE  2

    char http09_enable;

#define AP_HTTP_CONFORMANCE_UNSET   0
#define AP_HTTP_CONFORMANCE_UNSAFE  1
#define AP_HTTP_CONFORMANCE_STRICT  2
    char http_conformance;

#define AP_HTTP_METHODS_UNSET       0
#define AP_HTTP_METHODS_LENIENT     1
#define AP_HTTP_METHODS_REGISTERED  2
    char http_methods;

#define AP_HTTP_CL_HEAD_ZERO_UNSET   0
#define AP_HTTP_CL_HEAD_ZERO_ENABLE  1
#define AP_HTTP_CL_HEAD_ZERO_DISABLE 2
    int http_cl_head_zero;

#define AP_HTTP_EXPECT_STRICT_UNSET   0
#define AP_HTTP_EXPECT_STRICT_ENABLE  1
#define AP_HTTP_EXPECT_STRICT_DISABLE 2
    int http_expect_strict;

    apr_array_header_t *protocols;
    int protocols_honor_order;
    int async_filter;
    unsigned int async_filter_set:1;

    apr_size_t flush_max_threshold;
    apr_int32_t  flush_max_pipelined;
    unsigned int strict_host_check;
    unsigned int merge_slashes;

}core_server_config;

typedef unsigned char allow_options_t;
typedef unsigned int  overrides_t;

typedef enum{
    srv_sig_unset,
    srv_sig_off,
    srv_sig_on,
    srv_sig_withmail
} server_signature_e;

/*
 * bits of info that go into making an ETag for a file document.
 */
typedef unsigned long etag_components_t;

/**
 * @brief per-directory configuration
 */
typedef struct {
    /** path of the directory/regx/etc. */
    char *d;
    /** number of slashes in d */
    unsigned d_components;

    /** if (opts & OPT_UNSET) then no absolute assignment to options has been made */
    allow_options_t opts;
    allow_options_t opts_add;
    allow_options_t opts_remove;
    overrides_t     override;
    allow_options_t override_opts;

    /* used to be custom response config, no longer used */
    char **response_code_strings;

    /* hostname resolution etc */
#define HOSTNAME_LOOKUP_OFF     0
#define HOSTNAME_LOOKUP_ON      1
#define HOSTNAME_LOOKUP_DOUBLE  2
#define HOSTNAME_LOOKUP_UNSET   3
    unsigned int hostname_lookups : 4;

#define USE_CANONICAL_NAME_OFF   (0)
#define USE_CANONICAL_NAME_ON    (1)
#define USE_CANONICAL_NAME_DNS   (2)
#define USE_CANONICAL_NAME_UNSET (3)
    unsigned use_canonical_name : 2;

    unsigned d_is_fnmatch : 1;

#define ADD_DEFAULT_CHARSET_OFF   (0)
#define ADD_DEFAULT_CHARSET_ON    (1)
#define ADD_DEFAULT_CHARSET_UNSET (2)
    unsigned add_default_charset : 2;
    const char *add_default_charset_name;

    /* system resource control */
#ifdef RLIMIT_CPU
    struct rlimit *limit_cpu;
#endif
#if defined(RLIMIT_DATA) || defined(RLIMIT_VMEM) || defined(RLIMIT_AS)
    struct rlimit *limit_mem;
#endif
#ifdef RLIMIT_NPROC
    struct rlimit *limit_nproc;
#endif
    apr_off_t limit_req_body; /* limit on bytes in request msg body */
    long limit_xml_body;       /* limit on bytes in XML request msg body */

    /* logging options */
    server_signature_e server_signature;

    /* access_control */
    apr_array_header_t *sec_file;
    apr_array_header_t *sec_if;
    ap_regex_t *r;

    const char *mime_type;     /* forced with ForceType */
    const char *handler;       /* forced by something other than SetHandler */
    const char *output_filters;/* forced with SetOutputFilters */
    const char *input_filters; /* forced with SetInputFilters */
    int accept_path_info;      /* forced with AcceptPathInfo */

    /* what attributes/data should be included in ETag generation? */
    etag_components_t etag_bits;
    etag_components_t etag_add;
    etag_components_t etag_remove;

    /* run-time performance tuning */
#define ENABLE_MMAP_OFF   (0)
#define ENABLE_MMAP_ON    (1)
#define ENABLE_MMAP_UNSET (2)
    unsigned int enable_mmap : 2; /* whether files in this dir can be mmap'ed */

#define ENABLE_SENDFILE_OFF   (0)
#define ENABLE_SENDFILE_ON    (1)
#define ENABLE_SENDFILE_UNSET (2)
    unsigned int enable_sendfile : 2; /* files in this dir can be sendfile'ed */

#define USE_CANONICAL_PHYS_PORT_OFF   (0)
#define USE_CANONICAL_PHYS_PORT_ON    (1)
#define USE_CANONICAL_PHYS_PORT_UNSET (2)
    unsigned int use_canonical_phys_port : 2;

    unsigned int allow_encoded_slashes : 1;   /* URLS may contain %2f w/o being pitched indiscriminately */
    unsigned int decode_encoded_slashes : 1;  /* whether to decode encoded slashes in URLs */

#define AP_CONDITION_IF     1
#define AP_CONDITION_ELSE   2
#define AP_CONDITION_ELSEIF (AP_CONDITION_ELSE | AP_CONDITION_IF)
    unsigned int condition_ifelse : 2; /* is this an <If> <ElseIf>, or <Else> */

    ap_expr_info_t *condition;  /* conditionally merge <if> sections */

    /** per-dir log config */
    struct ap_logconf *log;

    /** table of directive allowed per AllowOverrideList */
    apr_table_t *override_list;

#define AP_MAXRANGES_UNSET     -1
#define AP_MAXRANGES_DEFAULT   -2
#define AP_MAXRANGES_UNLIMITED -3
#define AP_MAXRANGES_NORANGES   0

    /** Number of ranges before returning HTTP_OK. **/
    int max_ranges;
    /** MAX number of range overlaps allowed **/
    int max_overlaps;
    /** MAX number of range reversals allowed **/
    int max_reversals;

    unsigned int allow_encoded_slashes_set : 1;
    unsigned int decode_encoded_slashes_set : 1;

    /** named back reference */
    apr_array_header_t *refs;

    /** custom response config with expression support */
    apr_hash_t *response_code_exprs;

#define AP_CGI_PASS_AUTH_OFF   (0)
#define AP_CGI_PASS_AUTH_ON    (1)
#define AP_CGI_PASS_AUTH_UNSET (2)

    /** CGIPassAuth: Whether HTTP authorization header will be passed to scripts as CGI variables; */
    unsigned int cgi_pass_auth : 2;
    unsigned int qualify_redirect_url : 2;
    ap_expr_info_t *expr_handler;  /* forced with SetHandler */

    /** Table of rules for building CGI variables, NULL if none configured */
    apr_hash_t *cgi_var_rules;
    apr_size_t read_buf_size;

} core_dir_config;


/* Generic ON/OFF/UNSET for unsigned int foo : 2 */
#define AP_CORE_CONFIG_OFF   (0)
#define AP_CORE_CONFIG_ON    (1)
#define AP_CORE_CONFIG_UNSET (2)

/** item starts a new field */
#define AP_ERRORLOG_FLAG_FILED_SEP      1
/** item is the actual error message */
#define AP_ERRORLOG_FLAG_MESSAGE        2
/** skip whole line if item is zero-length */
#define AP_ERRORLOG_FLAG_REQUIRED       4
/** log zero-length item as '-' */
#define AP_ERRORLOG_FLAG_NULL_AS_HYPHEN 8

typedef int ap_errorlog_handler_fn_t(const ap_errorlog_info *info, const char *arg, char *buf, int buflen);
typedef struct {
    /** ap_errorlog_handler function */
    ap_errorlog_handler_fn_t *func;
    /** argument passed to item in {} */
    const char *arg;
    /** combination of flags */
    unsigned int flags;
    /** only log item if message's log level is higher than this */
    unsigned int min_loglevel;
}ap_errorlog_format_item;

/** declare core_module data structure here */
AP_DECLARE_DATA extern module core_module;

/**
 * accessor for core_module's specific data.
 */
APR_DECLARE(void *) ap_get_core_module_config(const struct ap_conf_vector_t *cv);

#ifndef AP_DEBUG
#define AP_CORE_MODULE_INDEX 0
#define ap_get_core_module_config(v) \
        (((void **)(v))[AP_CORE_MODULE_INDEX])
#define ap_set_core_module_config(v, val) \
        (((void **)(v))[AP_CORE_MODULE_INDEX]) = (val)
#else
#define AP_CORE_MODULE_INDEX (AP_DEBUG_ASSERT((core_module.module_index == 0), 0))
#endif

/** varible used to describe server's state (e.t. startup, exiting ...) */
AP_DECLARE_DATA extern int ap_main_state;

/** return value for unknown query_code */
#define AP_SQ_NOT_SUPPORTED       -1

/* values returned for AP_SQ_MAIN_STATE */
/** before the config preflight */
#define AP_SQ_MS_INITIAL_STARTUP   1
/** initial configuration run for setting up log config, etc. */
#define AP_SQ_MS_CREATE_PRE_CONFIG 2
/** tearing down configuration */
#define AP_SQ_MS_DESTROY_CONFIG    3
/** normal configuration run */
#define AP_SQ_MS_CREATE_CONFIG     4
/** running the MPM */
#define AP_SQ_MS_RUN_MPM           5
/** cleaning up for exit */
#define AP_SQ_MS_EXITING           6

/* values returned for AP_SQ_RUN_MODE */
/** command line not yet parsed */
#define AP_SQ_RM_UNKNOWN           1
/** normal operation (server requests or signal server) */
#define AP_SQ_RM_NORMAL            2
/** config test only */
#define AP_SQ_RM_CONFIG_TEST       3
/** only dump some parts of the config */
#define AP_SQ_RM_CONFIG_DUMP       4

/** hook method to log error message */
AP_DECLARE_HOOK(void, error_log, (const ap_errorlog_info *info, const char  *errstr))

#define ETAG_UNSET  0
#define ETAG_NONE   (1 << 0)
#define ETAG_MTIME  (1 << 1)
#define ETAG_INODE  (1 << 2)
#define ETAG_SIZE   (1 << 3)
#define ETAG_DIGEST (1 << 4)
#define ETAG_ALL    (ETAG_MTIME | ETAG_INODE | ETAG_SIZE)
/* This is the default value used */
#define ETAG_BACKWARD (ETAG_MTIME | ETAG_SIZE)

/* generic merge of flag */
#define AP_CORE_MERGE_FLAG(field, to, base, over) to->field =  \
                           over->field != AP_CORE_CONFIG_UNSET \
                           ?over->field : base->field
/*
 * possible values for query_code in ap_state_query()
 */
/** current status of server */
#define AP_SQ_MAIN_STATE    0
/** are we going to serve requests or are we just testing/dumping config */
#define AP_SQ_RUN_MODE      1
/** generation of the top-level apache parent */
#define AP_SQ_CONFIG_GEN    2

/** Query server for some state information */
AP_DECLARE(int) ap_state_query(int query_code);

#endif //HTTPSERVER_HTTP_CORE_H
