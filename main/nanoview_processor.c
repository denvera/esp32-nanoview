#include <stdint.h>
#include <sys/cdefs.h>

#define NV_START_BYTE 0xAA

#define NV_LIVE_POWER_LEN 34
#define NV_ACCUMULATED_POWER_LEN 64
#define NV_FIRMWARE_LEN 1

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


enum nv_packet_type {    
    live_power = 0x10,
    accumulated_energy = 0x11,
    firmware_version = 0x12
};
