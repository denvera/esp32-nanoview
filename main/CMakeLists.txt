idf_component_register(SRCS "nanoview_mqtt.c" "nanoview_commands.c" "wifi.c" "nanoview_processor.c" "nanoview_uart.c" "console.c"
                            "nanoview_main.c"
                    INCLUDE_DIRS ""
                    REQUIRES console spi_flash json nvs_flash mqtt)