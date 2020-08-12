idf_component_register(SRCS "nanoview_processor.c" "mqtt.c" "nanoview_uart.c" "console.c"
                            "nanoview_main.c"
                    INCLUDE_DIRS ""
                    REQUIRES console spi_flash)