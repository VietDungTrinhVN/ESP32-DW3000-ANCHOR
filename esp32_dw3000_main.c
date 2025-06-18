/* SPI Master Half Duplex EEPROM example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "dw3000_hw.h"
#include "dwhw.h"
#include "dwmac.h"
#include "dwphy.h"
#include "dwproto.h"
#include "ranging.h"
#include "dw3000_deca_regs.h"
/*
 This code demonstrates how to use the SPI master half duplex mode to read/write a AT932C46D EEPROM (8-bit mode).
*/


#define IS_ANCHOR 0

static const char TAG[] = "main";

static void twr_done_cb(uint64_t src, uint64_t dst, uint16_t dist,
    uint16_t num)
{
    ESP_LOGI(TAG,"TWR Done %04X: %d cm", (uint16_t)dst, dist);
}

void test_twr(void)
{
    // decadriver init
    dw3000_hw_init();
    dw3000_hw_reset();
    dw3000_hw_init_interrupt();

    // libdeca init
    dwhw_init();
    dwphy_config();
    dwphy_set_antenna_delay(DWPHY_ANTENNA_DELAY);
    uint16_t PANID = 0xDECA; // Example PANID
#if IS_ANCHOR == 1
    uint16_t MAC16 = 0x5678; // Example MAC16
#else 
    // uint16_t MAC16 = 0x5677; // Example MAC16
    uint16_t MAC16 = (uint16_t)'E'<<8 | (uint16_t)'V'; // Example destination address

#endif
    dwmac_init(PANID, MAC16, dwprot_rx_handler, NULL, NULL);
    dwmac_set_frame_filter();
    twr_init(400 /*TWR_PROCESSING_DELAY*/, false);
    twr_set_observer(twr_done_cb);
    // two way ranging to 0x0001

#if IS_ANCHOR == 1
    dwmac_set_rx_reenable(true);
    dwt_forcetrxoff();
    dwt_rxenable(DWT_START_RX_IMMEDIATE);
#else 
    uint16_t dst = (uint16_t)'A'<<8 | (uint16_t)'W'; // Example destination address
    twr_start_ss(dst);
    // twr_start_ss(0x5678);
#endif
}

void app_main(void)
{
   
    test_twr();
    uint16_t dst = (uint16_t)'A'<<8 | (uint16_t)'W'; // Example destination address
    while (1) {
        // Add your main loop handling code here.
        vTaskDelay(1000/portTICK_PERIOD_MS);
#if IS_ANCHOR == 0
        twr_start_ss(dst);
        // twr_start_ss(0x5678);
#endif
    }
}
