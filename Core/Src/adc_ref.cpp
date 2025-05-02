/**
 * @file adc_ref.cpp
 * @author Purdue Solar Racing
 * @brief Contains logic for calibrating the ADC reference voltage
 * @version 0.1
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "adc_ref.hpp"

#include "errors.hpp"

using namespace LumiVoxel;

void AdcRef::Calibrate()
{

	// if (__HAL_ADC_IS_CONVERSION_ONGOING_REGULAR(hadc))
	// 	return;

	HAL_ADC_Start(hadc);
	HAL_ADC_PollForConversion(hadc, 100);
	sequenceBuffer[0] = HAL_ADC_GetValue(hadc);
}

bool AdcRef::Init()
{
	if (initialized)
		return true;

	resolution = GetAdcResolution(hadc);
	oversampling = hadc->Init.Oversampling.Ratio + 1;

	if (calibrationInterval == 0)
	{
		ErrorMessage::SetMessage("AdcRef: Invalid calibration interval");
		return false;
	}

	if (hadc == nullptr || hadc->Instance == nullptr)
	{
		ErrorMessage::SetMessage("AdcRef: Invalid ADC handle");
		return false;
	}

	if (HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED) != HAL_OK)
	{
		ErrorMessage::SetMessage("AdcRef: Failed to calibrate ADC");
		return false;
	}

	if (!scheduler.Init())
	{
		ErrorMessage::WrapMessage("AdcRef: Failed to initialize scheduler");
		return false;
	}

	calibrationTaskId = scheduler.AddTask([this]() { Calibrate(); }, calibrationInterval, calibrationTaskOffset);

	if (calibrationTaskId == std::numeric_limits<uint32_t>::max())
	{
		ErrorMessage::WrapMessage("AdcRef: Failed to add calibration task");
		return false;
	}

	Calibrate();

	initialized = true;

	return true;
}

float AdcRef::GetVref() const
{
	constexpr float calibrationVoltage = VREFINT_CAL_VREF / 1000.0f;
	const uint16_t calibrationValue    = *VREFINT_CAL_ADDR;

	if (rawVrefInt == 0)
		return 0;

	return (calibrationVoltage * calibrationValue * oversampling) / rawVrefInt;
}

float AdcRef::GetVoltage(uint32_t value) const
{
	constexpr float calibrationVoltage = VREFINT_CAL_VREF / 1000.0f;
	const uint16_t calibrationValue    = *VREFINT_CAL_ADDR;

	if (value == 0)
		return 0;

	return (3.3f * value) / resolution;
}

uint32_t AdcRef::GetAdcResolution(ADC_HandleTypeDef* hadc)
{
	uint32_t resolution;
	switch (hadc->Init.Resolution)
	{
	case ADC_RESOLUTION_6B:
		resolution = (1 << 6);
		break;
	case ADC_RESOLUTION_8B:
		resolution = (1 << 8);
		break;
	case ADC_RESOLUTION_10B:
		resolution = (1 << 10);
		break;
	case ADC_RESOLUTION_12B:
		resolution = (1 << 12);
		break;
#ifdef ADC_RESOLUTION_14B
	case ADC_RESOLUTION_14B:
		resolution = (1 << 14);
		break;
#endif
#ifdef ADC_RESOLUTION_16B
	case ADC_RESOLUTION_16B:
		resolution = (1 << 16);
		break;
#endif
	default:
		resolution = 0;
	}

	return resolution;
}