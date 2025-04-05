/**
 * @file delay.h
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Implements microsecond and millesecond delay functions for STM32 boards
 * @version 0.3
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __DELAY_H
#define __DELAY_H

#ifndef STM32_PROCESSOR
#error "delay.h is only implemented on STM32 boards with the HAL library."
#endif

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include <stdbool.h>
#include <stdint.h>

namespace LumiVoxel
{

/**
 * @brief Wait for a specific number of microseconds before continuing.
 *
 * @param timer1MHz A reference to a 1MHz timer.
 * @param microseconds The number of microseconds to delay.
 */
void delayMicroseconds(TIM_HandleTypeDef* timer1MHz, uint16_t microseconds)
{
	TIM_TypeDef* instance = timer1MHz->Instance;
	instance->CR1 |= TIM_CR1_CEN;        // Ensure the timer is enabled
	instance->CNT = 0;                   // Set the counter to 0
	while (instance->CNT < microseconds) // Wait until count reaches the microsecond count
	{}
}

void delayMicroseconds(TIM_TypeDef* timer1MHz, uint16_t microseconds)
{
	timer1MHz->CR1 |= TIM_CR1_CEN;        // Ensure the timer is enabled
	timer1MHz->CNT = 0;                   // Set the counter to 0
	while (timer1MHz->CNT < microseconds) // Wait until count reaches the microsecond count
	{}
}

} // namespace LumiVoxel

#endif // End of include guard for delay.h
