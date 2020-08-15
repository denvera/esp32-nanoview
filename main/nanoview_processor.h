#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "nanoview_main.h"

struct nv_live_power {
    uint16_t mains_voltage_v;
    uint16_t live_power_w[16];
};

struct nv_accumulated_energy {
    uint32_t counters_wh[16];
};

struct nv_firmware_version {
    uint8_t version;
};

void nv_processor_task(struct task_config *tc);
void print_live_power(struct nv_live_power *nvlp);
void print_accumulated_energy(struct nv_accumulated_energy *nvae);

#ifdef __cplusplus
}
#endif