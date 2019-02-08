/**
 * Copyright (c) 2014 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdbool.h>
#include "boards.h"
#include "bsp.h"
#include "nrf_drv_gpiote.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


/**@brief Button event handler function.
 *
 * @details Responsible for controlling LEDs based on button presses.
 */
void button_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    switch (pin)
    {
    case BUTTON_1:
        // Turn on LED 1.
        nrf_drv_gpiote_out_clear(LED_1);
        break;
    case BUTTON_2:
        // Turn off LED 1.
        nrf_drv_gpiote_out_set(LED_1);
        break;
    case BUTTON_3:
        // Turn on LED 2.
        nrf_drv_gpiote_out_clear(LED_2);
        break;
    case BUTTON_4:
        // Turn off LED 2.
        nrf_drv_gpiote_out_set(LED_2);
        break;
    default:
        break;
    }
}


/**@brief Function for initializing GPIO pins.
 */
static void gpio_config()
{
    ret_code_t err_code;

    // Initialize GPIOTE driver.
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    // Configure output pins for LEDs.
    nrf_gpio_range_cfg_output(LED_1, LED_2);

    // Set output pins (this will turn off the LEDs).
    nrf_drv_gpiote_out_set(LED_1);
    nrf_drv_gpiote_out_set(LED_2);

    // Make a configuration for input pints. This is suitable for both pins in this example.
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    // Configure input pins for 4 buttons, all using the same event handler.
    err_code = nrf_drv_gpiote_in_init(BUTTON_1, &in_config, button_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_in_init(BUTTON_2, &in_config, button_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_in_init(BUTTON_3, &in_config, button_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_in_init(BUTTON_4, &in_config, button_event_handler);
    APP_ERROR_CHECK(err_code);

    // Enable input pins for buttons.
    nrf_drv_gpiote_in_event_enable(BUTTON_1, true);
    nrf_drv_gpiote_in_event_enable(BUTTON_2, true);
    nrf_drv_gpiote_in_event_enable(BUTTON_3, true);
    nrf_drv_gpiote_in_event_enable(BUTTON_4, true);
}


/**@brief Function for initializing logging.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Main function.
 */
int main(void)
{
    log_init();
    gpio_config();

    NRF_LOG_INFO("Application timer tutorial example started.");

    // Enter main loop.
    while (true)
    {
        __WFI();
    }
}
