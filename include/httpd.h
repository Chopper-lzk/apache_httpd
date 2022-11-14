//
// Created by kevin on 2022/9/27.
//

#ifndef HTTPSERVER_HTTPD_H
#define HTTPSERVER_HTTPD_H

#include "apr/apr.h"
#include "apr/apr_version.h"
#include "apr/apr_general.h"
#include "apr/apr_tables.h"
#include "apr/apr_pools.h"
#include "apr/apr_time.h"
#include "apr/apr_network_io.h"
#include "apr/apr_buckets.h"
#include "apr/apr_poll.h"
#include "apr/apr_thread_proc.h"
#include "apr/apr_hash.h"
#include "apr/apr_tables.h"
#include "apr/apr_uri.h"
#include "apr/apr_hooks.h"
#include "apr/apr_lib.h"
#include "apr/apr_random.h"
#include "apr/apr_getopt.h"
#include "apr/apr_strings.h"

#include "ap_config.h"
#include "ap_regex.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifndef AP_CORE_DECLARE
#define AP_CORE_DECLARE AP_DECLARE
#endif

#ifndef AP_DECLARE_DATA
#define AP_DECLARE_DATA
#endif
/** default string length */
#define MAX_STRING_LEN HUGE_STRING_LEN
/** length of huge string */
#define HUGE_STRING_LEN 8192



    typedef apr_uint64_t ap_method_mask_t;

    /** @see ap_method_list_t */
    typedef struct ap_method_list_t ap_method_list_t;

    /**
     * @struct ap_method_list_t
     */
    struct ap_method_list_t{
        /** bitmask used for known methods */
        ap_method_mask_t method_mask;
        /** array used for extension methods */
        apr_array_header_t *method_list;
    };

    typedef apr_uint64_t  ap_request_bnotes_t;

    /**
     * @brief this represents result of calling htaccess;
     */
    struct htaccess_result{
        /** the directory to which this applies */
        const char *dir;
        /** overrides allowed for .htaccess file */
        int override;
        /** override options allowed for .htaccess file */
        int override_opts;
        /** table of allowed directive for override */
        apr_table_t *override_list;
        /** configuration directives */
        struct ap_conf_vector_t *htaccess;
        /** next one */
        const struct htaccess_result *next;
    };

    /** a structure that represents one process */
    typedef struct process_rec process_rec;
    /** a structure that represents a virtual src */
    typedef struct server_rec server_rec;
    /** a structure that represents one connection */
    typedef struct conn_rec conn_rec;
    /** a structure that represents current requests */
    typedef struct request_rec request_rec;
    /** a structure that represents status of current connection */
    typedef struct conn_state_t conn_state_t;



    /**
     * @brief a structure that represents one process
     */
    struct process_rec{
        /** global pool */
        apr_pool_t *pool;
        /** configuration pool */
        apr_pool_t *pconf;
        /** the program used to execute program */
        const char *short_name;
        /** command line arguments */
        const char * const *argv;
        /** number of command line arguments */
        int argc;
    };

    /**
     * @struct server_addr_rec
     * @brief a structure used for per-vhost config
     */
    typedef struct server_addr_rec server_addr_rec;
    struct server_addr_rec{
        /** next src in list */
        server_addr_rec *next;
        /** name given in <VirtualHost> */
        char *virhost;
        /** the bound address, for this src */
        apr_sockaddr_t *host_addr;
        /** the bound port */
        apr_port_t *host_port;
    };

    struct ap_logconf{
        /** the per-module log levels */
        signed char *module_levels;

        /** the log level for this src */
        int level;
    };



    /**
     * @brief a structure to store information for one virtual src
     */
    struct server_rec{
        /** process this src is running in */
        process_rec *process;
        /** the next src in the list */
        server_rec *next;
        /** the name of error log */
        char *error_name;
        /** a file descriptor that references to error log */
        apr_file_t *error_log;
        /** log level configure */
        struct ap_logconf log;
        /** external error log writer provider */
        struct ap_errorlog_provider *errlog_provider;
        /** handle to be passed to external log provider's logging method */
        void *errorlog_provider_handle;

        /** config vector containing pointers to modules' per-src config structures */
        struct ap_conf_vector_t *module_config;
        /** MIME type info */
        struct ap_conf_vector_t *lookup_defaults;

        /** path to config file that src was defined in */
        const char *defn_name;
        /** line of config file */
        unsigned defn_line_number;
        /** true if this is virtual src */
        char is_virtual;

        /* information for redirects */

        /** for redirects */
        apr_port_t port;
        /** src request scheme for redirect response */
        const char *server_scheme;

        /* contact information */

        /** admin's contact information */
        char *server_admin;
        /** src host name */
        char *server_hostname;

        /* transaction handling */

        /** i haven't got a clue */
        server_addr_rec *addrs;
        /** timeout, as an apr interval, before we give up */
        apr_interval_time_t timeout;
        /** apr interval we will wait for another request */
        apr_interval_time_t keep_alive_timeout;
        /** max requests per connection */
        int keep_alive_max;
        /** use persistent connection */
        int keep_alive;

        /** normal names for serverAlias servers */
        apr_array_header_t *name;
        /** wildcarded names for serverAlias servers */
        apr_array_header_t *wild_names;

        /** pathname name for serverPath */
        const char *path;
        /** length of path */
        int pathlen;

        /** limit on size of http request line */
        int limit_req_line;
        /** limit on size of any request header field */
        int limit_req_fieldsize;
        /** limit on number of request header fields */
        int limit_req_fields;

        /** opaque storage location */
        void *context;

        /** whether keepalive timeout is explict (1) or inherited (0) from base src */
        unsigned int keep_alive_timeout_set:1;
    };

    /**
     * @brief enumeration of connection keepalive options
     */
    typedef enum {
        AP_CONN_UNKNOW,
        AP_CONN_CLOSE,
        AP_CONN_KEEPALIVE
    } ap_conn_keepalive_e;

