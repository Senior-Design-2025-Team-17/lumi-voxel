/**
 * @file adc_handle.cpp
 * @author Purdue Solar Racing (Aidan Orr)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2024
 */

#include "adc_handle.hpp"
#include "errors.hpp"

using namespace LumiVoxel;

bool AdcHandle::Init()
{
	if (initialized)
		return true;

	if (!adcRef.Init())
	{
		ErrorMessage::WrapMessage("AdcHandle: ADC reference failed to initialize");
		return false;
	}

	invResolution = 1.0f / adcRef.GetResolution();

	initialized = true;
	return true;
}