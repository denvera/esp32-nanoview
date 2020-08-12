/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "linenoise/linenoise.h"
#include "argtable3/argtable3.h"
#include "console.h"
#include "sdkconfig.h"
#include "nanoview_uart.h"

static const char* TAG = "esp32-nanoview";
#define VERSION "0.1"

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

    initialize_console();
    nv32_init_uart();

    QueueHandle_t xNvMessageQueue, xMqttQueue;

    xTaskCreate((TaskFunction_t)console_task, "console_task", 1024*4, NULL, configMAX_PRIORITIES, NULL);        
    xTaskCreate((TaskFunction_t)nv32_rx_task, "uart_rx_task", 1024*4, NULL, configMAX_PRIORITIES, NULL);
}
