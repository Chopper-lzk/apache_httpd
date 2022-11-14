//
// Created by kevin on 2022/10/14.
//

#include "http_request.h"

static int auth_internal_per_conf_hooks = 0;
static int auth_internal_per_conf_providers = 0;

AP_DECLARE(void) ap_clear_auth_internal(void){
    auth_internal_per_conf_hooks = 0;
    auth_internal_per_conf_providers = 0;
}