//
// Created by kevin on 2022/10/12.
//

#ifndef HTTPSERVER_UTIL_CFGTREE_H
#define HTTPSERVER_UTIL_CFGTREE_H

#ifdef __cplusplus
extern "C"{
#endif

    typedef struct ap_directive_t ap_directive_t;

    /**
     * @brief structure used to build config tree
     */
    struct ap_directive_t{
        /** the current directive. */
        const char *directive;

        /** arguments for current directive */
        const char *args;

        /** next directive node in tree */
        ap_directive_t *next;

        /** first child node of this directive */
        ap_directive_t *first_child;

        /** parent node of this directive */
        ap_directive_t *parent;

        /** store data here */
        void *data;

        /** name of file this directive found in */
        const char *filename;

        /** line number this directive was on */
        int line_num;

        /** pointer to last directive node in tree */
        ap_directive_t *last;
    };

#ifdef __cplusplus
}
#endif

#endif //HTTPSERVER_UTIL_CFGTREE_H
