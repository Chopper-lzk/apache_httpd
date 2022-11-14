//
// Created by kevin on 2022/11/7.
//

#ifndef HTTPSERVER_UTIL_EXPR_PRIVATE_H
#define HTTPSERVER_UTIL_EXPR_PRIVATE_H

/** The operations in a parse tree node */
typedef enum {
    op_NOP,
    op_True, op_False,
    op_Not, op_Or, op_And,
    op_Comp,
    op_EQ, op_NE, op_LT, op_LE, op_GT, op_GE, op_IN,
    op_REG, op_NRE,
    op_STR_EQ, op_STR_NE, op_STR_LT, op_STR_LE, op_STR_GT, op_STR_GE,
    op_Concat,
    op_String, op_Word,
    op_Digit, op_Var, op_Bool, op_ListElement,
    op_Sub, op_Split, op_Join,
    op_Regex, op_Backref,
    /*
     * call external functions/operators.
     * The info node contains the function pointer and some function specific
     * info.
     * For Binary operators, the Call node links to the Info node and the
     * Args node, which in turn links to the left and right operand.
     * For all other variants, the Call node links to the Info node and the
     * argument.
     */
    op_UnaryOpCall, op_UnaryOpInfo,
    op_BinaryOpCall, op_BinaryOpInfo, op_BinaryOpArgs,
    op_StringFuncCall, op_StringFuncInfo,
    op_ListFuncCall, op_ListFuncInfo
} ap_expr_node_op_e;


/** the basic parse tree node */
struct ap_expr_node{
    ap_expr_node_op_e node_op;
    const void *node_arg1;
    const void *node_arg2;
};

#endif //HTTPSERVER_UTIL_EXPR_PRIVATE_H
