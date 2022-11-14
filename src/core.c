//
// Created by kevin on 2022/10/17.
//
#include "stdlib.h"

#include "apr/apr_file_io.h"

#include "mod_core.h"
#include "http_log.h"
#include "http_core.h"
#include "ap_release.h"
#include "os/unix/os.h"
#include "mpm_common.h"


/* valid in core-conf, but not in runtime r->used_path_info */
#define AP_ACCEPT_PATHINFO_UNSET   3

/* LimitRequestBody handling */
#define AP_LIMIT_REQ_BODY_UNSET   ((apr_off_t) -1)
#define AP_DEFAULT_LIMIT_REQ_BODY ((apr_off_t) 1<< 30) /* 1GB */

/* LimitXMLRequestBody handling */
#define AP_LIMIT_UNSET            ((long) -1)
#define AP_DEFAULT_LIMIT_XML_BODY ((apr_size_t)1000000)

#define AP_FLUSH_MAX_THRESHOLD 65535
#define AP_FLUSH_MAX_PIPELINED 4

#ifdef WIN32
#define USE_ICASE AP_REG_ICASE
#else
#define USE_ICASE 0
#endif


static apr_random_t *rng = NULL;

AP_DECLARE_DATA int ap_main_state = AP_SQ_MS_INITIAL_STARTUP;
AP_DECLARE_DATA int ap_run_mode = AP_SQ_RM_UNKNOWN;
AP_DECLARE_DATA int ap_config_generation = 0;

static const char *server_description = NULL;

AP_CORE_DECLARE(void) ap_init_rng(apr_pool_t *p){
    unsigned char seed[8];
    apr_status_t rv;
    rng = apr_random_standard_new(p);
    do{
        rv = apr_generate_random_bytes(seed, sizeof (seed));
        if(rv != APR_SUCCESS)
            goto error;
        apr_random_add_entropy(rng, seed, sizeof (seed));
        rv = apr_random_insecure_ready(rng);
    }while( rv == APR_ENOTENOUGHENTROPY);
    if(rv == APR_SUCCESS)
        return;
    error:
        ap_log_error(APLOG_MARK, APLOG_CRIT, rv, NULL, APLOGNO(00141)"could not initialize random number generator");
        exit(1);
}

AP_DECLARE(const char *) ap_get_server_description(void){
    return server_description? server_description: AP_SERVER_BASEVERSION"(" PLATFORM ")";
}

static void *create_core_dir_config(apr_pool_t *a, char *dir){
    core_dir_config *conf;

    conf = (core_dir_config  *)apr_palloc(a, sizeof (core_dir_config));

    conf->opts = dir ? OPT_UNSET : OPT_UNSET | OPT_SYM_LINKS;
    conf->opts_add = conf->opts_remove = OPT_NONE;
    conf->override = OR_UNSET | OR_NONE;
    conf->override_opts = OPT_UNSET | OPT_ALL | OPT_SYM_OWNER | OPT_MULTI;

    conf->accept_path_info = AP_ACCEPT_PATHINFO_UNSET;

    conf->use_canonical_name = USE_CANONICAL_NAME_UNSET;
    conf->use_canonical_phys_port = USE_CANONICAL_PHYS_PORT_UNSET;
    conf->hostname_lookups = HOSTNAME_LOOKUP_UNSET;

    conf->limit_req_body = AP_LIMIT_REQ_BODY_UNSET;
    conf->limit_xml_body = AP_LIMIT_UNSET;

    conf->server_signature = srv_sig_unset;

    conf->add_default_charset = ADD_DEFAULT_CHARSET_UNSET;
    conf->add_default_charset_name = DEFAULT_ADD_DEFAULT_CHARSET_NAME;

    /* flag for use of inodes in ETags. */
    conf->etag_bits = ETAG_UNSET;
    conf->etag_add = ETAG_UNSET;
    conf->etag_remove = ETAG_UNSET;

    conf->enable_mmap = ENABLE_MMAP_UNSET;
    conf->enable_sendfile = ENABLE_SENDFILE_UNSET;
    conf->allow_encoded_slashes = 0;
    conf->decode_encoded_slashes = 0;

    conf->max_ranges = AP_MAXRANGES_UNSET;
    conf->max_overlaps = AP_MAXRANGES_UNSET;
    conf->max_reversals = AP_MAXRANGES_UNSET;

    conf->cgi_pass_auth = AP_CGI_PASS_AUTH_UNSET;
    conf->qualify_redirect_url = AP_CORE_CONFIG_UNSET;

    return (void *) conf;
}

