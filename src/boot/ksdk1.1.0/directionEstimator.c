#include <stdlib.h>
#include "devMMA8451Q.h"

uint_t8* numberOfInstancesCalculator(uint8_t quarter, int16_t* x_ptr);

void directionEstimator()
{
    int16_t x_value[50], y_value[50];
    int16_t x_mean = 0;
    int16_t y_mean = 0;
    uint8_t quarter;
    uint8_t* nr_ptr;
    uint8_t nr_likely;
    uint8_t nr_unlikely;
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
    }
    else if (x_mean >= 0 && y_mean < 0)
    {
        quarter = 2;
    }
    else if (x_mean < 0 && y_mean < 0)
    {
        quarter = 3;
    }
    else
    {
        quarter = 4;
    }

    nr_ptr = numberOfInstancesCalculator(quarter, &x_value);
    nr_likely = *nr_ptr;
    nr_unlikely = *(nr_ptr + 1);
    warpPrint(" %d,", nr_likely);
    warpPrint(" %d,", nr_unlikely);
}

uint_t8* numberOfInstancesCalculator(uint8_t quarter, int16_t* x_ptr)
{
    uint8_t nr[2];
    nr[0] = 0;
    nr[1] = 0;
    for (int i = 0; i < 50; i++)
    {
        switch(quarter)
        {
            case 1:
                if(*(x_ptr + i) >= 3052)
                {
                    nr[0] = nr[0] + 1;
                }
                else
                {
                    nr[1] = nr[1] + 1;
                }
                break;
            case 2:
                if(*(x_ptr + i) >= 2952)
                {
                    nr[0] = nr[0] + 1;
                }
                else
                {
                    nr[1] = nr[1] + 1;
                }
                break;
            case 3:
                if(*(x_ptr + i) <= -2834)
                {
                    nr[0] = nr[0] + 1;
                }
                else
                {
                    nr[1] = nr[1] + 1;
                }
                break;
            default:
              if(*(x_ptr + i) <= -3052)
                {
                    nr[0] = nr[0] + 1;
                }
                else
                {
                    nr[1] = nr[1] + 1;
                }
        }
        
    }
    return &nr;
}
