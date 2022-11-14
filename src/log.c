//
// Created by kevin on 2022/9/29.
//
#include "stdlib.h"
#include "unistd.h"

#include "apr/apr.h"
#include "apr/apr_general.h"        /* for signal stuff */
#include "apr/apr_thread_proc.h"
#include "apr/apr_strings.h"
#include "apr/apr_portable.h"

#include "http_log.h"
#include "ap_config.h"
#include "http_main.h"
#include "ap_hooks.h"
#include "util_time.h"
#include "mpm_common.h"
#include "ap_mpm.h"

#undef APLOG_MODULE_INDEX
#define APLOG_MODULE_INDEX AP_CORE_MODULE_INDEX

typedef struct{
    const char *t_name;
    int t_val;
} TRANS;

APR_HOOK_STRUCT(
        APR_HOOK_LINK(error_log)
        APR_HOOK_LINK(generate_log_id)
)

static apr_file_t *stderr_log = NULL;

int AP_DECLARE_DATA ap_default_loglevel = DEFAULT_LOGLEVEL;

static const TRANS priorities[] = {
        {"emerg", APLOG_EMERG},
        {"alert", APLOG_ALERT},
        {"crit", APLOG_CRIT},
        {"error", APLOG_ERR},
        {"warn", APLOG_WARNING},
        {"notice", APLOG_NOTICE},
        {"info",    APLOG_INFO},
        {"debug",   APLOG_DEBUG},
        {"trace1",  APLOG_TRACE1},
        {"trace2",  APLOG_TRACE2},
        {"trace3",  APLOG_TRACE3},
        {"trace4",  APLOG_TRACE4},
        {"trace5",  APLOG_TRACE5},
        {"trace6",  APLOG_TRACE6},
        {"trace7",  APLOG_TRACE7},
        {"trace8",  APLOG_TRACE8},
        {NULL,      -1},    /* mark the end */
};


AP_DECLARE(void) ap_open_stderr_log(apr_pool_t* p){
    apr_file_open_stderr(&stderr_log, p);
}

AP_DECLARE(void) ap_log_assert(const char *szExp, const char *szFile, int nLine){
    char time_str[APR_CTIME_LEN];
    apr_ctime(time_str, apr_time_now());
    ap_log_error(APLOG_MARK,APLOG_CRIT, 0, NULL, APLOGNO(00102)
                 "[%s] file %s, line %d, assertion \"%s\" failed", time_str, szFile, nLine,szExp);
#if defined(WIN32)
    DebugBreak();
#else
    abort();
#endif
}

static void add_log_id(const conn_rec *c, const request_rec *r){

    const char **id;
    if(r){
        id = &((request_rec *)r)->log_id;
    }else{
        id = &((conn_rec *)r)->log_id;
    }
    ap_run_generate_log_id(c, r, id);
}

static int do_errorlog_format(apr_array_header_t *fmt, ap_errorlog_info *info, char *buf, int buflen,
                              int *errstr_start,int *errstr_end, const char *err_fmt, va_list args){
#ifndef AP_UNSAFE_ERROR_LOG_UNESCAPED
    char scratch[MAX_STRING_LEN];
#endif
    int i;
    int len = 0;
    int field_start = 0;
    int skipping = 0;

    ap_errorlog_format_item *items = (ap_errorlog_format_item *)fmt->elts;

    AP_DEBUG_ASSERT(fmt->nelts > 0);
    for(i=0;i<fmt->nelts;i++){
        ap_errorlog_format_item *item = &items[i];
        if(item->flags & AP_ERRORLOG_FLAG_FILED_SEP){
            if(skipping){
                skipping = 0;
            }else{
                field_start = 0;
            }
        }

        if(item->flags & AP_ERRORLOG_FLAG_MESSAGE){
            *errstr_start = len;
#ifndef AP_UNSAFE_ERROR_LOG_UNESCAPED
            if(apr_vsnprintf(scratch, MAX_STRING_LEN, err_fmt, args)){
                len += ap_escape_errorlog_item(buf+len, scratch, buflen-len);
            }
#else
            len += apr_vsnprintf(buf+len, buflen-len, err_fmt, args);
#endif
            *errstr_end = len;

        }
        else if(skipping) continue;
        else if(info->level !=-1 &&(int) item->min_loglevel > info->level){
            len = field_start;
            skipping = 1;
        }else{
            int item_len = (*item->func)(info, item->arg, buf+len, buflen-len);
            if(!item_len){
                if(item ->flags & AP_ERRORLOG_FLAG_REQUIRED){
                    buf[0] = '\0';
                    return 0;
                }
                else if(item-> flags & AP_ERRORLOG_FLAG_NULL_AS_HYPHEN){
                    buf[len++] = '-';
                }else{
                    len = field_start;
                    skipping = 1;
                }
            }else{
                len += item_len;
            }
        }
    }
    return len;
}