#include "util_filter.h"

    /**
     * @brief structure to store information for one connection
     */
    struct conn_rec{
        /** pool associated with this connection */
        apr_pool_t *pool;
        /** physical vhost this conn came in on */
        server_rec *base_server;
        /** used by http_vhost.c */
        void *vhost_lookup_data;

        /* information about the connection itself */
        /** local address */
        apr_sockaddr_t *local_addr;
        /** remote address */
        apr_sockaddr_t *client_addr;
        /** client's ip address */
        char *client_ip;
        /** client's dns name, if known. */
        char *remote_host;
        /** only ever set if doing rfc1413 lookups. */
        char *remote_logname;

        /** src ip address */
        char *local_ip;
        /** local/s dns name */
        char *local_host;

        /** id for this connection, unique */
        long id;

        struct ap_conf_vector_t *conn_config;
        /** notes on *this* connection */
        apr_table_t *notes;
        /** a list of input filters to be used for this connection */
        ap_filter_t *input_filters;
        /** a list of output filters to be used for this connection */
        ap_filter_t *output_filters;
        /** handle to scoreboard information for this connection */
        void *sbh;
        /** bucket allocator */
        apr_bucket_alloc_t *bucket_alloc;
        /** current state of this connection */
        conn_state_t *cs;

        /** are there any filters that clogg/buffer the input stream */
        unsigned int clogging_input_filters:1;

        /** have we done double-reverse DNS？ -1 yes/failure, 0 not yet */
        signed int double_reverse:2;

        /** are we still talking? */
        unsigned aborted;

        /** are we going to keep connection alive for another request? */
        ap_conn_keepalive_e keepalive;

        /** how many times have we used it? */
        int keepalives;

        /** optional connection log level configuration */
        const struct ap_logconf *log;

        /** id to identify this connection in error log */
        const char *log_id;

#ifdef APAPR_HAS_THREADS
        apr_thread_t *current_thread;
#endif
        /** array of slave connections */
        apr_array_header_t *slaves;

        /** the real master connection */
        conn_rec *master;

        /** context of this connection */
        void *ctx;

        /** context under which this connection was suspended */
        void *suspend_baton;

        /** array of requests being handled under this connection */
        apr_array_header_t *requests;

        /** filters private/opaque context for this connection */
        struct ap_filter_conn_ctx *filter_conn_ctx;

        /** the minimum level of filter type to allow setaside buckets */
        int async_filter;

        int outgoing;

    };

    /**
     * enumeration of connection states
     */
    typedef enum {
        CONN_STATE_CHECK_REQUEST_LINE_READABLE,
        CONN_STATE_READ_REQUEST_LINE,
        CONN_STATE_HANDLER,
        CONN_STATE_WRITE_COMPLETION,
        CONN_STATE_SUSPENDED,
        CONN_STATE_LINGER, /* connection may be closed with lingering */
        CONN_STATE_LINGER_NORMAL, /* lingering with normal timeout */
        CONN_STATE_LINGER_SHORT /* lingering with short timeout */
    } conn_state_e;

    typedef enum {
        CONN_SENSE_DEFAULT,
        CONN_SENSE_WANT_READ, /* next event must be read */
        CONN_SENSE_WANT_WRITE /* next event must be write */
    } conn_sense_e;
    /**
     * @brief a structure contains current connection state
     */

    struct conn_state_t{
        /** current state of the connection */
        conn_state_e state;
        /** whether to read instead of write, or otherwise */
        conn_sense_e sense;
    };

    /**
     * @brief a structure that represents current request
     */
    struct request_rec{
        /** pool associated with current request */
        apr_pool_t *pool;

        /** connection to this client */
        conn_rec *connection;

        /** virtual host for this request */
        server_rec *server;

        /** pointer to redirected request if this is an external redirect */
        request_rec *next;
        /** pointer to redirected request if this is an internal redirect */
        request_rec *prev;

        /** pointer to main request is this is a sub-request */
        request_rec *main;

        /* info about request itself */
        /** first line of request */
        char *the_request;
        /** http/0.9, "simple" request */
        int assbackwards;
        /** a proxy request */
        int proxyreq;
        /** head request */
        int header_only;
        /** protocol version */
        int proto_num;
        /** protocol string, (e.g. HTTP/0.9) */
        const char *protocol;
        /** host */
        const char *hostname;

        /** time when the request started */
        apr_time_t request_time;

        /** status line, if set by script */
        const char *status_line;
        /** status line */
        int status;

        /* request method */
        /** M_GET, M_POST, etc. */
        int method_number;
        /** request method, (eg. GET, HEAD, POST, etc.) */
        const char *method;

        /**
         * 'allowed' is bitvector of allowed methods
         */
        ap_method_mask_t allowed;
        /** array of extension methods */
        apr_array_header_t *allowed_xmethods;
        /** list of allowed methods */
        ap_method_list_t *allowed_methods;

        /** byte count in stream is for body */
        apr_off_t sent_bodycnt;
        /** body byte count, for easy access */
        apr_off_t bytes_sent;
        /** list modified time of requested resource */
        apr_time_t mtime;

        /* http/1.1 connection-level feature */
        /** the range : header */
        const char *range;
        /** the real content length */
        apr_off_t clength;
        /** sending chunked transfer-coding */
        int chunked;

        /** method for reading request body */
        int read_body;
        /** reading chunked transfer-coding */
        int read_chunked;
        /** is client waiting for a 100 response */
        unsigned expecting_100;
        /** optional kept body for request. */
        apr_bucket_brigade *kept_body;
        /** for ap_body_to_table(): parsed body */
        apr_table_t *body_table;
        /** remaining bytes left to read from request body */
        apr_off_t remaining;

        /* MIME header environment */

        /** MIME header environment from request */
        apr_table_t *headers_in;
        /** MIME header environment for response */
        apr_table_t *headers_out;
        /** MIME header environment for response, printed even on errors */
        apr_table_t *err_headers_out;
        /** array of environment variables to be used fr sub processes */
        apr_table_t *subprocess_env;
        /** notes from one modules to another */
        apr_table_t *notes;

        /* content_type, handler, content_encoding and all content_languages */

        /** content type for current requests */
        const char *content_type;
        /** handler string that we use to call a handler function */
        const char *handler;

        /** how to encode the data */
        const char *content_encoding;
        /** array of string representing content languages */
        apr_array_header_t *content_languages;

        /** variant list validator */
        char *vlist_validator;

        /** if an authentication check was made, this gets set to user name */
        char *user;
        /** if an authentication check was made, this gets set to the auth type */
        char *ap_auth_type;

        /* what object is being requested */
        /** URI without any parsing */
        char *unparsed_uri;
        /** the path portion of uri */
        char *uri;
        /** filename on disk corresponding this response */
        char *filename;
        /** true filename stored in filesystem */
        char *canonical_filename;
        /** path_info extracted from this request */
        char *path_info;
        /** query_args extracted from this request */
        char *args;

        /** whether use path_info or not */
        int used_path_info;
        /** flag to determine if eos bucket has been sent */
        int eos_sent;

        /* various other configure info */
        /** options set in config files */
        struct ap_conf_vector_t *per_dir_config;
        /** notes on this request */
        struct ap_conf_vector_t *request_config;

        /** optional request log level */
        const struct ap_logconf *log;
        /** id to identify request in access and error log */
        const char *log_id;

        /** a linked list of .htaccess configuration directive */
        const struct htaccess_result *htaccess;

        /** a list of output filters to be used for this request */
        struct ap_filter_t *output_filters;
        /** a list of input filters to be used for this request */
        struct ap_filter_t *input_filters;

        /** a list of protocol level output filters */
        struct ap_filter_t *proto_output_filters;
        /** a list of protocol level input filters */
        struct ap_filter_t *proto_input_filters;

        /** this response can not be cached */
        int no_cache;
        /** there is no local copy of this response */
        int no_local_copy;

        /** mutex protect callbacks */
#if APR_HAS_THREADS
        apr_thread_mutex_t *invoke_mtx;
#endif
        /** struct containing components of uri */
        apr_uri_t parsed_uri;
        /** finfo.protection */
        apr_finfo_t  finfo;

        /** remote address information from conn_rec */
        apr_sockaddr_t *useragent_addr;
        char *useragent_ip;

        /** mime trailer environment from request */
        apr_table_t *trailers_in;
        /** mime trailer environment from response */
        apr_table_t *trailers_out;

        /** originator's dns name, null if not been checked */
        char *useragent_host;
        /** have we done double-reverse dns? */
        int double_reverse;
        /** mark request as potentially tainted */
        int taint;
        /** whether response has been flushed through network */
        unsigned int flushed:1;
        /** request flags associated with this request */
        ap_request_bnotes_t bnotes;
        /** indicates request has a body of unknown length and protocol handlers need to read it */
        int body_indeterminate;
    };

    AP_DECLARE(void) ap_abort_on_oom(void) __attribute__((noreturn));

