//
// Created by kevin on 2022/11/7.
//

#ifndef HTTPSERVER_AP_EXPR_H
#define HTTPSERVER_AP_EXPR_H

/** a node in expression parse tree */
typedef struct ap_expr_node ap_expr_t;

/* struct describing a parsed expression */
typedef struct{
    /** root of actual expression parse tree */
    ap_expr_t *root_node;

    /** filename where expression has been defined */
    const char *file_name;
    /** line number where expression has been defined */
    unsigned int line_number;
    /** flags relevant for expression */
    unsigned int flags;
    /** module that is used for loglevel configuration */
    int module_index;
} ap_expr_info_t;

#endif //HTTPSERVER_AP_EXPR_H
