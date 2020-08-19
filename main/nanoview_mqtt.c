#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "driver/uart.h"


#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "mqtt_client.h"

#include "nanoview_uart.h"
#include "nanoview_main.h"
#include "nanoview_mqtt.h"


static const char *TAG = "MQTT_TASK";

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            
            break;
     
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

static esp_mqtt_client_handle_t nv_mqtt_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = BROKER_URL,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
    return client;
}

void nv_mqtt_task(struct task_config *tc) {
    esp_mqtt_client_handle_t mqtt_client = nv_mqtt_start();
    char *publish_string = NULL;
    while(true) {
        if(xQueueReceive(tc->xMqttQueue, &publish_string, (TickType_t)(250 / portTICK_RATE_MS)) == pdPASS) {            
            if (publish_string != NULL) {                
                int msg_id = esp_mqtt_client_publish(mqtt_client, "/esp-mqtt/nanoview", publish_string, 0, 1, 0);
                free(publish_string);
                publish_string = NULL;
            }
        }
        //vTaskDelay((1000 / portTICK_RATE_MS));
    }
}