static void *merge_core_dir_configs(apr_pool_t* a, void *basev, void *newv){
    core_dir_config *base = (core_dir_config *)basev;
    core_dir_config *new = (core_dir_config *) newv;
    core_dir_config *conf;

    /* create this conf by duplicating the base, replacing elements */
    conf = (core_dir_config *) apr_pmemdup(a, base, sizeof (core_dir_config));

    conf->d = new-> d;
    conf->d_is_fnmatch = new->d_is_fnmatch;
    conf->d_components = new->d_components;
    conf->r = new->r;
    conf->refs = new->refs;
    conf->condition = new->condition;

    if(new->opts & OPT_UNSET){
       /* there was no explict setting of new->opts, so we merge preserve invariant */
       conf->opts_add = (conf->opts_add & ~new->opts_remove) | new->opts_add;
       conf->opts_remove = (conf->opts_remove & ~new->opts_add) | new->opts_remove;
       conf->opts = (conf->opts & ~conf->opts_remove) | conf->opts_add;
       if(((base->opts & (OPT_INCLUDES  | OPT_INC_WITH_EXEC)) == (OPT_INCLUDES | OPT_INC_WITH_EXEC)) &&
          ((new->opts & (OPT_INCLUDES | OPT_INC_WITH_EXEC)) == OPT_INCLUDES)){
           conf->opts &= ~OPT_INC_WITH_EXEC;
       }
    }else{
        /* otherwise we just copy, since an explicit opts setting overrides all earlier +/- modifiers. */
        conf->opts = new->opts;
        conf->opts_add = new->opts_add;
        conf->opts_remove = new->opts_remove;
    }
    if(!(new->override & OR_UNSET)){
        conf->override = new->override;
    }
    if(!(new->override_opts & OPT_UNSET)){
        conf->override_opts = new->override_opts;
    }
    if(new->override_list !=NULL){
        conf->override_list = new->override_list;
    }
    if(conf->response_code_exprs == NULL){
        conf->response_code_exprs = new->response_code_exprs;
    }else if(new->response_code_exprs !=NULL){
        conf->response_code_exprs = apr_hash_overlay(a, new->response_code_exprs, conf->response_code_exprs);
    }
    /* otherwise we simply use base->response_code_exprs */
    if(new->hostname_lookups != HOSTNAME_LOOKUP_UNSET){
        conf->hostname_lookups = new->hostname_lookups;
    }
    if(new->accept_path_info != AP_ACCEPT_PATHINFO_UNSET){
        conf->accept_path_info = new->accept_path_info;
    }
    if(new->use_canonical_name != USE_CANONICAL_NAME_UNSET){
        conf->use_canonical_name = new->use_canonical_name;
    }
    if(new->use_canonical_phys_port != USE_CANONICAL_PHYS_PORT_UNSET){
        conf->use_canonical_phys_port = new->use_canonical_phys_port;
    }
#ifdef RLIMIT_CPU
    if(new->limit_cpu){
        conf->limit_cpu = new->limit_cpu;
    }
#endif
#if defined(RLIMIT_DATA) || defined(RLIMIT_VMEM) || defined(RLIMIT_AS)
    if(new->limit_mem){
        conf->limit_mem = new->limit_mem;
    }
#endif
#ifdef RLIMIT_NPROC
    if(new->limit_nproc){
        conf->limit_nproc = new->limit_nproc;
    }
#endif
    if(new->limit_req_body != AP_LIMIT_REQ_BODY_UNSET){
        conf->limit_req_body = new->limit_req_body;
    }
    if(new->limit_xml_body != AP_LIMIT_UNSET)
        conf->limit_xml_body = new->limit_xml_body;
    if(!conf->sec_file){
        conf->sec_file = new->sec_file;
    }else if(new->sec_file){
        conf->sec_file = apr_array_append(a, base->sec_file, new->sec_file);
    }
    if(!conf->sec_if){
        conf->sec_if= new->sec_if;
    }else if(new->sec_if){
        conf->sec_if = apr_array_append(a, base->sec_if, new->sec_if);
    }
    if(new->server_signature != srv_sig_unset){
        conf->server_signature = new->server_signature;
    }
    if(new->add_default_charset != ADD_DEFAULT_CHARSET_UNSET){
        conf->add_default_charset = new->add_default_charset;
        conf->add_default_charset_name = new->add_default_charset_name;
    }
    if(new->mime_type){
        conf->mime_type = new->mime_type;
    }
    if(new->handler){
        conf->handler = new->handler;
    }
    if(new->output_filters){
        conf->output_filters = new->output_filters;
    }
    if(new->input_filters){
        conf->input_filters = new->input_filters;
    }
    /*
     * now we merge the setting of fileETag directive
     */
    if(new->etag_bits == ETAG_UNSET){
        conf->etag_add = (conf->etag_add & (~ new->etag_remove)) | new->etag_add;
        conf->etag_remove = (conf->etag_remove & (~new->etag_add)) | new->etag_remove;
        conf->etag_bits = (conf->etag_bits & (~ conf->etag_remove)) | conf->etag_add;
    }else{
        conf->etag_bits = new->etag_bits;
        conf->etag_add = new->etag_add;
        conf->etag_remove = new->etag_remove;
    }

    if(conf->etag_bits != ETAG_NONE){
        conf->etag_bits &= (~ETAG_NONE);
    }
    if(conf->enable_mmap != ENABLE_MMAP_UNSET){
        conf->enable_mmap = new->enable_mmap;
    }
    if(new->enable_sendfile != ENABLE_SENDFILE_UNSET){
        conf->enable_sendfile = new->enable_sendfile;
    }
    if(new->read_buf_size){
        conf->read_buf_size = new->read_buf_size;
    }else{
        conf->read_buf_size = base->read_buf_size;
    }

    if(new->allow_encoded_slashes_set){
        conf->allow_encoded_slashes = new->allow_encoded_slashes;
    }
    if(new->decode_encoded_slashes_set){
        conf->decode_encoded_slashes = new->decode_encoded_slashes;
    }
    if(new->log){
        if(!conf->log){
            conf->log = new ->log;
        }else{
            conf->log = ap_new_log_config(a, new->log);
            ap_merge_log_config(base->log, conf->log);
        }
    }
    conf->max_ranges = new->max_ranges != AP_MAXRANGES_UNSET ? new->max_ranges : base->max_ranges;
    conf->max_overlaps = new->max_overlaps != AP_MAXRANGES_UNSET ? new->max_overlaps : base-> max_overlaps;
    conf->max_reversals = new->max_reversals != AP_MAXRANGES_UNSET ? new->max_reversals : base->max_reversals;

    conf->cgi_pass_auth = new->cgi_pass_auth != AP_CGI_PASS_AUTH_UNSET ? new->cgi_pass_auth : base->cgi_pass_auth;
    if(new->cgi_var_rules){
        if(!conf->cgi_var_rules){
            conf->cgi_var_rules = new->cgi_var_rules;
        }else{
            conf->cgi_var_rules = apr_hash_overlay(a, new->cgi_var_rules, conf->cgi_var_rules);
        }
    }
    AP_CORE_MERGE_FLAG(qualify_redirect_url, conf, base, new);
    return (void*) conf;
}

