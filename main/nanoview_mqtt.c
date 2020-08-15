#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "nanoview_uart.h"
#include "nanoview_main.h"
#include "nanoview_mqtt.h"


void nv_mqtt_task(struct task_config *tc) {
    while(true) {
            //if(xQueueReceive(tc->xMqttQueue, &p, (TickType_t)(250 / portTICK_RATE_MS)) == pdPASS) {
            //}
        vTaskDelay((1000 / portTICK_RATE_MS));
    }
}