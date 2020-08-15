#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "nanoview_main.h"


static int debug(int argc, char **argv) {
    static bool debug = false;
    if (debug) {
        esp_log_level_set("RX_TASK", ESP_LOG_INFO);
        printf("Set level to INFO\n");
    } else {
        esp_log_level_set("RX_TASK", ESP_LOG_DEBUG);
        printf("Set level to DEBUG\n");
    }
    debug = !debug;
    return 0;
}

void register_debug(void) {            
    const esp_console_cmd_t debug_cmd = {
        .command = "debug",
        .help = "Toggle log level between DEBUG and INFO",
        .hint = NULL,
        .func = &debug,
        .argtable = NULL
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&debug_cmd) );
}

static int monitor(int argc, char **argv) {    
    if (NV_MONITOR) {        
        printf("Disable monitor mode\n");
    } else {        
        printf("Enable monitor mode\n");
    }
    NV_MONITOR = !NV_MONITOR;
    return 0;
}

void register_monitor(void) {            
    const esp_console_cmd_t monitor_cmd = {
        .command = "monitor",
        .help = "Toggle monitor mode to print received messages",
        .hint = NULL,
        .func = &monitor,
        .argtable = NULL
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&monitor_cmd) );
}