static void* create_core_server_config(apr_pool_t *a, server_rec *s){
    core_server_config *conf;
    int is_virtual = s->is_virtual;

    /* palloc memory of core_server_config */
    conf = (core_server_config *) apr_palloc(a, sizeof (core_server_config));

    /* global default / global-only settings */
    if(!is_virtual){
        conf->ap_document_root = DOCUMENT_LOCATION;
        conf->access_name = DEFAULT_ACCESS_FNAME;

        /* A mapping only make sense in global context */
        conf->accf_map = apr_table_make(a, 5);
#if APR_HAS_SO_ACCEPTFILTER
/* todo add body when macro APR_HAS_SO_ACCEPTFILTER is true */
#elif defined(WIN32)
/* todo add body when macro WIN32 is true */
#else
        apr_table_setn(conf->accf_map, "http", "data");
        apr_table_setn(conf->accf_map, "https", "data");
#endif
        conf->flush_max_threshold = AP_FLUSH_MAX_THRESHOLD;
        conf->flush_max_pipelined = AP_FLUSH_MAX_PIPELINED;
    }else{
        conf->flush_max_pipelined = -1;
    }
    /* initialization, no special case for global context */
    conf->sec_dir = apr_array_make(a, 40, sizeof (ap_conf_vector_t *));
    conf->sec_url = apr_array_make(a, 40, sizeof (ap_conf_vector_t *));

    conf->trace_enable = AP_TRACE_UNSET;
    conf->protocols = apr_array_make(a, 5, sizeof (const char *));
    conf->protocols_honor_order = -1;
    conf->async_filter = 0;
    conf->strict_host_check = AP_CORE_CONFIG_UNSET;
    conf->merge_slashes     = AP_CORE_CONFIG_UNSET;

    return (void *) conf;
}

