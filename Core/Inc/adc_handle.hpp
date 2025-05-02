/**
 * @file adc_handle.hpp
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Creates a ADC handle for the STM32 microcontroller
 * @version 0.1
 *
 * @copyright Copyright (c) 2024
 */
#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_adc.h)

#include "adc_ref.hpp"

#include <array>
#include <cstdint>

namespace LumiVoxel
{

/**
 * @brief ADC handle class
 *
 */
class AdcHandle
{
	AdcRef& adcRef;
	const volatile uint32_t& rawValue;
	float invResolution = 0;

	bool initialized = false;

  public:
	/**
	 * @brief Construct a new ADC Handle object
	 *
	 * @param adcRef ADC Reference voltage
	 * @param rawValue reference to the raw ADC value
	 */
	constexpr AdcHandle(AdcRef& adcRef, const volatile uint32_t& rawValue)
		: adcRef(adcRef),
		  rawValue(rawValue) {}

	/**
	 * @brief Initialize the ADC handle
	 *
	 * @return `bool` true if the ADC handle was initialized successfully, false otherwise
	 */
	bool Init();

	/**
	 * @brief Get the most recent voltage measurement
	 *
	 * @return `float` the voltage
	 */
	float GetVoltage() const
	{
		return adcRef.GetVoltage(rawValue);
	}
};

} // namespace PSR::BMS