static int log_ctime(const ap_errorlog_info *info, const char *arg, char *buf, int buflen){
    int time_len = buflen;
    int option = AP_CTIME_OPTION_NONE;
    while (arg && *arg){
        switch (*arg) {
            case 'u': option |= AP_CTIME_OPTION_USEC;
                      break;
            case 'c': option |= AP_CTIME_OPTION_COMPACT;
                      break;
        }
        arg++;
    }
    ap_recent_ctime_ex(buf, apr_time_now(), option, &time_len);
    return time_len -1;
}

static int cpystrn(char *buf, const char *arg, int buflen){
    char *end;
    if(!arg) return 0;
    end = apr_cpystrn(buf, arg, buflen);
    return end - buf;
}

static int log_module_name(const ap_errorlog_info *info, const char *arg, char *buf, int buflen){
    return cpystrn(buf, ap_find_module_short_name(info->module_index), buflen);
}

static int log_loglevel(const ap_errorlog_info *info, const char *arg, char * buf, int buflen){
    if(info -> level < 0)
        return 0;
    else
        return cpystrn(buf, priorities[info->level].t_name, buflen);
}
static int log_pid(const ap_errorlog_info *info, const char *arg, char *buf, int buflen){
    pid_t pid = getpid();
    return apr_snprintf(buf, buflen, "%" APR_PID_T_FMT, pid);
}

static int log_tid(const ap_errorlog_info *info, const char *arg, char *buf, int buflen){
#if APR_HAS_THREADS
    int result;
#endif
#if defined(HAVE_GETTID) || defined(HAVE_SYS_GETTID) || defined(HAVE_PTHREAD_GETTHREADID_NP)
    if(arg && *arg == 'g'){
#if defined(HAVE_GETTID)
        pid_t tid = gettid()
#elif defined(HAVE_PTHREAD_GETTHREADID_NP)
        pid_t tid = syscall(SYS_gettid);
#endif
        if(tid == -1) return 0;
        return apr_snprintf(buf, buflen, "%" APR_PID_T_FMT, tid);
    }
#endif
#if APR_HAS_THREADS
    if(ap_mpm_query(AP_MPMQ_IS_THREADED, &result) == APR_SUCCESS && result != AP_MPMQ_NOT_SUPPORTED){
        apr_os_thread_t tid = apr_os_thread_current();
        return apr_snprintf(buf, buflen, "%pT", &tid);  /* why we print &tid not tid ?*/
    }
#endif
    return 0;
}

static int log_file_line(const ap_errorlog_info *info, const char *arg, char *buf, int buflen){
    if(info -> file == NULL){
        return  0;
    }else{
        const char * file = info ->file;
#if defined(_OSD_POSIX) || defined(WIN32) || defined(__MVS__)
        char tmp[256];
        char *e = strrchr(file, '/');
#ifdef WIN32
        if(!e){
            e = strrchr(file, '\\');
        }
#endif
       if(e != NULL && e[1] != '\0'){
            apr_snprintf(tmp, sizeof(tmp), "%s", &e[1]);
            e = &tmp[strlen(tmp)-1];
            if(*e == ')'){
                *e = '\0';
            }
            file = tmp;
        }
#else
        const char *p;
        if(file[0] == '/' && (p = ap_strrchr_c(file, '/'))!= NULL){
            file = p+1;
        }
#endif
        return apr_snprintf(buf, buflen, "%s(%d)", file, info->line);
    }
}