#if APR_HAS_THREADS

AP_DECLARE(apr_status_t) ap_thread_create(apr_thread_t **thread,
                                          apr_threadattr_t *attr,
                                          apr_thread_start_t func,
                                          void *data, apr_pool_t *pool);

/* make main() thread */
AP_DECLARE(apr_status_t) ap_thread_main_create(apr_thread_t **thread, apr_pool_t *pool);
#if APR_VERSION_AT_LEAST(1,8,0)

/*
 * use APR 1.8+ implementation
 */
#if APR_HAS_THREAD_LOCAL && !defined(AP_NO_THREAD_LOCAL)

#define AP_THREAD_LOCAL               APR_THREAD_LOCAL

#endif
#define ap_thread_current             apr_thread_current
#define ap_thread_current_create      apr_thread_current_create
#define ap_thread_current_after_fork  apr_thread_current_after_fork
#else /* APR_VERSION_AT_LEAST(1.8.0) */

#if !defined(AP_NO_THREAD_LOCAL)
#if defined(__cplusplus) && __cplusplus >= 201103L
#define AP_THREAD_LOCAL thread_local
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112 &&
      (!defined(__GNUC__) || __GNUC__ >4 || (__GNUC__ ==4 && __GNUC_MINOR__ >= 9))
#define AP_THREAD_LOCAL _Thread_local
#elif defined(__GNUC__)
#define AP_THREAD_LOCAL __thread
#elif defined(WIN32) && defined(_MSC_VER)
#define AP_THREAD_LOCAL __declspec(thread)
#endif
#endif /* !defined(AP_NO_THREAD_LOCAL */

