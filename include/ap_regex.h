//
// Created by kevin on 2022/11/7.
//

#ifndef HTTPSERVER_AP_REGEX_H
#define HTTPSERVER_AP_REGEX_H

#include "apr/apr.h"
#include "ap_config.h"

/* Error values: */
enum{
    AP_REG_ASSERT = 1, /** internal error ? */
    AP_REG_ESPACE,     /** failed to get memory */
    AP_REG_INVARG,     /** invalid argument */
    AP_REG_NOMATCH,    /** match failed */
};

typedef struct {
    void *re_pcre;
    int re_nsub;
    apr_size_t re_erroffset;
}ap_regex_t;

/* Options for ap_regcomp, ap_regexec, and ap_rxplus versions: */

#define AP_REG_ICASE    0x01 /**< use a case-insensitive match */
#define AP_REG_NEWLINE  0x02 /**< don't match newlines against '.' etc */
#define AP_REG_NOTBOL   0x04 /**< ^ will not match against start-of-string */
#define AP_REG_NOTEOL   0x08 /**< $ will not match against end-of-string */

#define AP_REG_EXTENDED (0)  /**< unused */
#define AP_REG_NOSUB    (0)  /**< unused */

#define AP_REG_MULTI    0x10 /**< perl's /g (needs fixing) */
#define AP_REG_NOMEM    0x20 /**< nomem in our code */
#define AP_REG_DOTALL   0x40 /**< perl's /s flag */

#define AP_REG_NOTEMPTY 0x080 /**< Empty match not valid */
#define AP_REG_ANCHORED 0x100 /**< Match at the first position */

#define AP_REG_DOLLAR_ENDONLY 0x200 /**< '$' matches at end of subject string only */

#define AP_REG_NO_DEFAULT 0x400 /**< Don't implicitely add AP_REG_DEFAULT options */

#define AP_REG_MATCH "MATCH_" /**< suggested prefix for ap_regname */

#define AP_REG_DEFAULT (AP_REG_DOTALL|AP_REG_DOLLAR_ENDONLY)

/** compile a regular expression */
AP_DECLARE(int) ap_regcomp(ap_regex_t *preg, const char *regex, int cflags);

/** Destroy a pre-compiled regex */
AP_DECLARE(void) ap_regfree(ap_regex_t *preg);

#endif //HTTPSERVER_AP_REGEX_H