static int log_apr_status(const ap_errorlog_info *info, const char *arg, char *buf, int buflen){
    apr_status_t status = info->status;
    int len;
    if(!status) return 0;

    if(status < APR_OS_START_EAIERR){
        len = apr_snprintf(buf, buflen, "(%d)", status);
    }else if(status < APR_OS_START_SYSERR){
        len = apr_snprintf(buf, buflen, "(EAI %d)", status - APR_OS_START_EAIERR);
    }else if(status < 100000+ APR_OS_START_SYSERR){
        len = apr_snprintf(buf, buflen, "(os %d)", status - APR_OS_START_SYSERR);
    }else{
        len = apr_snprintf(buf, buflen, "os 0x%08x", status - APR_OS_START_SYSERR);
    }
    apr_strerror(status, buf+len, buflen -len);
    len += strlen(buf+len);
    return len;
}
static int log_table_entry(const apr_table_t *table, const char *name, char *buf, int buflen){
#ifndef AP_UNSAFE_ERROR_LOG_UNESCAPED
    const char *value;
    char scratch[MAX_STRING_LEN];

    if((value = apr_table_get(table, name)) != NULL){
        ap_escape_errorlog_item(scratch, value, MAX_STRING_LEN);
        return cpystrn(buf, scratch, buflen);
    }

    return 0;
#else
    return cpystrn(buf, apr_table_get(table, name), buflen);
#endif
}

static int log_header(const ap_errorlog_info *info, const char *arg, char *buf, int buflen){
    if(info -> r)
        return log_table_entry(info->r->headers_in, arg, buf, buflen);
    return 0;
}

static void write_logline(char *errstr, apr_size_t len, apr_file_t *logf, int level){
    apr_file_puts(errstr, logf);
    apr_file_flush(logf);
}

static int do_errorlog_default(const ap_errorlog_info *info, char *buf, int buflen, int *errstr_start, int *errstr_end,
                               const char *errstr_fmt, va_list args){
    int len = 0;
    int field_start = 0;
    int item_len;
#ifndef AP_UNSAFE_ERROR_LOG_UNESCAPED
    char scratch[MAX_STRING_LEN];
#endif

    if(!info -> using_provider && !info ->startup){
        buf[len++] = '[';
        len += log_ctime(info, "u",  buf+len, buflen-len);
        buf[len++] = ']';
        buf[len++] = ' ';
    }
    if(!info->startup){
        buf[len++] = '[';
        len += log_module_name(info, NULL, buf+len, buflen-len);
        buf[len++] = ']';
        len += log_loglevel(info, NULL, buf+len, buflen-len);
        len += cpystrn(buf+ len, "] [pid ", buflen - len);

        len += log_pid(info, NULL, buf+len, buflen - len);
#if APR_HAS_THREADS
        field_start = len;
        len += cpystrn(buf+len, ":tid", buflen - len);
        item_len = log_tid(info, NULL, buf+len, buflen - len);
        if(!item_len)
            len = field_start;  /* got error, roll back here */
        else
            len += item_len;
#endif
        buf[len++] = ']';
        buf[len++] = ' ';
    }

    if(info->level >= APLOG_DEBUG){
        item_len = log_file_line(info, NULL, buf+len, buflen - len);
        if(item_len){
            len += item_len;
            len += cpystrn(buf+len, ": ", buflen-len);
        }
    }

    if(info->status){
        item_len = log_apr_status(info, NULL, buf+len, buflen - len);
        if(item_len){
            len += item_len;
            len += cpystrn(buf+len, ": ", buflen - len);
        }
    }

    if(info -> r){
        len += apr_snprintf(buf+len, buflen-len, "[%s %s:%d] ",
                            info->r->connection->outgoing ? "remote" : "client",
                            info->r->useragent_ip,
                            info->r->useragent_addr?info->r->useragent_addr->port : 0);
    }else if(info -> c){
        len += apr_snprintf(buf+len, buflen - len, "[%s %s:%d] ",
                            info->c->outgoing ? "remote": "client",
                            info->c->client_ip,
                            info->c->client_addr ? info->c->client_addr->port : 0);
    }
    *errstr_start = len;
#ifndef AP_UNSAFE_ERROR_LOG_UNESCAPED
    if(apr_vsnprintf(scratch, MAX_STRING_LEN, errstr_fmt, args)){
        len += ap_escape_errorlog_item(buf+len, scratch, buflen-len);
    }
#else
    len += apr_vsnprintf(buf+len, buflen - len, errstr_fmt, args);
#endif
    *errstr_end = len;
    len += cpystrn(buf+len, ", referer: ", buflen - len);
    item_len = log_header(info, "Referer", buf+len, buflen - len);
    if(item_len)
        len += item_len;
    else
        len = field_start;
    return len;
}

