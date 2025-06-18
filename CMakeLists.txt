set(srcs "esp32_dw3000_main.c")

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ".")
