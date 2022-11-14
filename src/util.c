//
// Created by kevin on 2022/9/29.
//
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "errno.h"
#include "test_char.h"

#include "mpm_common.h"
#include "httpd.h"

static const char * const oom_message = "[crit] Memory allocation failed, "
                                        "aborting process." APR_EOL_STR;

static const char c2x_table[] = "0123456789abcdef";

static APR_INLINE unsigned char * c2x(unsigned what, unsigned char prefix, unsigned char *where){
#if APR_CHARSET_EBCDIC
    what = apr_xlate_conv_byte(ap_hdrs_to_ascii, (unsigned char)what);
#endif
    *where++ = prefix;
    *where++ = c2x_table[what >> 4];
    *where++ = c2x_table[what & 0xf];
    return where;
}

APR_DECLARE(void) ap_abort_on_oom(){
    int written, count = strlen(oom_message);
    const char *buf = oom_message;
    do{
        written = write(STDERR_FILENO, buf, count);
        if(written == count) break;
        if(written > 0){
            buf += written;
            count -= written;
        }
    } while (written >= 0 || errno == EINTR);
    abort();
}

#if APR_HAS_THREADS

#if AP_HAS_THREAD_LOCAL && !APR_VERSION_AT_LEAST(1,8,0)
static AP_THREADLOCAL apr_thread_t *current_thread = NULL;
#endif

struct thread_ctx{
    apr_thread_start_t func;
    void *data;
};

static void *APR_THREAD_FUNC thread_start(apr_thread_t *thread, void *data){
    struct thread_ctx *ctx = data;
    {
        apr_pool_t *tp = apr_thread_pool_get(thread);
        apr_allocator_t *ta = apr_pool_allocator_get(tp);
        if(ta){
            apr_allocator_max_free_set(ta, ap_max_mem_free);
        }
    }
#if AP_HAS_THREAD_LOCAL && !APR_VERSION_AT_LEAST(1,8,0)
    current_thread = thread;
#endif
    return ctx->func(thread, ctx->data);
}

static apr_status_t main_thread_cleanup(void* arg){
    apr_thread_t *thd = arg;
    apr_pool_destroy(apr_thread_pool_get(thd));
    return APR_SUCCESS;
}

APR_DECLARE(apr_status_t) ap_thread_create(apr_thread_t **thread,
                                           apr_threadattr_t *attr,
                                           apr_thread_start_t func,
                                           void *data, apr_pool_t *pool)
 {
     struct thread_ctx *ctx = apr_palloc(pool, sizeof (*ctx));
     ctx->func = func;
     ctx->data = data;
     return apr_thread_create(thread, attr, thread_start, ctx, pool);
 }

AP_DECLARE(apr_status_t) ap_thread_main_create(apr_thread_t **thread, apr_pool_t *pool)
{
    apr_status_t rv;
    apr_threadattr_t *attr = NULL;

    if((rv= apr_threadattr_create(&attr, pool))
            || (rv = apr_threadattr_detach_set(attr, 1))
#if APR_VERSION_AT_LEAST(1,8,0)
            || (rv = apr_threadattr_max_free_set(attr, ap_max_mem_free))
#endif
            || (rv = ap_thread_current_create(thread, attr, pool))){
        *thread = NULL;
        return rv;
    }
    apr_pool_cleanup_register(pool, *thread, main_thread_cleanup, apr_pool_cleanup_null);

    return APR_SUCCESS;
}

#if !APR_VERSION_AT_LEAST(1,8,0)
AP_DECLARE(apr_status_t) ap_thread_current_create(apr_thread_t **current,
                                                  apr_threadattr_t *apr,
                                                  apr_pool_t *pool)
{
# if AP_HAS_THREAD_LOCAL
    apr_allocator_t *ta;
    apr_status_t rv;
    apr_os_thread_t osthd;
    apr_pool_t *p;

    *current =ap_thread_current()
    if(*current){
        return APR_EEXIST;
    }
    abort_fn = (pool) ? apr_pool_abort_get(pool): NULL;
    rv=apr_allocator_create(&ta);
    if(rv != APR_SUCCESS){
        if(abort_fn) abort_fn(rv);
        return rv;
    }
    apr_allocator_max_free_set(ta, ap_max_mem_set);
    rv=apr_pool_create_unmanaged_ex(&p, abort_fn, ta);
    if(rv != APR_SUCCESS) return rv;
    apr_allocator_owner_set(ta, p);

    osthd = apr_os_thread_current();
    rv = apr_os_thread_put(current, &osthd, p);
    if(rv != APR_SUCCESS){
        apr_pool_destory(p);
        return rv;
    }
    current_thread = *current;
    return APR_SUCCESS;
#else
    return APR_ENOTIMPL;
#endif
}
AP_DECLARE(apr_thread_t *) ap_thread_current(void){
#if APAP_HAS_THREAD_LOCAL
    return current_thread;
#else
    return NULL;
#endif
}
#endif /* !APR_VERSION_AT_LEAST(1,8,0) */