static void log_error_core(const char *file, int line, int module_index,
                           int level, apr_status_t status, const server_rec *s,
                           const conn_rec *c, const request_rec * r, apr_pool_t *pool,
                           const char *fmt, va_list args){
    char errstr[MAX_STRING_LEN];
    apr_file_t *logf=NULL;
    int level_and_mask = level & APLOG_LEVELMASK;
    ap_errorlog_provider *errorlog_provider = NULL;
    const request_rec *rmain = NULL;
    ap_errorlog_info info;
    void *errorlog_provider_handle = NULL;
    core_server_config *sconf = NULL;

    int log_conn_info = 0, log_req_info = 0;
    apr_array_header_t **lines = NULL;
    int done=0;
    int line_number=0;

    if(r){
        AP_DEBUG_ASSERT(r->connection != NULL);
        c = r->connection;
    }
    if(s == NULL){
#ifdef DEBUG
        if(level_and_mask != APLOG_NOTICE && level_and_mask > ap_default_loglevel) return;
#endif
        logf = stderr_log;
        if(!logf && ap_server_conf && ap_server_conf->errlog_provider){
            errorlog_provider = ap_server_conf->errlog_provider;
            errorlog_provider_handle = ap_server_conf ->errorlog_provider_handle;
        }
    }else{
        int configured_level = r ? ap_get_request_module_loglevel(r, module_index):
                               c ? ap_get_conn_server_module_loglevel(c, s, module_index):
                                   ap_get_server_module_loglevel(s, module_index);
        if((level_and_mask != APLOG_NOTICE) && level_and_mask > configured_level){
            return;
        }
        if(s-> error_log){
            logf = s->error_log;
        }
        errorlog_provider = s->errlog_provider;
        errorlog_provider_handle = s->errorlog_provider_handle;
        if(s->module_config){
            sconf = ap_get_core_module_config(s ->module_config);
            if(c && !c->log_id){
                add_log_id(c, NULL);
                if(sconf->error_log_conn &&sconf -> error_log_conn ->nelts > 0)
                    log_conn_info = 1;
            }
            if(r){
                if(r->main)
                    rmain = r->main;
                else
                    rmain = r;
                if(!rmain->log_id){
                    if(sconf->error_log_req && sconf->error_log_req->nelts > 0)
                        log_req_info = 1;
                    add_log_id(c, rmain);
                }
            }
        }
    }
    if(!logf && !(errorlog_provider &&errorlog_provider_handle)) return;
    info.s              = s;
    info.c              = c;
    info.pool           = pool;
    info.file           = NULL;
    info.line           = 0;
    info.status         = 0;
    info.using_provider = (logf == NULL);
    info.startup        = ((level & APLOG_STARTUP) == APLOG_STARTUP);
    info.format         = fmt;

    while (!done){
        apr_array_header_t *log_format;
        int len =0, errstr_start =0, errstr_end =0;
        if(log_conn_info){
            if(line_number == 0){
                lines = (apr_array_header_t **)sconf->error_log_conn->elts;
                info.r = NULL;
                info.rmain = NULL;
                info.level = -1;
                info.module_index = APLOG_NO_MODULE;
            }
            log_format = lines[line_number++];

            if(line_number == sconf ->error_log_conn->nelts){
                line_number = 0;
                log_conn_info = 0;
            }
        } else if(log_req_info){
            if(line_number == 0){
                lines = (apr_array_header_t **)sconf -> error_log_req ->elts;
                info.r = rmain;
                info.rmain = rmain;
                info.level = -1;
                info.module_index = APLOG_NO_MODULE;
            }
            log_format = lines[line_number++];
            if(line_number == sconf->error_log_req->nelts){
                line_number =0;
                log_req_info =0;
            }
        }else{
            info.r            = r;
            info.rmain        = rmain;
            info.level        = level_and_mask;
            info.module_index = module_index;
            info.file         = file;
            info.line         = line;
            info.status       = status;
            log_format = sconf ? sconf->error_log_format:NULL;
            done = 1;
        }

        if(log_format && !info.startup){
            len += do_errorlog_format(log_format, &info, errstr+len, MAX_STRING_LEN-len, &errstr_start,
                                      &errstr_end,fmt, args);
        }else{
            len += do_errorlog_default(&info, errstr+len, MAX_STRING_LEN-len, &errstr_start,
                                       &errstr_end, fmt, args);
        }
        if(!*errstr){
            /* don't log empty lines */
            continue;
        }
        if(logf || (errorlog_provider->flags & AP_ERRORLOG_PROVIDER_ADD_EOL_STR)){
            if(len > MAX_STRING_LEN - sizeof (APR_EOL_STR)){
                len = MAX_STRING_LEN - sizeof (APR_EOL_STR);
            }
            strcpy(errstr+len, APR_EOL_STR);
            len += strlen(APR_EOL_STR);
        }
        if(logf){
            write_logline(errstr, len, logf, level_and_mask);
        }else{
            errorlog_provider->writer(&info, errorlog_provider_handle, errstr, len);
        }

        if(done){
            errstr[errstr_end] = '\0';
            ap_run_error_log(&info, errstr+errstr_start);

        }
        *errstr = '\0';
    }
}

