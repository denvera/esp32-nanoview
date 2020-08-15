#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/task.h"
#include "freertos/queue.h"

extern bool NV_MONITOR;

struct task_config {
    QueueHandle_t xNvMessageQueue;
    QueueHandle_t xMqttQueue;
    bool monitor;
    bool debug;
};



#ifdef __cplusplus
}
#endif