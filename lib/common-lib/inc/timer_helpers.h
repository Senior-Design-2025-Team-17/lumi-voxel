/**
 * @file timer_helpers.h
 * @author Purdue Solar Racing
 * @brief Commmon timer helper functions
 * @version 0.1
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_tim.h)

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Get the input frequency of a timer
 * 
 * @param tim The timer peripheral to get the input frequency of
 * @return `uint32_t` The input frequency of the timer
 */
uint32_t GetTimerInputFrequency(TIM_TypeDef* tim);

/**
 * @brief Set the frequency of a timer
 * 
 * @param tim The timer peripheral to set the frequency of
 * @param frequency The desired frequency
 * @param precision The counter precision
 * @return `bool` Whether the frequency was set successfully. False if the required precision is too high
 */
bool SetTimerFrequency(TIM_TypeDef* tim, uint32_t frequency, uint32_t precision);

/**
 * @brief Convert a PWM value to a CCR value
 * 
 * @param tim The timer peripheral
 * @param pwm The PWM value (0-1)
 * @return `uint32_t` The CCR value
 */
uint32_t PwmToCCR(TIM_TypeDef* tim, float pwm);

/**
 * @brief Get the CCR value for a channel
 * 
 * @param tim The timer peripheral
 * @param channel The channel bitmask to get the CCR value of
 * @return `volatile uint32_t*` The CCR value
 */
volatile uint32_t* ChannelToCCR(TIM_TypeDef* tim, uint32_t channel);

#ifdef __cplusplus
}
#endif