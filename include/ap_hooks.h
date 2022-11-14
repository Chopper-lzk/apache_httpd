//
// Created by kevin on 2022/10/20.
//

#ifndef HTTPSERVER_AP_HOOKS_H
#define HTTPSERVER_AP_HOOKS_H

#include "apr/apr_hooks.h"

#define AP_DECLARE_HOOK(ret, name, args)  \
        APR_DECLARE_EXTERNAL_HOOK(ap, AP, ret, name, args)

#define AP_IMPLEMENT_HOOK_VOID(name, args_decl, args_use) \
        APR_IMPLEMENT_EXTERNAL_HOOK_VOID(ap, AP, name, args_decl, args_use)

#define AP_IMPLEMENT_HOOK_RUN_FIRST(ret, name, args_decl, args_use, decline) \
        APR_IMPLEMENT_EXTERNAL_HOOK_RUN_FIRST(ap,AP, ret, name, args_decl, args_use, decline ) \

#define AP_IMPLEMENT_HOOK_RUN_ALL(ret, name, args_decl, args_use, ok, decline) \
        APR_IMPLEMENT_EXTERNAL_HOOK_RUN_ALL(ap,AP, ret, name, args_decl, args_use, ok, decline) \

#endif //HTTPSERVER_AP_HOOKS_H
