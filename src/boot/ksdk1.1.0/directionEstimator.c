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
int32_t nr_weight;
int32_t total_weight;

void numberOfInstancesCalculator(uint8_t quarter, uint8_t quart_half, uint8_t n, int16_t* x_ptr);

void directionEstimator()
{
    uint8_t n = 100;
    int16_t x_value[n], y_value[n];
    int32_t x_mean = 0;
    int32_t y_mean = 0;
    uint8_t quarter;
    uint8_t quart_half;
    int32_t perc_likely;
    int32_t perc_unlikely;
    for(int i = 0; i < n; i++)
    {
        x_value[i] = returnSensorDataMMA8451Q(false);
        y_value[i] = returnSensorDataMMA8451Q(true);
        x_mean  = x_mean + x_value[i];
        y_mean  = y_mean + y_value[i];
    }
    x_mean = x_mean / n;
    y_mean = y_mean / n;

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

    numberOfInstancesCalculator(quarter, quart_half, n, &x_value);
    perc_likely = (nr_weight * 100) / total_weight;
    perc_unlikely = 100 - perc_likely;
    /*switch(quarter)
    {
        case 1:
            if(!quart_half)
            {
                warpPrint("North");
                warpPrint(" %d", perc_likely);
                warpPrint("%%, ");
                warpPrint("West");
                warpPrint(" %d", perc_unlikely);
                warpPrint("%%, ");
            }
            else
            {
                warpPrint("West");
                warpPrint(" %d", perc_likely);
                warpPrint("%%, ");
                warpPrint("North");
                warpPrint(" %d", perc_unlikely);
                warpPrint("%%, ");
            }
            break;
        case 2:
            if(!quart_half)
            {
                warpPrint("West");
                warpPrint(" %d", perc_likely);
                warpPrint("%%, ");
                warpPrint("South");
                warpPrint(" %d", perc_unlikely);
                warpPrint("%%, ");
            }
            else
            {
                warpPrint("South");
                warpPrint(" %d", perc_likely);
                warpPrint("%%, ");
                warpPrint("West");
                warpPrint(" %d", perc_unlikely);
                warpPrint("%%, ");
            }
            break;
        case 3:
            if(!quart_half)
            {
                warpPrint("South");
                warpPrint(" %d", perc_likely);
                warpPrint("%%, ");
                warpPrint("East");
                warpPrint(" %d", perc_unlikely);
                warpPrint("%%, ");
            }
            else
            {
                warpPrint("East");
                warpPrint(" %d", perc_likely);
                warpPrint("%%, ");
                warpPrint("South");
                warpPrint(" %d", perc_unlikely);
                warpPrint("%%, ");
            }
            break;
        default:
            if(!quart_half)
            {
                warpPrint("East");
                warpPrint(" %d", perc_likely);
                warpPrint("%%, ");
                warpPrint("North");
                warpPrint(" %d", perc_unlikely);
                warpPrint("%%, ");
            }
            else
            {
                warpPrint("North");
                warpPrint(" %d", perc_likely);
                warpPrint("%%, ");
                warpPrint("East");
                warpPrint(" %d", perc_unlikely);
                warpPrint("%%, ");
            }
    }
    warpPrint(" %d", (x_mean % 10000));
    warpPrint(" %d", (y_mean % 10000));*/
}

void numberOfInstancesCalculator(uint8_t quarter, uint8_t quart_half, uint8_t n, int16_t* x_ptr)
{
    nr = 0;
    nr_weight = 0;
    total_weight = 0;
    for (int i = 0; i < n; i++)
    {
        switch(quarter)
        {
            case 1:
                if(*(x_ptr + i) >= 3052)
                {
                    if(quart_half)
                    {
                        nr_weight = nr_weight + *(x_ptr + i) - 3052;
                    }
                    total_weight = total_weight + *(x_ptr + i) - 3052;
                }
                else
                {
                    if(!quart_half)
                    {
                        nr_weight = nr_weight + 3052 - *(x_ptr + i);
                    }
                    total_weight = total_weight + 3052 - *(x_ptr + i);
                }
                break;
            case 2:
                if(*(x_ptr + i) >= 2952)
                {
                    if(!quart_half)
                    {
                        nr_weight = nr_weight + *(x_ptr + i) - 2952;
                    }
                    total_weight = total_weight + *(x_ptr + i) - 2952;
                }
                else
                {
                    if(quart_half)
                    {
                        nr_weight = nr_weight + 2952 - *(x_ptr + i);
                    }
                    total_weight = total_weight + 2952 - *(x_ptr + i);
                }
                break;
            case 3:
                if(*(x_ptr + i) >= -2834)
                {
                    if(!quart_half)
                    {
                        nr_weight = nr_weight + *(x_ptr + i) + 2834;
                    }
                    total_weight = total_weight + *(x_ptr + i) + 2834;
                }
                else
                {
                    if(quart_half)
                    {
                        nr_weight = nr_weight - 2834 - *(x_ptr + i);
                    }
                    total_weight = total_weight - 2834 - *(x_ptr + i);
                }
                break;
            default:
              if(*(x_ptr + i) >= -3052)
                {
                    if(quart_half)
                    {
                        nr_weight = nr_weight + *(x_ptr + i) + 3052;
                    }
                    total_weight = total_weight + *(x_ptr + i) + 3052;
                }
                else
                {
                    if(!quart_half)
                    {
                        nr_weight = nr_weight - 3052 - *(x_ptr + i);
                    }
                    total_weight = total_weight - 3052 - *(x_ptr + i);
                }
        }
        warpPrint(" %d\n", *(x_ptr + i));
    }
}