AP_DECLARE(apr_size_t) ap_escape_errorlog_item(char *dest, const char *source, apr_size_t buflen){
    unsigned char *d, *ep;
    const unsigned char *s;

    if(!source || !buflen){
        return 0;
    }
    d = (unsigned char *) dest;
    s = (const unsigned char *) source;
    ep = d+buflen -1;
    for(; d<ep && *s; ++s){
        if(TEST_CHAR(*s, T_ESCAPE_LOGITEM)){
            *d++ = '\\';
            if(d >= ep){
                --d;
                break;
            }
            switch(*s){
                case '\b':
                    *d++= 'b';
                    break;
                case '\n':
                    *d++= 'n';
                    break;
                case '\r':
                    *d++ = 'r';
                    break;
                case '\v':
                    *d++ = 'v';
                    break;
                case '\\':
                    *d++ = *s;
                    break;
                case '"':
                    d[-1] = *s;
                    break;
                default:
                    if(d >= ep -2){
                        ep = --d;
                        break;
                    }
                    c2x(*s, 'x', d);
                    d+=3;
            }
            }
        else{
            *d++ = *s;
        }
    }
    *d = '\0';
    return (d - (unsigned char *) dest);
}

AP_DECLARE_NONSTD(apr_status_t) ap_pool_cleanup_set_null(void *data){
    void **ptr = (void **) data;
    *ptr = NULL; /* set pointer to null */
    return APR_SUCCESS;
}

AP_DECLARE(void) ap_str_tolower(char *str){
    while(*str){
        *str = apr_tolower(*str);
        ++str;
    }
}

AP_DECLARE(void*) ap_calloc(size_t nelem, size_t size){
    void *p = calloc(nelem, size);
    if(p == NULL && nelem != 0 && size != 0)
        ap_abort_on_oom();
    return p;
}

AP_DECLARE(void *) ap_malloc(size_t size){
    void *p = malloc(size);
    if(p == NULL && size != 0)
        ap_abort_on_oom();
    return p;
}

#if !APR_CHARSET_EBCDIC
/*
 * Our own known-fast translation table for casecmp by character.
 * Only ASCII alpha characters 41-5A are folded to 61-7A, other
 * octets (such as extended latin alphabetics) are never case-folded.
 * NOTE: Other than Alpha A-Z/a-z, each code point is unique!
 */
static const unsigned char ucharmap[256] = {
        0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,
        0x8,  0x9,  0xa,  0xb,  0xc,  0xd,  0xe,  0xf,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
        0x40,  'a',  'b',  'c',  'd',  'e',  'f',  'g',
        'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
        'p',  'q',  'r',  's',  't',  'u',  'v',  'w',
        'x',  'y',  'z', 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
        0x60,  'a',  'b',  'c',  'd',  'e',  'f',  'g',
        'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
        'p',  'q',  'r',  's',  't',  'u',  'v',  'w',
        'x',  'y',  'z', 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
        0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
        0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
        0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
        0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
        0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
        0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
        0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
        0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
        0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
        0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};
#else /* APR_CHARSET_EBCDIC */
/*
 * Derived from apr-iconv/ccs/cp037.c for EBCDIC case comparison,
 * provides unique identity of every char value (strict ISO-646
 * conformance, arbitrary election of an ISO-8859-1 ordering, and
 * very arbitrary control code assignments into C1 to achieve
 * identity and a reversible mapping of code points),
 * then folding the equivalences of ASCII 41-5A into 61-7A,
 * presenting comparison results in a somewhat ISO/IEC 10646
 * (ASCII-like) order, depending on the EBCDIC code page in use.
 *
 * NOTE: Other than Alpha A-Z/a-z, each code point is unique!
 */
