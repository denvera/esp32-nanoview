#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_NV_CONFIG_INCLUDE
#define BROKER_URL CONFIG_BROKER_URL
#else
#include "nanoview_config.h"
#endif

void nv_mqtt_task(struct task_config *tc);

#ifdef __cplusplus
}
#endif