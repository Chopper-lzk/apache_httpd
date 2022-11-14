//
// Created by kevin on 2022/10/17.
//
#include "stdlib.h"

#include "apr/apr_strings.h"

#include "httpd.h"
#include "ap_mmn.h"
#include "http_config.h"
#include "http_log.h"

#define APLOG_UNSET    (APLOG_NO_MODULE - 1 )

APR_DECLARE_DATA const char *ap_server_argv0 = NULL;
AP_DECLARE_DATA server_rec *ap_server_conf = NULL;
APR_DECLARE_DATA apr_pool_t *ap_pglobal = NULL;

AP_DECLARE_DATA apr_array_header_t  *ap_server_pre_read_config = NULL;
AP_DECLARE_DATA apr_array_header_t  *ap_server_post_read_config = NULL;
AP_DECLARE_DATA apr_array_header_t  *ap_server_config_defines = NULL;

AP_DECLARE_DATA module *ap_top_module = NULL;
AP_DECLARE_DATA module **ap_loaded_modules = NULL;

static apr_hash_t *ap_config_hash = NULL;

static int conf_vector_length = 0;

/** a list of module symbol names */
static char **ap_module_short_names = NULL;

typedef struct ap_mod_list_struct ap_mod_list;

struct ap_mod_list_struct{
    struct ap_mod_list_struct *next;
    module *m;
    const command_rec *cmd;
};

AP_DECLARE(const char *) ap_find_module_short_name(int module_index){
    if(module_index < 0 || module_index >= conf_vector_length) return NULL;
    return ap_module_short_names[module_index];
}
static void rebuild_conf_hash(apr_pool_t *p, int add_prelinked);

static void ap_add_module_commands(module *m, apr_pool_t *p){
    apr_pool_t *tpool;
    ap_mod_list *mln;
    const command_rec *cmd;
    char *dir;

    cmd = m->cmds;
    if(ap_config_hash == NULL){
        rebuild_conf_hash(p, 0);
    }
    tpool = apr_hash_pool_get(ap_config_hash);

    while(cmd && cmd-> name){
        mln = apr_palloc(tpool, sizeof (ap_mod_list));
        mln -> cmd = cmd;
        mln -> m = m;
        dir = apr_pstrdup(tpool, cmd->name);

        ap_str_tolower(dir);

        mln->next = apr_hash_get(ap_config_hash, dir, APR_HASH_KEY_STRING);
        apr_hash_set(ap_config_hash, dir, APR_HASH_KEY_STRING, mln);
        ++cmd;
    }
}

static void rebuild_conf_hash(apr_pool_t *p, int add_prelinked){
    module **m;

    ap_config_hash = apr_hash_make(p);

    apr_pool_cleanup_register(p, &ap_config_hash, ap_pool_cleanup_set_null, apr_pool_cleanup_null);

    if(add_prelinked){
        for(m = ap_prelinked_modules; *m != NULL; m++){
            ap_add_module_commands(*m, p);
        }
    }
}

typedef void* (*merger_func)(apr_pool_t *, void *, void *);

/* dynamic_modules is number of modules that have been added after pre-loaded ones have been set up */
static int dynamic_modules = 0;

/* total_modules is the number of modules that have been linked into server */
static int total_modules = 0;

/* maximum possible value for total_modules */
static int max_modules = 0;

/* a list of merge_dir_config functions */
static merger_func *merger_func_cache;

AP_DECLARE(const char *) ap_setup_prelinked_modules(process_rec *process){
    module **m;
    module **m2;
    const char *error;

    apr_hook_global_pool = process -> pool;
    rebuild_conf_hash(process ->pconf, 0);

    /* Initialize total_modules variable and module indices */
    total_modules =0;
    for(m = ap_preloaded_modules;m != NULL; m++){
        (*m)->module_index = total_modules++;
    }

    max_modules = total_modules + DYNAMIC_MODULE_LIMIT +1;
    conf_vector_length = max_modules;

    /* Initialize list of loaded modules and short names */
    ap_loaded_modules = (module **) apr_palloc(process->pool, sizeof(module *) * conf_vector_length);
    if(!ap_module_short_names)
        ap_module_short_names = ap_calloc(sizeof (char *), conf_vector_length);

    if(!merger_func_cache)
        merger_func_cache = ap_calloc(sizeof (merger_func *), conf_vector_length);

    if(ap_loaded_modules == NULL || ap_module_short_names == NULL || merger_func_cache == NULL)
        return "Oops! Out of memory in ap_setup_prelinked_modules()!";

    for(m = ap_preloaded_modules, m2 = ap_loaded_modules; *m!=NULL;)
        *m2++ = *m++;
    *m2 = NULL;

    /* Initialize chain of linked modules */
    for(m = ap_prelinked_modules; *m !=NULL; m++){
        error = ap_add_module(*m, process->pconf, NULL);
        if(error) return error;
    }
    apr_hook_sort_all();
    return NULL;
}

