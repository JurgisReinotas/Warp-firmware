#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "gpio_pins.h"
#include "SEGGER_RTT.h"
#include "warp.h"

#include "devMMA8451Q.h"

uint8_t nr;

void numberOfInstancesCalculator(uint8_t quarter, int16_t* x_ptr);

void directionEstimator()
{
    int16_t x_value[50], y_value[50];
    int16_t x_mean = 0;
    int16_t y_mean = 0;
    uint8_t quarter;
    uint8_t quart_half;
    uint8_t percentage;
    for(int i = 0; i < 50; i++)
    {
        x_value[i] = returnSensorDataMMA8451Q(false);
        y_value[i] = returnSensorDataMMA8451Q(true);
        x_mean  = x_mean + x_value[i]/50;
        y_mean  = y_mean + y_value[i]/50;
    }

    // Check which quarter the board is pointing at
    if (x_mean >= 0 && y_mean >= 0)
    {
        quarter = 1;
        if(x_mean >= 3052) quart_half = 1;
        else quart_half = 0;
    }
    else if (x_mean >= 0 && y_mean < 0)
    {
        quarter = 2;
        if(x_mean >= 2952) quart_half = 0;
        else quart_half = 1;
    }
    else if (x_mean < 0 && y_mean < 0)
    {
        quarter = 3;
        if(x_mean >= -2834) quart_half = 0;
        else quart_half = 1;
    }
    else
    {
        quarter = 4;
        if(x_mean >= -3052) quart_half = 1;
        else quart_half = 0;
    }

    numberOfInstancesCalculator(quarter, quart_half, &x_value);
    switch(quarter)
    {
        case 1:

            break;
        case 2:

            break;
        case 3:

            break;
        default:
            
    }
}

void numberOfInstancesCalculator(uint8_t quarter, uint8_t quart_half, int16_t* x_ptr)
{
    nr = 0;
    for (int i = 0; i < 50; i++)
    {
        switch(quarter)
        {
            case 1:
                if(*(x_ptr + i) >= 3052)
                {
                    if(quart_half) nr = nr + 1;
                }
                else
                {
                    if(!quart_half) nr = nr + 1;
                }
                break;
            case 2:
                if(*(x_ptr + i) >= 2952)
                {
                    if(!quart_half) nr = nr + 1;
                }
                else
                {
                    if(quart_half) nr = nr + 1;
                }
                break;
            case 3:
                if(*(x_ptr + i) >= -2834)
                {
                    if(!quart_half) nr = nr + 1;
                }
                else
                {
                    if(quart_half) nr = nr + 1;
                }
                break;
            default:
              if(*(x_ptr + i) >= -3052)
                {
                    if(quart_half) nr = nr + 1;
                }
                else
                {
                    if(!quart_half) nr = nr + 1;
                }
        }
        
    }
}
