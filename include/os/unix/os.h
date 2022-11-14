//
// Created by kevin on 2022/11/3.
//

#ifndef HTTPSERVER_OS_H
#define HTTPSERVER_OS_H

#ifndef PLATFORM
#define PLATFORM "Unix"
#endif

#define AP_PLATFORM_REWRITE_ARGS_HOOK ap_mpm_rewrite_args

#endif //HTTPSERVER_OS_H