static const unsigned char ucharmap[256] = {
        0x00, 0x01, 0x02, 0x03, 0x9C, 0x09, 0x86, 0x7F,
        0x97, 0x8D, 0x8E, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x9D, 0x85, 0x08, 0x87,
        0x18, 0x19, 0x92, 0x8F, 0x1C, 0x1D, 0x1E, 0x1F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x0A, 0x17, 0x1B,
        0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x05, 0x06, 0x07,
        0x90, 0x91, 0x16, 0x93, 0x94, 0x95, 0x96, 0x04,
        0x98, 0x99, 0x9A, 0x9B, 0x14, 0x15, 0x9E, 0x1A,
        0x20, 0xA0, 0xE2, 0xE4, 0xE0, 0xE1, 0xE3, 0xE5,
        0xE7, 0xF1, 0xA2, 0x2E, 0x3C, 0x28, 0x2B, 0x7C,
        0x26, 0xE9, 0xEA, 0xEB, 0xE8, 0xED, 0xEE, 0xEF,
        0xEC, 0xDF, 0x21, 0x24, 0x2A, 0x29, 0x3B, 0xAC,
        0x2D, 0x2F, 0xC2, 0xC4, 0xC0, 0xC1, 0xC3, 0xC5,
        0xC7, 0xD1, 0xA6, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
        0xF8, 0xC9, 0xCA, 0xCB, 0xC8, 0xCD, 0xCE, 0xCF,
        0xCC, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
        0xD8, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0xAB, 0xBB, 0xF0, 0xFD, 0xFE, 0xB1,
        0xB0, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
        0x71, 0x72, 0xAA, 0xBA, 0xE6, 0xB8, 0xC6, 0xA4,
        0xB5, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7A, 0xA1, 0xBF, 0xD0, 0xDD, 0xDE, 0xAE,
        0x5E, 0xA3, 0xA5, 0xB7, 0xA9, 0xA7, 0xB6, 0xBC,
        0xBD, 0xBE, 0x5B, 0x5D, 0xAF, 0xA8, 0xB4, 0xD7,
        0x7B, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0xAD, 0xF4, 0xF6, 0xF2, 0xF3, 0xF5,
        0x7D, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
        0x71, 0x72, 0xB9, 0xFB, 0xFC, 0xF9, 0xFA, 0xFF,
        0x5C, 0xF7, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7A, 0xB2, 0xD4, 0xD6, 0xD2, 0xD3, 0xD5,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0xB3, 0xDB, 0xDC, 0xD9, 0xDA, 0x9F
};
#endif

AP_DECLARE(int) ap_cstr_casecmp(const char *s1, const char *s2){
    const unsigned char * u1 = (const unsigned char *) s1;
    const unsigned char * u2 = (const unsigned char *) s2;
    for(;;){
        const int c2 = ucharmap[*u2++];
        const int cmp = (int)ucharmap[*u1++] -c2;
        if(cmp || !c2) return cmp;
    }
}

static char *substring_conf(apr_pool_t *p, const char *start, int len , char quote){
    char *result = apr_palloc(p, len+1);
    char * resp = result;
    int i;
    for(i=0;i<len;i++){
        if(start[i]=='\\' && (start[i+1]=='\\' || (quote && start[i+1] == quote)))
            *resp++ = start[++i];
        else
            *resp++ = start[i];
    }

    *resp++ = '\0';
#if RESOLVE_ENV_PER_TOKEN
    return (char *)ap_resolve_env(p, result);
#else
    return result;
#endif
}

AP_DECLARE(char *) ap_getword_conf(apr_pool_t *p, const char **line){
    const char *str = *line, *strend;
    char *res;
    char quote;

    while(apr_isspace(*str)) ++str;
    if(!*str){
        *line = str;
        return "";
    }
    if((quote = *str) == '"' || quote == '\''){
        strend = str+1;
        while(*strend && *strend != quote){
            if(*strend == '\\' && strend[1] && (strend[1]==quote || strend[1]=='\\')) strend+=2;
            else ++strend;
        }
        res = substring_conf(p, str+1, strend-str-1, quote);
        if(*strend ==quote) ++strend;
    }else{
        strend = str;
        while(*strend && !apr_isspace(*strend)) ++strend;
        res = substring_conf(p, str, strend-str, 0);
    }
    while(apr_isspace(*strend)) ++strend;
    *line = strend;
    return res;
}

static apr_status_t regex_cleanup(void *preg){
    ap_regfree((ap_regex_t *)preg);
    return APR_SUCCESS;
}

AP_DECLARE(ap_regex_t *) ap_pregcomp(apr_pool_t *p, const char *pattern, int cflags){
    ap_regex_t *preg = apr_palloc(p, sizeof (ap_regex_t));
    int err = ap_regcomp(preg, pattern, cflags);
    if(err){
        if(err == AP_REG_ESPACE) ap_abort_on_oom();
        return NULL;
    }
    apr_pool_cleanup_register(p, (void *)preg, regex_cleanup, apr_pool_cleanup_null);
    return preg;
}

AP_DECLARE(int) ap_is_directory(apr_pool_t *p, const char *path){
    apr_finfo_t finfo;
    if(apr_stat(&finfo, path, APR_FINFO_TYPE, p) != APR_SUCCESS)
        return 0; /* in err condition, not a directory */
    return (finfo.filetype == APR_DIR);
}



#endif /* APR_HAS_THREADS */