static void *merge_core_server_configs(apr_pool_t *p, void *basev, void * virtv){
    core_server_config *base = (core_server_config *)basev;
    core_server_config *virt = (core_server_config *)virtv;
    /* pmemdup new memory as core_sever_config merged by base and virt */
    core_server_config *conf = (core_server_config *) apr_pmemdup(p, base, sizeof (core_server_config));

    if(virt->ap_document_root)
        conf->ap_document_root = virt->ap_document_root;
    if(virt->access_name)
        conf->access_name = virt->access_name;

    /* XXX optimize to keep base->sec_ pointers if virt->sec_ array is empty */
    conf->sec_dir = apr_array_append(p, base->sec_dir, virt->sec_dir);
    conf->sec_url = apr_array_append(p, base->sec_url, virt->sec_url);

    if(virt->redirect_limit)
        conf->redirect_limit = virt->redirect_limit;
    if(virt->subreq_limit)
        conf->subreq_limit = virt->subreq_limit;
    if(virt->trace_enable != AP_TRACE_UNSET)
        conf->trace_enable = virt->trace_enable;
    if(virt->http09_enable != AP_HTTP09_UNSET)
        conf->http09_enable = virt->http09_enable;
    if(virt->http_conformance != AP_HTTP_CONFORMANCE_UNSET)
        conf->http_conformance = virt->http_conformance;
    if(virt->http_methods != AP_HTTP_METHODS_UNSET)
        conf->http_methods = virt->http_methods;
    if(virt->http_cl_head_zero != AP_HTTP_EXPECT_STRICT_UNSET)
        conf->http_expect_strict = virt->http_expect_strict;

    /* no action for virt->accf_map, not allowed per-vhost */

    if(virt->protocol)
        conf->protocol = virt->protocol;
    if(virt->gprof_dir)
        conf->gprof_dir = virt->gprof_dir;
    if(virt->error_log_format)
        conf->error_log_format = virt->error_log_format;
    if(virt->error_log_conn)
        conf->error_log_conn = virt->error_log_conn;
    if(virt->error_log_req)
        conf->error_log_req = virt->error_log_req;

    if(virt->conn_log_level){
        if(!conf->conn_log_level){
            conf->conn_log_level = virt->conn_log_level;
        }else{
            conf->conn_log_level = apr_array_append(p, conf->conn_log_level, virt->conn_log_level);
        }
    }

    conf->merge_trailers = (virt->merge_trailers != AP_MERGE_TRAILERS_UNSET)?
                            virt->merge_trailers:base->merge_trailers;
    conf->protocols = ((virt->protocols->nelts > 0))? virt->protocols : base->protocols;
    conf->protocols_honor_order = ((virt->protocols_honor_order < 0)?
                                    base->protocols_honor_order:virt->protocols_honor_order);
    conf->async_filter = ((virt->async_filter_set) ? virt->async_filter : base->async_filter);
    conf->async_filter_set = base->async_filter_set || virt->async_filter_set;
    conf->flush_max_threshold = (virt->flush_max_threshold) ? virt->flush_max_threshold: base->flush_max_pipelined;
    conf->strict_host_check = (virt->strict_host_check != AP_CORE_CONFIG_UNSET) ?
                               virt->strict_host_check : base->strict_host_check;
    AP_CORE_MERGE_FLAG(strict_host_check, conf, base, virt);
    AP_CORE_MERGE_FLAG(merge_slashes, conf, base, virt);
    return (void *)conf;

}

