/**
 * @file gpio_pin.hpp
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Wraps a GPIO port and pin for easy manipulation
 * @version 0.1
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)

#include <cstdint>

namespace PSR
{

class GpioPin
{
  private:
	volatile GPIO_TypeDef* port; ///< @brief The GPIO port
	uint32_t pin;                ///< @brief The pin bitmask

  public:
	constexpr GpioPin(GPIO_TypeDef* port, uint32_t pin)
		: port(port), pin(pin)
	{}

	inline void Set()
	{
		port->BSRR = pin;
	}

	inline void Reset()
	{
		port->BSRR = pin << 16;
	}

	void Toggle()
	{
		port->ODR ^= pin;
	}

	void SetValue(bool value)
	{
		if (value)
			Set();
		else
			Reset();
	}

	bool IsSet() const
	{
		return (port->IDR & pin) != 0;
	}
};

} // namespace PSR
