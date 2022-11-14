//
// Created by kevin on 2022/10/17.
//

#ifndef HTTPSERVER_AP_MPM_H
#define HTTPSERVER_AP_MPM_H

#include "ap_hooks.h"


#ifdef GPROF
extern void moncontrol(int);
#define AP_MONCONTROL(x) moncontrol(x)
#else
#define AP_MONCONTROL(x)
#endif


/** MPM can do threading */
#define AP_MPMQ_IS_THREADED       2


/** @defgroup mpmq MPM query
 * @{
 */

/** @defgroup thrdfrk Subtypes/Values returned for AP_MPMQ_IS_THREADED and AP_MPMQ_IS_FORKED
 *  @ingroup mpmq
 *  @{
 */
#define AP_MPMQ_NOT_SUPPORTED        0

#endif //HTTPSERVER_AP_MPM_H