/* return a parent if it matches given directives */
static const ap_directive_t *find_parent(const ap_directive_t *dirp, const char *what){
    while(dirp->parent !=NULL) {
        dirp= dirp->parent;
        /* ### it would be nice to have atom-ized directives */
        if(ap_cstr_casecmp(dirp->directive, what) == 0)
            return dirp;
    }
    return NULL;
}

AP_DECLARE(const char *) ap_check_cmd_context(cmd_parms *cmd, unsigned forbidden){
    const char *gt = (cmd->cmd->name[0] == '<' && cmd->cmd->name[strlen(cmd->cmd->name)-1] != '>')? ">" : "";
    const ap_directive_t *found;

    if((forbidden & NOT_IN_VIRTUALHOST) && cmd->server->is_virtual){
        return apr_pstrcat(cmd->pool, cmd->cmd->name, gt, " cannot occur within <VirtualHost> section", NULL);
    }

    if((forbidden & NOT_IN_DIR_CONTEXT) && cmd->limited != -1){
        return apr_pstrcat(cmd->pool, cmd->cmd->name, gt, " cannot occur within <Limit> or <LimitExcept> section", NULL);
    }

    if((forbidden & NOT_IN_HTACCESS) && (cmd->pool == cmd->temp_pool)){
        return apr_pstrcat(cmd->pool, cmd->cmd->name, gt, " cannot occur within htaccess files", NULL);
    }

    if((forbidden & NOT_IN_DIR_CONTEXT) == NOT_IN_DIR_CONTEXT){
        if(cmd->path != NULL){
            return apr_pstrcat(cmd->pool, cmd->cmd->name, gt, " cannot occur within directory context", NULL);
        }
        if(cmd->cmd->req_override & EXEC_ON_READ){
            return NULL;
        }
    }

    if (((forbidden & NOT_IN_DIRECTORY)
       && ((found = find_parent(cmd->directive, "<Directory"))
       || (found = find_parent(cmd->directive, "<DirectoryMatch"))))
       || ((forbidden & NOT_IN_LOCATION)
       && ((found = find_parent(cmd->directive, "<Location"))
       || (found = find_parent(cmd->directive, "<LocationMatch"))))
       || ((forbidden & NOT_IN_FILES)
       && ((found = find_parent(cmd->directive, "<Files"))
       || (found = find_parent(cmd->directive, "<FilesMatch"))
       || (found = find_parent(cmd->directive, "<If"))
       || (found = find_parent(cmd->directive, "<ElseIf"))
       || (found = find_parent(cmd->directive, "<Else"))))
       || ((forbidden & NOT_IN_PROXY)
       && ((found = find_parent(cmd->directive, "<Proxy"))
       || (found = find_parent(cmd->directive, "<ProxyMatch"))))) {
           return apr_pstrcat(cmd->pool, cmd->cmd->name, gt, " cannot occur within ", found->directive,
                              "> section", NULL);
    }

}

/* report a missing '>' syntax error */
static char *unclosed_directive(cmd_parms *cmd){
    return apr_pstrcat(cmd->pool, cmd->cmd->name, "> directive missing closing '>'", NULL);
}

static char *missing_container_arg(cmd_parms *cmd){
    return apr_pstrcat(cmd->pool, cmd->cmd->name, "> directive requires additional arguments", NULL);
}

