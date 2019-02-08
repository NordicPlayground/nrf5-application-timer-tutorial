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
#include "app_timer.h"
#include "nrf_drv_clock.h"


APP_TIMER_DEF(m_repeated_timer_id);     /**< Handler for repeated timer used to blink LED 1. */
APP_TIMER_DEF(m_single_shot_timer_id);  /**< Handler for single shot timer used to light LED 2. */


/**@brief Function starting the internal LFCLK oscillator.
 *
 * @details This is needed by RTC1 which is used by the Application Timer
 *          (When SoftDevice is enabled the LFCLK is always running and this is not needed).
 */
static void lfclk_request(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
}


/**@brief Button event handler function.
 *
 * @details Responsible for controlling LEDs based on button presses.
 */
void button_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    ret_code_t      err_code;
    static uint32_t timeout = 0;

    switch (pin)
    {
    case BUTTON_1:
        // Start repeated timer (start blinking LED).
        err_code = app_timer_start(m_repeated_timer_id, APP_TIMER_TICKS(200), NULL);
        APP_ERROR_CHECK(err_code);
        break;
    case BUTTON_2:
        // Stop the repeated timer (stop blinking LED).
        err_code = app_timer_stop(m_repeated_timer_id);
        APP_ERROR_CHECK(err_code);
        break;
    case BUTTON_3:
        // Start single shot timer which turns on LED2 when it expires.
        // Increase the timeout with 1 second every time.
        timeout += 1000;
        err_code = app_timer_start(m_single_shot_timer_id, APP_TIMER_TICKS(timeout), NULL);
        APP_ERROR_CHECK(err_code);
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


/**@brief Timeout handler for the repeated timer.
 */
static void repeated_timer_handler(void * p_context)
{
    nrf_drv_gpiote_out_toggle(LED_1);
}


/**@brief Timeout handler for the single shot timer.
 */
static void single_shot_timer_handler(void * p_context)
{
    nrf_drv_gpiote_out_clear(LED_2);
}


/**@brief Create timers.
 */
static void create_timers()
{  
    ret_code_t err_code;

    // Create timers
    err_code = app_timer_create(&m_repeated_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                repeated_timer_handler);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&m_single_shot_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                single_shot_timer_handler);
    APP_ERROR_CHECK(err_code);
}


/**@brief Main function.
 */
int main(void)
{
    log_init();
    gpio_config();
    lfclk_request();
    app_timer_init();
    create_timers();

    NRF_LOG_INFO("Application timer tutorial example started.");

    // Enter main loop.
    while (true)
    {
        __WFI();
    }
}
