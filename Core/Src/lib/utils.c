#include "utils.h"

const uint32_t DELAY_US_REF_TIME = MSI_VALUE / 1000000 / 2;

/**
 * @brief Endless loop function to stop the execution of the program here.
 * @note This function does nothing if CPU_TRAPS_ENABLED is set to zero.
 */
void utils_TrapCpu(void)
{
#if CPU_TRAPS_ENABLED
    while(1)
        ;
#endif
}

/**
 * @brief "Busy wait" delay function.
 * @param duration Delay time in [us] (approximative value based on a ~200MHz
 * core clock).
 * @note: this function does not count the time when paused by an interrupt
 * function.
 */
#ifdef __arm__
void utils_DelayUs(uint32_t duration)
{
	uint32_t i 	= 0;
	uint32_t j 	= 0;
    
	for(i=0; i<=duration; i++)
    {
		for(j=0; j<=DELAY_US_REF_TIME; j++)
        {
#ifdef __GNUC__
			asm("nop");
#else
			__asm{NOP};
#endif
		}
	}
}
#endif

/**
 * @brief Blocks until the specified time is elapsed.
 * @param duration delay time in [ms].
 * @note this function does not count the time when paused by an interrupt
 * function.
 */
#ifdef __arm__
void utils_DelayMs(uint32_t duration)
{
	uint32_t startTime = HAL_GetTick();
    
    while(HAL_GetTick() - startTime <= duration)
        ;
}
#endif

/**
 * @brief Saturates a float number between two bounds.
 * @param val value to constrain between two limits.
 * @param min lower limit.
 * @param max upper limit.
 */
void utils_SaturateF(float *val, float min, float max)
{
	if(*val < min)
		*val = min;
	else if(*val > max)
		*val = max;
}

/**
  * @brief Saturate an 16-bit unsigned integer number between two bounds.
  * @param val value to constrain between the two limits.
  * @param min lower limit.
  * @param max upper limit.
  */
void utils_SaturateU16(uint16_t *val, uint16_t min, uint16_t max)
{
    if(*val < min)
        *val = min;
    else if(*val > max)
        *val = max;
}

/**
  * @brief Saturate a signed 32-bit integer number between two bounds.
  * @param val value to constrain between the two limits.
  * @param min lower limit.
  * @param max upper limit.
  */
void utils_SaturateI32(int32_t *val, int32_t min, int32_t max)
{
    if(*val < min)
        *val = min;
    else if(*val > max)
        *val = max;
}

/**
  * @brief Saturate a 32-bit unsigned integer number between two bounds.
  * @param val value to constrain between the two limits.
  * @param min lower limit.
  * @param max upper limit.
  */
void utils_SaturateU32(uint32_t *val, uint32_t min, uint32_t max)
{
	if(*val < min)
		*val = min;
	else if(*val > max)
		*val = max;
}

/**
  * @brief Saturates a float number between two bounds.
  * @param val value to constrain between the two limits.
  * @param min lower limit.
  * @param max upper limit.
  * @return min if val<min, max if val>max, val otherwise.
  */
float utils_SaturatedF(float val, float min, float max)
{
    if(val < min)
        return min;
    else if(val > max)
        return max;
    else
        return val;
}

/**
  * @brief Saturates a 16-bit unsigned integer number between two bounds.
  * @param val value to constrain between the two limits.
  * @param min lower limit.
  * @param max upper limit.
  * @return min if val<min, max if val>max, val otherwise.
  */
uint16_t utils_SaturatedU16(uint16_t val, uint16_t min, uint16_t max)
{
    if(val < min)
        return min;
    else if(val > max)
        return max;
    else
        return val;
}

/**
  * @brief Saturates a 32-bit signed integer number between two bounds.
  * @param val value to constrain between the two limits.
  * @param min lower limit.
  * @param max upper limit.
  * @return min if val<min, max if val>max, val otherwise.
  */
int32_t utils_SaturatedI32(int32_t val, int32_t min, int32_t max)
{
    if(val < min)
        return min;
    else if(val > max)
        return max;
    else
        return val;
}

/**
  * @brief Saturates a 32-bit unsigned integer number between two bounds.
  * @param val value to constrain between the two limits.
  * @param min lower limit.
  * @param max upper limit.
  * @return min if val<min, max if val>max, val otherwise.
  */
uint32_t utils_SaturatedU32(uint32_t val, uint32_t min, uint32_t max)
{
    if(val < min)
        return min;
    else if(val > max)
        return max;
    else
        return val;
}

/**
  * @brief Compute the mean of the array values.
  * @param array array of float number to get the mean from.
  * @param size size of the array.
  * @retval the mean of the array values.
  */
float utils_Mean(float *array, int size)
{
    int i;
    float sum = 0.0f;
    
    for(i=0; i<size; i++)
        sum += array[i];
    
    return sum / (float) size;
}

/**
 * @brief Returns the number with the greatest absolute value.
 * @param a the first number.
 * @param b the first number.
 * @return a if abs(a)>abs(b), b otherwise.
 */
float utils_GreatestAbsoluteValue(float a, float b)
{
    if(a*a > b*b)
        return a;
    else
        return b;
}

/**
 * @brief Constrains a float number in a periodic range [min max].
 * @param x the number to constrain between the two bounds.
 * @param min the low bound.
 * @param max the high bound.
 * @return the number constrained in the two bounds.
 */
float utils_ConstrainPeriodicF(float x, float min, float max)
{
    while(x < min)
        x += (max-min);

    while(x > max)
        x -= (max-min);

    return x;
}

/**
 * @brief Constrains an int number in a periodic range [min max].
 * @param x the number to constrain between the two bounds.
 * @param min the low bound.
 * @param max the high bound.
 * @return the number constrained in the two bounds.
 */
int utils_ConstrainPeriodicI(int x, int min, int max)
{
    while(x < min)
        x += (max-min);

    while(x > max)
        x -= (max-min);

    return x;
}

/**
 * @brief Returns the positive modulo of an integer number.
 * @param num the number to apply the modulo to.
 * @param mod the divider.
 * @return the positive modulo of num by mod.
 */
int32_t utils_Modulo(int32_t num, int32_t mod)
{
    num = num % mod;

    if(num < 0)
        num += mod;

    return num;
}

/**
 * @brief Computes an interpolated value from a look-up table (LUT).
 * @param lut pointer on the look-up table.
 * @param indexF decimal index to read.
 * @return the interpolated value (betwen floor(index) and floor(index)+1).
 */
float utils_ReadLut(const float *lut, float indexF)
{
    int i = (int)indexF;

    float w2 = indexF - ((float)i);
    float w1 = 1.0f - w2;

    return w1*lut[i] + w2*lut[i + 1];
}