AP_DECLARE(void) ap_register_hooks(module *m, apr_pool_t *p){
    if(m->register_hooks){
        if(getenv("SHOW_HOOKS")){
            printf("Registering hooks for %s\n", m->name);
            apr_hook_debug_enabled = 1;
        }
        apr_hook_debug_current = m -> name;
        m->register_hooks(p);
    }
}

AP_DECLARE(const char *) ap_add_module(module *m, apr_pool_t *p, const char * sym_name){
    ap_module_symbol_t *sym = ap_prelinked_module_symbols;

    if(m -> version != MODULE_MAGIC_NUMBER_MAJOR){
        return apr_psprintf(p, "Module \"%s\" is not compatible with this version "
                               "of Apach (found %d, need %d), please contact vendor for the"
                               "correct version.", m->name, m -> version, MODULE_MAGIC_NUMBER_MAJOR);
    }

    if(m->module_index == -1){
        if(dynamic_modules >= DYNAMIC_MODULE_LIMIT){
            return apr_psprintf(p, "Module \"%s\" could not be loaded because dynamic module"
                                   "limit was reached. Please increase DYNAMIC_MODULE_LIMIT and recompile.", m->name);
        }
        ap_assert(total_modules<conf_vector_length);

        m->module_index = total_modules ++;
        dynamic_modules++;
    }else if(!sym_name){
        while(sym->modp != NULL){
            if(sym -> modp == m){
                sym_name = sym->name;
                break;
            }
            sym++;
        }
    }

    if(m -> next ==NULL){
        m ->next = ap_top_module;
        ap_top_module = m;
    }

    if(sym_name){
        int len = strlen(sym_name);
        int slen = strlen("_module");
        if(len > slen && !strcmp(sym_name + len - slen, "_module")){
            len -= slen;
        }

        ap_module_short_names[m -> module_index] = ap_malloc(len+1);
        memcpy(ap_module_short_names[m -> module_index], sym_name, len);
        ap_module_short_names[m->module_index][len] = '\0'; /* end mark */
        merger_func_cache[m->module_index] = m -> merge_dir_config;
    }
    if(ap_strrchr_c(m->name, '/'))
        m->name = 1+ ap_strrchr_c(m->name, '/');
    if(ap_strrchr_c(m->name, '\\'))
        m->name = 1+ ap_strrchr_c(m->name, '\\');
#ifdef _OSD_POSIX
/* todo: add body when macro _OSD_POSIX defines */
#endif
    ap_add_module_commands(m, p);

    ap_register_hooks(m, p);
    return NULL;
}

AP_DECLARE(void) ap_run_rewrite_args(process_rec *process){
    module *m;
    for(m = ap_top_module; m; m=m->next){
        if(m->rewrite_args){
            (*m->rewrite_args)(process);
        }
    }
}

AP_DECLARE(struct ap_logconf *) ap_new_log_config(apr_pool_t *p, const struct ap_logconf *old){
    struct ap_logconf *l = apr_palloc(p, sizeof (struct ap_logconf));
    if(old){
        l->level = old->level;
        if(old->module_levels){
            l->module_levels = apr_pmemdup(p, old->module_levels, conf_vector_length);
        }
    }else{
        l->level = APLOG_UNSET;
    }
    return l;
}

AP_DECLARE(void) ap_merge_log_config(const struct ap_logconf *old_conf, struct ap_logconf *new_conf){
    if(new_conf->level != APLOG_UNSET){
        /* setting the main loglevel resets all per-module log levels. */
        return;
    }
    new_conf->level =old_conf->level;
    if(new_conf->module_levels == NULL){
        new_conf->module_levels = old_conf->module_levels;
    }else if(old_conf->module_levels != NULL){
        int i;
        for(i = 0; i< conf_vector_length; i++){
            if(new_conf -> module_levels[i] == APLOG_UNSET)
                new_conf->module_levels[i] = old_conf->module_levels[i];
        }
    }
}

