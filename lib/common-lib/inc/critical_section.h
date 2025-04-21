/**
 * @file critical_section.h
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Contains funcitions for entering and exiting critical (non-interrupt) sections
 * @version 0.1
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)

/**
 * @brief Disables interrupts after this function call
 * 
 * @return `uint32_t` The previous PRIMASK value, which can be used to restore the previous interrupt state
 */
static __always_inline uint32_t EnterCriticalSection()
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

/**
 * @brief Restores the interrupt state to the previous state
 * 
 * @param primask The previous PRIMASK value
 */
static __always_inline void ExitCriticalSection(uint32_t primask)
{
	__set_PRIMASK(primask);
}