AP_DECLARE(int) ap_state_query(int query){
    switch (query) {
        case AP_SQ_MAIN_STATE:
            return ap_main_state;
        case AP_SQ_RUN_MODE:
            return ap_run_mode;
        case AP_SQ_CONFIG_GEN:
            return ap_config_generation;
        default:
            return AP_SQ_NOT_SUPPORTED;
    }
}
static const char *dirsection(cmd_parms *cmd, void *mconfig, const char *arg){
    const char *errmsg;
    const char *endp = ap_strrchr_c(arg, '>');
    int old_overrides = cmd->override;
    char *old_path = cmd->path;
    core_dir_config  *conf;
    ap_conf_vector_t *new_dir_conf = ap_create_per_dir_config(cmd->pool);
    ap_regex_t *r = NULL;
    const command_rec *thiscmd = cmd->cmd;

    const char *err = ap_check_cmd_context(cmd, NOT_IN_DIR_CONTEXT);
    if(err != NULL) return NULL;

    if(endp == NULL) return unclosed_directive(cmd);

    arg = apr_pstrndup(cmd->temp_pool, arg, endp - arg);

    if(!arg[0]){
        return missing_container_arg(cmd);
    }

    cmd->path = ap_getword_conf(cmd->pool, &arg);
    cmd->override = OR_ALL | ACCESS_CONF;

    if(!strcmp(cmd->path, "~")){
        cmd->path = ap_getword_conf(cmd->pool, &arg);
        if(!cmd ->path) return "<Directory ~ > block must specify a path";
        r = ap_pregcomp(cmd->pool, cmd->path, AP_REG_EXTENDED | USE_ICASE);
        if(!r){
            return "Regex could not be compiled";
        }
    }else if(thiscmd->cmd_data){
        r = ap_pregcomp(cmd->pool, cmd->path, AP_REG_EXTENDED|USE_ICASE);
        if(!r){
            return "Regex could not be compiled";
        }
    }else if(strcmp(cmd->path, "/") != 0){
        int run_mode = ap_state_query(AP_SQ_RUN_MODE);
        char *newpath;

        apr_status_t rv = apr_filepath_merge(&newpath, NULL, cmd->path, APR_FILEPATH_TRUENAME, cmd->pool);
        if(rv != APR_SUCCESS && rv != APR_EPATHWILD){
            return apr_pstrcat(cmd->pool, "<Directory \"", cmd->path, "\"> path is invalid.", NULL);
        }

        if(run_mode == AP_SQ_RM_CONFIG_TEST && !ap_is_directory(cmd->temp_pool, cmd->path)){
            ap_log_error(APLOG_MARK, APLOG_STARTUP, 0, cmd->temp_pool, APLOGNO(10234),
                         "Warning: <Directory \"%s\"> does not exist or is not a directory", cmd->path);
        }
        cmd->path = newpath;
        if(cmd->path[strlen(cmd->path)-1] != '/')
            cmd->path = apr_pstrcat(cmd->pool, cmd->path, "/", NULL);
    }

    /* initialize our config and fetch it */
    conf = ap_set_config_vectors(cmd->server, new_dir_conf, cmd->path, &core_module, cmd->pool);

    errmsg = ap_walk_config(cmd->directive->first_child, cmd, new_dir_conf);

}

static const command_rec core_cmds[] = {
    AP_INIT_RAW_ARGS("<Directory", dirsection, NULL, RSRC_CONF,
                     "Container for directive affecting resources located in specified directories"),
};


AP_DECLARE_MODULE(core) = {
        MPM20_MODULE_STUFF,
        AP_PLATFORM_REWRITE_ARGS_HOOK, /* hook to run before apache parses args */
        create_core_dir_config,       /* create per-directory config structure */
        merge_core_dir_configs,       /* merge per-directory config structures */
        create_core_server_config,    /* create per-server config structure */
        merge_core_server_configs,    /* merge per-server config structures */
        core_cmds,                    /* command apr_table_t */
        register_hooks                /* register hooks */
};