static ap_conf_vector_t * create_empty_config(apr_pool_t *p){
    void *conf_vector = apr_pcalloc(p, sizeof (void *) * conf_vector_length);
    return conf_vector;
}
AP_CORE_DECLARE(ap_conf_vector_t *) ap_create_per_dir_config(apr_pool_t *p){
    return create_empty_config(p);
}

AP_CORE_DECLARE(void *) ap_set_config_vectors(server_rec *server, ap_conf_vector_t *section_vector, const char *section,
                                              module *mod, apr_pool_t *pconf){
    void *section_config = ap_get_module_config(section_vector, mod);
    void *server_config = ap_get_module_config(server->module_config, mod);
    if(!section_vector && mod->create_dir_config){
        section_config = (*mod->create_dir_config)(pconf, (char *)section_vector);
        ap_set_module_config(section_vector, mod,section_config);
    }
    if(!server_config && mod->create_server_config){
        server_config = (*mod->create_server_config)(pconf, server);
        ap_set_module_config(server->module_config, mod, server_config);
    }
    return section_vector;
}

static const char *invoke_cmd(const command_rec *cmd, cmd_parms *parms, void *mconfig, const char *args,
                              ap_directive_t *parent){
    int override_list_ok = 0;
    char *w, *w2, *w3;
    const char *errmsg = NULL;

    /* have we been provided a list of acceptable directives ? */
    if(parms->override_list !=NULL){
        if(apr_table_get(parms->override_list, cmd->name) != NULL){
            override_list_ok = 1;
        }
    }

    if((parms->override & cmd->req_override) == 0 && !override_list_ok){
        if(parms->override & NONFATAL_OVERRIDE){
            ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, parms->temp_pool, APLOGNO(02295)
                          "%s in .htaccess forbidden by AllowOverride", cmd->name);
            return NULL;
        }else if(parms->directive && parms->directive->parent){
            return apr_pstrcat(parms->pool, cmd->name, "not allowed in ", parms->directive->parent->directive, ">",
                               " context", NULL);
        }else{
            return apr_pstrcat(parms->pool, cmd->name, " not allowed here", NULL);
        }
    }

    parms ->info = cmd->cmd_data;
    parms ->cmd = cmd;
    parms ->parent = parent;

    switch (cmd->args_how) {
        case RAW_ARGS:
#ifdef RESOLVE_ENV_PER_TOKEN
            args = ap_resolve_env(parms->pool, args);
#endif
            return cmd->AP_RAW_ARGS(parms, mconfig, args);
    }
}

static const char *ap_walk_config_sub(const ap_directive_t *current, cmd_parms *parms, ap_conf_vector_t *section_vector)
{
    const command_rec *cmd;
    ap_mod_list *ml;
    char *dir = apr_pstrdup(parms->temp_pool, current->directive);

    ap_str_tolower(dir);

    ml = apr_hash_get(ap_config_hash, dir, APR_HASH_KEY_STRING);

    if(ml == NULL){
        parms ->err_directive = current;
        if(parms ->override &  NONFATAL_UNKNOWN){
            ap_log_perror(APLOG_MARK, APLOG_WARNING, 0, parms->temp_pool, APLOGNO(02296) "Unknown directive %s "
                          "perhaps misspelled or defined by a module not included in the server configuration", dir);
            return NULL;
        }else{
            return apr_pstrcat(parms->pool, "Invalid command '", current->directive,
                               "', perhaps misspelled or defined by a module not included in the server configuration",
                               NULL);
        }
    }

    for(;ml != NULL; ml = ml->next){
        void *dir_config = ap_set_config_vectors(parms->server, section_vector, parms->path, ml->m, parms->pool);
        const char *retval;
        cmd = ml->cmd;
        if(cmd->req_override & EXEC_ON_READ){
            continue;
        }
        retval = invoke_cmd(cmd, parms, dir_config, current->args, NULL);
    }
}

AP_DECLARE(const char *) ap_walk_config(ap_directive_t *current, cmd_parms *parms,
                                              ap_conf_vector_t *section_vector){
    ap_conf_vector_t *oldconfig = parms->context;
    parms->context = section_vector;

    for(; current !=NULL; current= current->next){
        const char *errmsg;
        parms->directive = current;
        errmsg = ap_walk_config_sub(current, parms, section_vector);
    }
}