AP_DECLARE(void) ap_log_error_(const char *file, int line, int module_index,
                               int level, apr_status_t status,
                               const server_rec *s, const char *fmt, ...){
    va_list  args;
    va_start(args, fmt);
    log_error_core(file, line, module_index, level, status, s, NULL, NULL,
                   NULL, fmt, args);
    va_end(args);
}

AP_DECLARE(void) ap_log_perror_(const char *file, int line, int module_index, int level, apr_status_t status,
                                apr_pool_t *p, const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    log_error_core(file, line, module_index, level, status, NULL,NULL,NULL,p, fmt, args);
    va_end(args);
}

AP_DECLARE(const char *) ap_parse_log_level(const char *str, int *val){
    const char *err = "Log level keyword must one of emerg/alert/crit/error/"
                      "warn/notice/info/debug/trace1/.../trace8";
    int i = 0;
    if(str == NULL) return err;

    while(priorities[i].t_name != NULL){
        if(!strcmp(str, priorities[i].t_name)){
            *val = priorities[i].t_val;
            return NULL;
        }
        i++;
    }
    return err;
}

AP_IMPLEMENT_HOOK_VOID(error_log, (const ap_errorlog_info *info, const char *errstr),
                       (info, errstr))


AP_IMPLEMENT_HOOK_RUN_FIRST(int, generate_log_id, (const conn_rec *c, const request_rec *r, const char **id),
                            (c, r, id), DECLINED)
