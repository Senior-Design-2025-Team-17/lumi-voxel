/**
 * @file adc_ref.hpp
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief Contains logic for calibrating the ADC reference voltage
 * @version 0.1
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_adc.h)

#include "high_precision_counter.hpp"
#include "scheduler.hpp"

#include <array>
#include <cstdint>

namespace LumiVoxel
{

class AdcRef
{
  private:
	ADC_HandleTypeDef* const hadc;
	float resolution = 0;
	float oversampling = 0;

	Scheduler& scheduler;
	float calibrationInterval;
	float calibrationTaskOffset;
	uint32_t calibrationTaskId = std::numeric_limits<uint32_t>::max();

	uint32_t* const sequenceBuffer;
	const size_t sequenceLength;
	const uint32_t rawVrefInt;
	bool initialized = false;

	void Calibrate();

  public:
	constexpr AdcRef(
		ADC_HandleTypeDef* hadc,
		Scheduler& scheduler,
		float calibrationInterval,
		float calibrationTaskOffset,
		uint32_t* sequenceBuffer,
		size_t sequenceLength,
		uint32_t rawVrefInt)
		: hadc(hadc),
		  scheduler(scheduler),
		  calibrationInterval(calibrationInterval),
		  calibrationTaskOffset(calibrationTaskOffset),
		  sequenceBuffer(sequenceBuffer),
		  sequenceLength(sequenceLength),
		  rawVrefInt(rawVrefInt) {}

	bool Init();

	ADC_HandleTypeDef* GetHandle() const
	{
		return hadc;
	}

	bool Calibrated() const
	{
		return initialized && rawVrefInt != 0;
	}

	uint32_t GetResolution()
	{
		return resolution;
	}

	float GetVref() const;

	float GetVoltage(uint32_t value) const;

	static uint32_t GetAdcResolution(ADC_HandleTypeDef* hadc);
};

} // namespace PSR::BMS