AP_DECLARE(apr_status_t) ap_thread_current_create (apr_thread_t **current,
                                                   apr_threadattr_t *attr,
                                                   apr_pool_t *pool);
AP_DECLARE(apr_thread_t *) ap_thread_current(void);
#endif /* APR_VERSION_AT_LEAST(1,8,0) */
#endif /* APR_HAS_THREADS */

#ifdef AP_THREAD_LOCAL
#define AP_HAS_THREAD_LOCAL 1
#else
#define AP_HAS_THREAD_LOCAL 0
#endif

AP_DECLARE(void) ap_log_assert(const char *szExp, const char *szFile, int nLine)
                              __attribute__((noreturn));

#define ap_assert(exp) ((exp)?(void)0: ap_log_assert(#exp, __FILE__, __LINE__))

#ifdef AP_DEBUG
#define AP_DEBUG_ASSERT(exp) ap_assert(exp)
#else
#define AP_DEBUG_ASSERT(exp) ((void)0)
#endif

#define OK        0
#define DECLINED -1
#define DONE     -2

AP_DECLARE(apr_size_t) ap_escape_errorlog_item(char *dest, const char *source, apr_size_t buflen)
                       AP_FN_ATTR_NONNULL((1));

#include "string.h"

AP_DECLARE(const char *) ap_strrchr_c(const char *s, int c);

