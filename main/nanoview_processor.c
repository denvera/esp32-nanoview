#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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
    uint8_t last_data[2][128];
    bzero(last_data, 2*128);
    while(true) {
        if(xQueueReceive(tc->xNvMessageQueue, &p, (TickType_t)(250 / portTICK_RATE_MS)) == pdPASS) {
            switch (p.type) {
                case LIVE_POWER: ;
                    struct nv_live_power *nvlp = (struct nv_live_power *)&(p.data);                    
                    if (NV_MONITOR) print_live_power(nvlp);
                    if (memcmp(last_data[0], p.data, NV_LIVE_POWER_LEN) != 0) {
                        json_string = create_live_power_json(nvlp);
                        free(json_string);   
                    }
                    memcpy(last_data[0], p.data, NV_LIVE_POWER_LEN);
                    break;
                
                case ACCUMULATED_ENERGY: ;
                    struct nv_accumulated_energy *nvae = (struct nv_accumulated_energy *)&(p.data);                    
                    if (NV_MONITOR) print_accumulated_energy(nvae);
                    if (memcmp(last_data[1], p.data, NV_ACCUMULATED_POWER_LEN) != 0) {
                        json_string = create_accumulated_energy_json(nvae);
                        free(json_string);   
                    }
                    memcpy(last_data[1], p.data, NV_ACCUMULATED_POWER_LEN);
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

char *create_voltage_json(uint16_t volts) {
    char *json_string = NULL;
    cJSON *voltage_obj = cJSON_CreateObject();
    cJSON_AddItemToObject(voltage_obj, "name", cJSON_CreateString("mains_voltage"));
    cJSON_AddItemToObject(voltage_obj, "volts", volts);
    json_string = cJSON_Print(voltage_obj);
    cJSON_Delete(voltage_obj);
    return json_string;
}

char *create_live_power_json(struct nv_live_power *nvlp) {
    char *json_string = NULL;
    cJSON *live_power = cJSON_CreateArray();
    cJSON *voltage_obj = cJSON_CreateObject();
    cJSON *name = cJSON_CreateString("mains_voltage");
    cJSON *volts = cJSON_CreateNumber(nvlp->mains_voltage_v);
    cJSON_AddItemToObject(voltage_obj, "name", name);
    cJSON_AddItemToObject(voltage_obj, "volts", volts);
    cJSON_AddItemToArray(live_power, voltage_obj);
    cJSON *power_obj = NULL;
    for (int i = 0; i < 16; i++) {        
        power_obj = cJSON_CreateObject();
        cJSON_AddItemToObject(power_obj, "name", cJSON_CreateString("live_power"));
        cJSON_AddItemToObject(power_obj, "channel", cJSON_CreateNumber(i+1));
        cJSON_AddItemToObject(power_obj, "value", cJSON_CreateNumber(nvlp->live_power_w[i]));
        cJSON_AddItemToArray(live_power, power_obj);
    }
    json_string = cJSON_Print(live_power);
    //ESP_LOGD(PROCESSOR_TASK_TAG, "Live Power: %s", json_string);
    cJSON_Delete(live_power);
    return json_string;
}

char *create_accumulated_energy_json(struct nv_accumulated_energy *nvae) {
    cJSON *accumulated_energy = cJSON_CreateArray();        
    cJSON *acc_energy_obj = NULL;
    char *json_string = NULL;
    for (int i = 0; i < 16; i++) {
        acc_energy_obj = cJSON_CreateObject();
        cJSON_AddItemToObject(acc_energy_obj, "name", cJSON_CreateString("accumulated_energy"));
        cJSON_AddItemToObject(acc_energy_obj, "channel", cJSON_CreateNumber(i+1));
        cJSON_AddItemToObject(acc_energy_obj, "value", cJSON_CreateNumber(nvae->counters_wh[i]));
        cJSON_AddItemToArray(accumulated_energy, acc_energy_obj);
    }
    json_string = cJSON_Print(accumulated_energy);
    //ESP_LOGD(PROCESSOR_TASK_TAG, "Accumulated Energy: %s", json_string);
    cJSON_Delete(accumulated_energy);
    return json_string;
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