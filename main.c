#include <stdbool.h>
#include "app_button.h"
#include "app_error.h"
//#include "app_timer.h"
#include "boards.h"
#include "bsp.h"
//#include "nrf_drv_clock.h"
#include "nrf_drv_gpiote.h"

// Pins for LED's and buttons.
// The diodes on the DK are connected with the cathodes to the GPIO pin, so
// clearing a pin will light the LED and setting the pin will turn of the LED.
#define LED_1_PIN                       BSP_LED_0     // LED 1 on the nRF51-DK or nRF52-DK
#define LED_2_PIN                       BSP_LED_1     // LED 3 on the nRF51-DK or nRF52-DK
#define LED_3_PIN                       BSP_LED_2     // LED 3 on the nRF51-DK or nRF52-DK
#define LED_4_PIN                       BSP_LED_3     // LED 3 on the nRF51-DK or nRF52-DK
#define BUTTON_1_PIN                    BSP_BUTTON_0  // Button 1 on the nRF51-DK or nRF52-DK
#define BUTTON_2_PIN                    BSP_BUTTON_1  // Button 2 on the nRF51-DK or nRF52-DK
#define BUTTON_3_PIN                    BSP_BUTTON_2  // Button 3 on the nRF51-DK or nRF52-DK
#define BUTTON_4_PIN                    BSP_BUTTON_3  // Button 4 on the nRF51-DK or nRF52-DK


// Function for controlling LED's based on button presses.
void button_handler(nrf_drv_gpiote_pin_t pin)
{
    switch (pin)
    {
    case BUTTON_1_PIN:
        nrf_drv_gpiote_out_clear(LED_1_PIN);
        break;
    case BUTTON_2_PIN:
        nrf_drv_gpiote_out_set(LED_1_PIN);
        break;
    case BUTTON_3_PIN:
        nrf_drv_gpiote_out_clear(LED_2_PIN);
        break;
    case BUTTON_4_PIN:
        nrf_drv_gpiote_out_set(LED_2_PIN);
        break;
    default:
        break;
    }
}


// Button event handler.
void gpiote_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    // The button_handler function could be implemented here directly, but is
    // extracted to a separate function as it makes it easier to demonstrate
    // the scheduler later in the tutorial.
    button_handler(pin);
}


// Function for configuring GPIO.
static void gpio_config()
{
    ret_code_t err_code;

    // Initialze driver.
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    // Configure 3 output pins for LED's.
    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);
    err_code = nrf_drv_gpiote_out_init(LED_1_PIN, &out_config);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_out_init(LED_2_PIN, &out_config);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_out_init(LED_3_PIN, &out_config);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_out_init(LED_4_PIN, &out_config);
    APP_ERROR_CHECK(err_code);

    // Set output pins (this will turn off the LED's).
    nrf_drv_gpiote_out_set(LED_1_PIN);
    nrf_drv_gpiote_out_set(LED_2_PIN);
    nrf_drv_gpiote_out_set(LED_3_PIN);
    nrf_drv_gpiote_out_set(LED_4_PIN);

    // Make a configuration for input pints. This is suitable for both pins in this example.
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    // Configure input pins for buttons, with separate event handlers for each button.
    err_code = nrf_drv_gpiote_in_init(BUTTON_1_PIN, &in_config, gpiote_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_in_init(BUTTON_2_PIN, &in_config, gpiote_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_in_init(BUTTON_3_PIN, &in_config, gpiote_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_gpiote_in_init(BUTTON_4_PIN, &in_config, gpiote_event_handler);
    APP_ERROR_CHECK(err_code);

    // Enable input pins for buttons.
    nrf_drv_gpiote_in_event_enable(BUTTON_1_PIN, true);
    nrf_drv_gpiote_in_event_enable(BUTTON_2_PIN, true);
    nrf_drv_gpiote_in_event_enable(BUTTON_3_PIN, true);
    nrf_drv_gpiote_in_event_enable(BUTTON_4_PIN, true);
}


// Main function.
int main(void)
{
    // Configure GPIO's.
    gpio_config();

    // Main loop.
    while (true)
    {
        // Wait for interrupt.
        __WFI();
    }
}
