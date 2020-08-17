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
    char *json_string;

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

/*
[
    {
        "name":"live_power or accumulated_energy or mains_voltage"
        "channel":"total or 1..16"
        "value": "value of channel"
    }
]
*/

char *create_live_power_json(struct nv_live_power *nvlp) {
    cJSON *live_power = cJSON_CreateArray();
    cJSON *name = cJSON_CreateString("mains_voltage");

    for (int i = 1; i <= 16; i++) {

    }
    return "but";
}

char *create_accumulated_energy_json(struct nv_accumulated_energy *nvae) {
    cJSON *accumulated_energy = cJSON_CreateArray();
    cJSON *child_obj = NULL;
    for (int i = 1; i <= 16; i++) {
        
    }
    return "but";
}

void print_live_power(struct nv_live_power *nvlp) {
    printf("%-20s", "Live Energy: ");
    printf("%5dV", nvlp->mains_voltage_v);
    for (int i = 0; i < 16; i++) {
        printf("%5dW", nvlp->live_power_w[i]);
    }
    printf("\n");
}

void print_accumulated_energy(struct nv_accumulated_energy *nvae) {
    printf("%-26s", "Accumulated Energy: ");
        for (int i = 0; i < 16; i++) {
        printf("%4dWh", nvae->counters_wh[i]);
    }
    printf("\n");
}