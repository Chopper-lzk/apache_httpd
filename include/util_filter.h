//
// Created by kevin on 2022/10/11.
//

#ifndef HTTPSERVER_UTIL_FILTER_H
#define HTTPSERVER_UTIL_FILTER_H

#ifdef __cplusplus
extern "C"{
#endif
    /**
     * @brief input filtering modes
     */
    typedef enum {
        /** the filter should return at most readbytes data. */
        AP_MODE_READBYTES,
        /** the filter should return at most one line of CRLF data. */
        AP_MODE_GETLINE,
        /** the filter should eat any CRLF pairs that it sees in data. */
        AP_MODE_EATCRLF,
        /** the filter read should be treated as speculative. */
        AP_MODE_SEPCULATIVE,
        /** the filter read should be exhaustive. */
        AP_MODE_EXHAUSTIVE,
        /** the filter should initialize the connection if needed. */
        AP_MODE_INI

    } ap_input_mode_t;

    typedef struct ap_filter_t ap_filter_t;
    typedef struct ap_filter_rec_t ap_filter_rec_t;
    typedef struct ap_filter_provider_t ap_filter_provider_t;

    typedef apr_status_t (*ap_out_filter_func)(ap_filter_t *f,
            apr_bucket_brigade *b);
    typedef apr_status_t (*ap_in_filter_func)(ap_filter_t *f,
            apr_bucket_brigade *b,
            ap_input_mode_t mode,
            apr_read_type_e block,
            apr_off_t readbytes);
    typedef int (*ap_init_filter_func)(ap_filter_t *f);

    typedef union ap_filter_func {
        ap_out_filter_func out_func;
        ap_in_filter_func in_func;
    } ap_filter_func;

    /**
     * @brief this structure declares different kinds of filters
     */
    typedef enum {
        /** these filters are used to alter content */
        AP_FTYPE_RESOURCE      = 10,
        /** these filters are used to alter content as a whole bu
         *  after all AP_FTYPE_RESOURCE filters are execuated. */
        AP_FTYPE_CONTENT_SET   = 20,
        /** these filters are used to handle protocol */
        AP_FTYPE_PROTOCOL      = 30,
        /** these filters implement transport encodings */
        AP_FTYPE_TRANSCODE     = 40,
        /** these filters will alter the content, but in ways that
         *  are more strongly associated with connection. */
        AP_FTYPE_CONNECTION    = 50,
        /** these filters don't alter content, they are responsible for sending and receiving data. */
        AP_FTYPE_NETWORK       = 60,
        } ap_filter_type;

    /**
     * @brief thes flags used to indicate given filter is input or output filter
     */
    typedef enum {
        /** input filters */
        AP_FILTER_INPUT         = 1,
        /** output filters */
        AP_FILTER_OUTPUT        = 2,
        } ap_filter_direction_e;
    /**
     * @brief this structure is used to record information about filter registered.
     */
    struct ap_filter_rec_t{
        /** registered name for this filter */
        const char *name;

        /** function to call when this filter is invoked. */
        ap_filter_func filter_func;

        /** function to call directly before handlers are invoked. */
        ap_init_filter_func filter_init_func;

        /** the next filter_rec in list */
        ap_filter_rec_t *next;

        /** providers for this filter */
        ap_filter_provider_t *providers;

        /** the type of this filter */
        ap_filter_type ftype;

        /** trace level for this filter */
        int debug;

        /** protocol flags for this filter */
        unsigned int proto_flags;

        /** whether this filter is a input or output filters */
        ap_filter_direction_e direction;
    };

    struct ap_filter_private;

    struct ap_filter_t{
        /** internal representation of this filter,
        * includes name, type and actual function pointer.
        */
        ap_filter_rec_t *frec;

        /** a place to store any data associated with current filter */
        void *ctx;

        /** next filter in chain */
        ap_filter_t *next;

        /** request_rec associated with current filter */
        request_rec *r;

        /** conn_rec associated with current filter */
        conn_rec *c;

        /* filter private/opaque data */
        struct ap_filter_provider_t *priv;
    };

    struct ap_filter_conn_ctx;

#ifdef _cplusplus
}
#endif

#endif //HTTPSERVER_UTIL_FILTER_H
