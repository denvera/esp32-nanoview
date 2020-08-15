#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct nv_packet {
    uint8_t type;
    uint8_t data[128];
};

void nv_init_uart();
void nv_rx_task();
uint16_t nv_crc(uint8_t data[], uint8_t len);

#ifdef __cplusplus
}
#endif