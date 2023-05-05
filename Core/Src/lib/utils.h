#ifndef DEF_LIB_UTILS_H
#define DEF_LIB_UTILS_H

#include "../Footboard_main.h"

/** @defgroup Utils Utils
  * @brief Utility functions and constants.
  *
  * This module provides functions and constants that may be used at many
  * places.
  *
  * @ingroup Lib
  * @addtogroup Utils
  * @{
  */

#define CPU_TRAPS_ENABLED 1 ///< utils_TrapCpu() will be block forever if 1, or return immediately if 0.

/**
 * @brief Converts a time in seconds to microseconds.
 * @param t time to convert [s].
 * @return converted time [us].
 */
#define SECOND_TO_MICROSECOND_F(t) (((float)(t))*1000000.0f)

/**
 * @brief Converts a time in microseconds to seconds.
 * @param t time to convert [us].
 * @return converted time [s].
 */
#define MICROSECOND_TO_SECOND_F(t) (((float)(t))/1000000.0f)

/**
 * @brief Converts an angular speed in deg/s to RPM.
 * @param x angular speed to convert [deg/s].
 * @return converted angular speed [RPM].
 */
#define DEG_PER_SEC_TO_RPM(x) (((float)(x))/360.0f*60.0f)

/**
 * @brief Converts an angular speed in RPM to deg/s.
 * @param x angular speed to convert [RPM].
 * @return converted angular speed [deg/s].
 */
#define RPM_TO_DEG_PER_SEC(x) (((float)(x))/60.0f*360.0f)

#define PI_F 3.14159265359f ///< PI constant as a float number (M_PI).
#define INV_SQRT_2 0.70710678118f ///< 1/sqrt(2) as a float number (M_SQRT1_2).

/**
 * @brief Converts an angle in degrees to radians.
 * @param x angle to convert [deg].
 * @return converted angle [rad].
 */
#define DEG_TO_RAD(x) (((float)(x))/180.0f*PI_F)

/**
 * @brief Converts an angle in radians to degrees.
 * @param x angle to convert [rad].
 * @return converted angle [deg].
 */
#define RAD_TO_DEG(x) (((float)(x))/PI_F*180.0f)

/**
 * @brief Gets the sign of the given float number.
 * @param x number to get the sign from.
 * @return +1.0f if x is positive, or -1.0f if x is negative.
 */
#define SIGNF(x) copysignf(1.0f, x)

#define MHZ_TO_HZ 1000000 ///< Factor to convert from MHz to Hz.

void utils_TrapCpu(void);

void utils_DelayUs(uint32_t duration);
void utils_DelayMs(uint32_t duration);

void utils_SaturateF(float *val, float min, float max);
void utils_SaturateU16(uint16_t *val, uint16_t min, uint16_t max);
void utils_SaturateI32(int32_t *val, int32_t min, int32_t max);
void utils_SaturateU32(uint32_t *val, uint32_t min, uint32_t max);
float utils_SaturatedF(float val, float min, float max);
uint16_t utils_SaturatedU16(uint16_t val, uint16_t min, uint16_t max);
int32_t utils_SaturatedI32(int32_t val, int32_t min, int32_t max);
uint32_t utils_SaturatedU32(uint32_t val, uint32_t min, uint32_t max);
float utils_Mean(float *array, int size);
float utils_GreatestAbsoluteValue(float a, float b);
float utils_ConstrainPeriodicF(float x, float min, float max);
int utils_ConstrainPeriodicI(int x, int min, int max);
int32_t utils_Modulo(int32_t num, int32_t mod);
float utils_ReadLut(const float *lut, float indexF);

/**
  * @}
  */

#endif
