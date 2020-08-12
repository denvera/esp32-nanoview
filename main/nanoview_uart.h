#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void nv32_init_uart();
void nv32_rx_task();
uint16_t nv_crc(uint8_t data[], uint8_t len);

#ifdef __cplusplus
}
#endif