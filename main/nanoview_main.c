
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_task.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "console.h"
#include "sdkconfig.h"
#include "nanoview_uart.h"
#include "nanoview_processor.h"
#include "nanoview_commands.h"
#include "nanoview_mqtt.h"
#include "nanoview_main.h"
#include "wifi.h"

static const char* TAG = "esp32-nanoview";
#define VERSION "0.1"

bool NV_MONITOR = false;

void app_main()
{
    printf("ESP32 NanoView v%s\n", VERSION);
        
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("Running on %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);


    initialize_console();
    register_debug();
    register_monitor();
    nv_init_uart();

    static struct task_config tc = {
            .monitor         = false,
            .debug           = false
    };

    tc.xNvMessageQueue = xQueueCreate(10, sizeof(struct nv_packet));
    tc.xMqttQueue      = xQueueCreate(10, sizeof(char *));

    if (tc.xNvMessageQueue == 0 || tc.xMqttQueue == 0) {
            ESP_LOGE(TAG, "Error creating queue!");
            esp_restart();
    }
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    xTaskCreate((TaskFunction_t)console_task,      "console_task",   1024*4, &tc, configMAX_PRIORITIES, NULL);
    xTaskCreate((TaskFunction_t)nv_mqtt_task,      "mqtt_task",      1024*8, &tc, configMAX_PRIORITIES, NULL);
    xTaskCreate((TaskFunction_t)nv_processor_task, "processor_task", 1024*8, &tc, configMAX_PRIORITIES, NULL);
    xTaskCreate((TaskFunction_t)nv_rx_task,        "uart_rx_task",   1024*4, &tc, configMAX_PRIORITIES, NULL);

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
 
    wifi_init_sta();
    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    while(true) {
        EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY);

        if (bits & WIFI_CONNECTED_BIT) {
                ESP_LOGI(TAG, "connected to ap SSID:%s",
                        ESP_WIFI_SSID);
                init_sntp();

        } else if (bits & WIFI_FAIL_BIT) {
                ESP_LOGI(TAG, "Failed to connect to SSID:%s",
                        ESP_WIFI_SSID);
                vTaskDelay((30000 / portTICK_RATE_MS));
                esp_restart();
        } else {
                ESP_LOGE(TAG, "UNEXPECTED EVENT");
        }
    }

}
