//
// Created by kevin on 2022/11/10.
//

#include "ap_regex.h"

#ifdef HAVE_PCRE2
    /* todo add definiation of macros when HAVE_PCRE2 defines */
#else
#include "pcre.h"
#define PCREn(x) PCRE_ ## x
#endif

static int default_cflags = AP_REG_DEFAULT;

AP_DECLARE(int) ap_regcomp(ap_regex_t *preg, const char*pattern, int cflags){
#ifdef HAVE_PCRE2
    /* todo add function body when macro HAVE_PCRE2 defines */
#else
    const char *errorptr;
    int erroffset;
#endif
    int errcode = 0;
    int options = PCREn(DUPNAMES);

    if((cflags & AP_REG_NO_DEFAULT) ==0 )
        cflags |= default_cflags;
    if((cflags & AP_REG_ICASE) != 0)
        options |= PCREn(CASELESS);
    if((cflags & AP_REG_NEWLINE) != 0)
        options |= PCREn(MULTILINE);
    if((cflags & AP_REG_DOTALL) != 0)
        options |= PCREn(DOTALL);
    if((cflags & AP_REG_DOLLAR_ENDONLY) != 0)
        options |= PCREn(DOLLAR_ENDONLY);

#ifdef HAVE_PCRE2
    /* todo: add function body when macro HAVE_PCRE2 defines */
#else
    preg->re_pcre = pcre_compile2(pattern, options, &errcode,&errorptr, &erroffset, NULL);
#endif
    preg->re_erroffset = erroffset;
    if(preg->re_pcre == NULL){
        /* Internal ERR21 is "failed to get memory" according to pcreapi(3) */
        if(errcode == 21)
            return AP_REG_ESPACE;
        return AP_REG_INVARG;
    }
#ifdef HAVE_PCRE2
    /* todo: add function body when macro HAVE_PCRE2 defines */
#else
    pcre_fullinfo((const pcre *)preg->re_pcre,NULL, PCRE_INFO_CAPTURECOUNT, &(preg->re_nsub));
#endif
    return 0;
}

/*************************************************
 *           Free store held by a regex          *
 *************************************************/

AP_DECLARE(void) ap_regfree(ap_regex_t *preg){
#ifdef HAVE_PCRE2
    /* add function body when macro HAVE_PCRE2 defines */
#else
    (pcre_free)(preg->re_pcre);
#endif
}