#ifdef AP_DEBUG

#undef strrchr
#define strrchr(s, c) ap_strrchr(s, c)

#else

/**  use this instead of strrchr */
#define ap_strrchr_c(s, c)  strrchr(s, c)

#endif

/** convert a string to all lowercase */
AP_DECLARE(void) ap_str_tolower(char *s);

/** Maximum number of dynamically loaded modules */
#ifndef DYNAMIC_MODULE_LIMIT
#define DYNAMIC_MODULE_LIMIT 256
#endif

/** wrapper for calloc() that calls ap_abort_on_oom if out of memory */
AP_DECLARE(void *) ap_calloc(size_t nelem, size_t size) __attribute__((malloc)) AP_FN_ATTR_ALLOC_SIZE2(1,2);

/** wrapper for malloc() that calls ap_abort_on_oom if out of memory */
AP_DECLARE(void *) ap_malloc(size_t size) __attribute__((malloc)) AP_FN_ATTR_ALLOC_SIZE(1);

/** get the server description with form suitable for local displays */
AP_DECLARE(const char *) ap_get_server_description(void);

/** get data a time that server was built */
AP_DECLARE(const char *) ap_get_server_built(void);

/** define this to be default server homedir. */
#ifndef HTTPD_ROOT
#ifdef OS2
#define HTTPD_ROOT "/os2httpd"
#elif defined(WIN32)
#define HTTPD_ROOT "/apache"
#elif defined(NETWARE)
#define HTTPD_ROOT "/apache"
#else
/*macos, linux, ubuntu e.t.*/
#define HTTPD_ROOT "/usr/local/apache"
#endif
#endif /* HTTPD_ROOT */

#ifndef SERVER_CONFIG_FILE
#define SERVER_CONFIG_FILE "conf/httpd.conf"
#endif

/** the default default character set name to add if AddDefaultCharset is enabled. */
#define DEFAULT_ADD_DEFAULT_CHARSET_NAME   "iso-8859-1"

/** default location of documents. can be overridden by DocumentRoot directive */
#ifndef DOCUMNET_LOCATION
#ifdef OS2
/* set default for os/2 file system */
#define DOCUMENT_LOCATION HTTPD_ROOT "/docs"
#else
#define DOCUMENT_LOCATION HTTPD_ROOT"/htdocs"
#endif
#endif

/** define this to be what your per-directory security files are called */
#ifndef DEFAULT_ACCESS_FNAME
#ifdef OS2
/* set default of os/2 file system */
#define DEFAULT_ACCESS_FNAME "htaccess"
#else
#define DEFAULT_ACCESS_FNAME ".htaccess"
#endif
#endif /* DEFAULT_ACCESS_FNAME */

/** perform a case-insensitive comparison of two strings @a str1 and @a str2. */
AP_DECLARE(int) ap_cstr_casecmp(const char *s1, const char *s2);

/** get second word in the string paying attension to quoting */
AP_DECLARE(char *) ap_getword_conf(apr_pool_t *p, const char **line);

/** compiler a regular expression to be used later */
AP_DECLARE(ap_regex_t *) ap_pregcomp(apr_pool_t *p, const char * pattern, int cflags);

/** given the name of an object in file system determine if it is a directory */
AP_DECLARE(int) ap_is_directory(apr_pool_t *p, const char *name);

/** convert a string to all lowercase */
AP_DECLARE(void) ap_str_tolower(char *s);

#ifdef __cplusplus
}
#endif

#endif //HTTPSERVER_HTTPD_H

