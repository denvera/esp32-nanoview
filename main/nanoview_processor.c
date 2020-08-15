#include <stdint.h>
#include <sys/cdefs.h>
#include <cJSON.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nanoview_uart.h"
#include "nanoview_main.h"
#include "nanoview_processor.h"

#define PROCESSOR_TASK_TAG "NV_PROCESSOR"

#define NV_START_BYTE 0xAA

#define NV_LIVE_POWER_LEN 34
#define NV_ACCUMULATED_POWER_LEN 64
#define NV_FIRMWARE_LEN 1



enum nv_packet_type {    
    LIVE_POWER          = 0x10,
    ACCUMULATED_ENERGY  = 0x11,
    FIRMWARE_VERSION    = 0x12
};

void nv_processor_task(struct task_config *tc) {
    struct nv_packet p;

    while(true) {
        if(xQueueReceive(tc->xNvMessageQueue, &p, (TickType_t)(250 / portTICK_RATE_MS)) == pdPASS) {
            switch (p.type) {
                case LIVE_POWER: ;
                    struct nv_live_power *nvlp = (struct nv_live_power *)&(p.data);
                    if (NV_MONITOR) print_live_power(nvlp);                    
                    break;
                
                case ACCUMULATED_ENERGY: ;
                    struct nv_accumulated_energy *nvae = (struct nv_accumulated_energy *)&(p.data);
                    if (NV_MONITOR) print_accumulated_energy(nvae);
                    break;

                case FIRMWARE_VERSION:
                    break;

                default:
                    ESP_LOGW(PROCESSOR_TASK_TAG, "Unknown packet type: %02x", p.type);                    
            }            
        }
    }
}

void print_live_power(struct nv_live_power *nvlp) {
    printf("%-20s", "Live Energy: ");
    printf("\n");
}

void print_accumulated_energy(struct nv_accumulated_energy *nvae) {
    printf("%-20s", "Accumulated Energy: ");
    printf("\n");
}