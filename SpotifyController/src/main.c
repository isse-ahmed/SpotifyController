#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio.h"
#include "esp_common.h"
#include <stdint.h>


//Pins for the buttons
#define BLINK_PIN 0
#define SKIP_PIN 2
#define SKIP_GPIO FUNC_GPIO2
#define PAUSE_PLAY_PIN 5
#define PAUSE_PLAY_GPIO FUNC_GPIO5
#define PREV_PIN 4
#define PREV_GPIO FUNC_GPIO4

//Information of Pins and Debounce (Milliseconds)
#define NUM_PINS 3
#define DEBOUNCE_DELAY 50

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;
    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void spotify_buttons(void *ignore)
{
    //Test LED Button
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U,0);

    //Button Setup
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U,SKIP_GPIO);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U,PAUSE_PLAY_GPIO);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U,PREV_GPIO);
    GPIO_DIS_OUTPUT(GPIO_ID_PIN(SKIP_PIN));
    GPIO_DIS_OUTPUT(GPIO_ID_PIN(PREV_PIN));
    GPIO_DIS_OUTPUT(GPIO_ID_PIN(PAUSE_PLAY_PIN));

    //Variables for debouncing
    uint8_t button_pins[NUM_PINS] = {SKIP_PIN,PAUSE_PLAY_PIN, PREV_PIN};
    uint8_t last_states[NUM_PINS] = {1,1,1};
    uint32_t last_debounce_times[NUM_PINS] = {0,0,0};
    uint32_t current_state = 1;

    while(true)
    {
        for(int i = 0; i < NUM_PINS; i++)
        {
            current_state = GPIO_INPUT_GET(GPIO_ID_PIN(button_pins[i]));
            if(current_state != last_states[i])
            {
                last_debounce_times[i] = current_state;
            }

            //Checking if the ticks passed is greater than the amount of ticks after 50ms
            if((xTaskGetTickCount() - last_debounce_times[i]) > (DEBOUNCE_DELAY / portTICK_RATE_MS))
            {
                if(current_state == 0)
                {
                    //Turn on the LED for 1 second
                    GPIO_OUTPUT_SET(GPIO_ID_PIN(0), 0);
                    vTaskDelay(1000/portTICK_RATE_MS);
                    GPIO_OUTPUT_SET(GPIO_ID_PIN(0), 1);
                }
                last_states[i] = current_state;
            }
        }

        vTaskDelay(10 / portTICK_RATE_MS); // Short delay to prevent CPU hogging
    }
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    xTaskCreate(&spotify_buttons, "startup", 2048, NULL, 1, NULL);
}

