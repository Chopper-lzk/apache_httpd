//
// Created by kevin on 2022/10/27.
//
#include "apr/apr_time.h"
#include "util_time.h"

/* number of characters needed to format microsecond part of a timestamp. */
#define AP_CTIME_USEC_LENGTH   7
/* length of iso 8601 date/time */
#define AP_CTIME_COMPACT_LEN   20

/* the '+1' is for current second */
#define TIME_CAHCE_SIZE (AP_TIME_RECENT_THRESHOLD +1)
/* cache for exploded values for recent timestamps */

/* used for hashing items into cache */
#define TIME_CACHE_MASK (AP_TIME_RECENT_THRESHOLD)
struct exploded_time_cache_element{
    apr_int64_t t;
    apr_time_exp_t xt;
    apr_int64_t t_validate;
};

static struct exploded_time_cache_element exploded_cache_localtime[TIME_CAHCE_SIZE];
static struct exploded_time_cache_element exploded_cache_gmt[TIME_CAHCE_SIZE];

static apr_status_t  cached_explode(apr_time_exp_t *xt, apr_time_t t, struct exploded_time_cache_element *cache,
                                    int use_gmt){
    apr_int64_t seconds = apr_time_sec(t);
    struct exploded_time_cache_element *cache_element = &(cache[seconds & TIME_CACHE_MASK]);
                                                        /* equals `cache+(seconds & TIME_CACHE_MASK)` */
    struct exploded_time_cache_element cache_element_snapshot;
    if(cache_element -> t >= seconds){
        memcpy(&cache_element_snapshot, cache_element, sizeof (struct exploded_time_cache_element));
        if((seconds != cache_element_snapshot.t) || (seconds != cache_element_snapshot.t_validate)){
            if(use_gmt){
                return apr_time_exp_gmt(xt, t);
            }else{
                return apr_time_exp_lt(xt,t );
            }
        }else{
            memcpy(xt, &(cache_element_snapshot.xt), sizeof (apr_time_exp_t));
        }
    }else{
        apr_status_t r;
        if(use_gmt) r = apr_time_exp_gmt(xt, t);
        else r = apr_time_exp_lt(xt, t);
        if(r != APR_SUCCESS){
            return r;
        }
        cache_element -> t = seconds;
        memcpy(&(cache_element ->xt), xt, sizeof (apr_time_exp_t));
        cache_element -> t_validate = seconds;
    }
    xt->tm_usec = (int ) apr_time_usec(t);
    return APR_SUCCESS;
}

AP_DECLARE(apr_status_t) ap_explode_recent_localtime(apr_time_exp_t *tm, apr_time_t t){
    return cached_explode(tm,t, exploded_cache_localtime, 0);
}

AP_DECLARE(apr_status_t) ap_recent_ctime_ex(char *date_str, apr_time_t t, int option, int *len){
    apr_time_exp_t xt;
    const char *s;
    int real_year;
    int needed;

    /* calculate needed buffer length */
    if(option & AP_CTIME_OPTION_COMPACT)
        needed = AP_CTIME_COMPACT_LEN;
    else
        needed = APR_CTIME_LEN;
    if(option & AP_CTIME_OPTION_USEC){
        needed += AP_CTIME_USEC_LENGTH;
    }

    /* check provided buffer length */
    if(len && *len>=needed){
        *len = needed;
    }else{
        if(len != NULL){
            *len = 0;
        }
        return APR_ENOMEM;
    }

    /* example without options: "Wed Jun 30 21:49:08 1993" */
    /*                           123456789012345678901234  */
    /* example for compact format: "1993-06-30 21:49:08" */
    /*                              1234567890123456789  */


    ap_explode_recent_localtime(&xt, t);
    real_year = 1900 + xt.tm_year;
    if(option & AP_CTIME_OPTION_COMPACT){
        int real_month = xt.tm_mon+1;
        *date_str++ = real_year / 1000 + '0';
        *date_str++ = real_year % 1000 / 100 + '0';
        *date_str++ = real_year % 100 / 10 + '0';
        *date_str++ = real_year % 10 +'0';
        *date_str++ = '-';
        *date_str++ = real_month / 10 + '0';
        *date_str++ = real_month % 10 + '0';
        *date_str++ = '-';
    }else{
         s = &apr_day_snames[xt.tm_wday][0];
         *date_str++ = *s++;
         *date_str++ = *s++;
         *date_str++ = *s++;
         *date_str++ = ' ';
         s = &apr_month_snames[xt.tm_mon][0];
         *date_str++ = *s++;
         *date_str++ = *s++;
         *date_str++ = *s++;
         *date_str++ = ' ';
    }
    *date_str++ = xt.tm_mday / 10 + '0';
    *date_str++ = xt.tm_mday % 10 + '0';
    *date_str++ = ' ';
    *date_str++ = xt.tm_hour / 10 + '0';
    *date_str++ = xt.tm_hour % 10 + '0';
    *date_str++ = ':';
    *date_str++ = xt.tm_min / 10 + '0';
    *date_str++ = xt.tm_min % 10 + '0';
    *date_str++ = ':';
    *date_str++ = xt.tm_sec / 10 + '0';
    *date_str++ = xt.tm_sec % 10 + '0';
    if(option & AP_CTIME_OPTION_USEC){
        int div;
        int usec = (int) xt.tm_usec;
        *date_str++ = '.';
        for(div=100000; div>0; div/=10){
            *date_str++ = usec / div + '0';
            usec %= div;
        }
    }
    if(!(option & AP_CTIME_OPTION_COMPACT)){
        *date_str++ = ' ';
        *date_str++ = real_year / 1000 + '0';
        *date_str++ = real_year % 1000 / 100 + '0';
        *date_str++ = real_year % 100 / 10 + '0';
        *date_str++ = real_year % 10 + '0';
    }
    *date_str++ = 0;

    return APR_SUCCESS;
}
