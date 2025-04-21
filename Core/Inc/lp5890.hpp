/**
 * @file lp5890.hpp
 * @author Aidan Orr
 * @brief Interface for the LP5890 LED driver
 * @version 0.1
 *
 * @details References: https://www.ti.com/lit/ds/symlink/lp5890.pdf
 */
#pragma once

#include "gpio_pin.hpp"
#include "lp5890/registers.hpp"
#include "lp5899.hpp"

#include "stm32_includer.h"
#include STM32_INCLUDE(STM32_PROCESSOR, hal.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_def.h)
#include STM32_INCLUDE(STM32_PROCESSOR, hal_spi.h)

#include <algorithm>
#include <array>
#include <cstdint>

namespace LumiVoxel::Lp5890
{

/**
 * @brief LP5890 LED driver class
 *
 * @tparam ledCount Number of LEDs connected to the driver
 */
class Driver
{
  public:
	static constexpr uint16_t ColorMax = 65535;
	static constexpr uint16_t ColorMin = 0;
	static constexpr size_t LedCount   = 256;

	static constexpr uint16_t CcsiClock = 0xA; ///< @brief CCSI clock rate (5MHz)

  private:
	Lp5899& interface;
	std::array<uint16_t, LedCount> red   = { 0 };
	std::array<uint16_t, LedCount> green = { 0 };
	std::array<uint16_t, LedCount> blue  = { 0 };
	float& brightness;

	uint8_t globalBrightness     = 7;
	uint8_t redGroupBrightness   = 255;
	uint8_t greenGroupBrightness = 255;
	uint8_t blueGroupBrightness  = 255;

	Lp5890::FC0& fc0Config;
	Lp5890::FC1& fc1Config;
	Lp5890::FC2& fc2Config;
	Lp5890::FC3& fc3Config;
	Lp5890::FC4& fc4Config;

	bool initialized = false;

	/// @brief Quantizes a color value to the range of 0-ColorMax
	/// @param color The color value to quantize (0.0-1.0)
	/// @return The quantized color value (0-ColorMax)
	static uint16_t QuantizeColor(float color)
	{
		float v = color * (ColorMax - ColorMin) + ColorMin;
		return std::clamp((uint16_t)v, (uint16_t)ColorMin, ColorMax);
	}

  public:
	/**
	 * @brief Construct a new Lp5890 Driver object
	 *
	 * @param interface A reference to the LP5899 interface object
	 * @param brightness A reference to the brightness value
	 */
	constexpr Driver(
		Lp5899& interface,
		float& brightness,
		Lp5890::FC0& fc0,
		Lp5890::FC1& fc1,
		Lp5890::FC2& fc2,
		Lp5890::FC3& fc3,
		Lp5890::FC4& fc4)
		: interface(interface),
		  brightness(brightness),
		  fc0Config(fc0),
		  fc1Config(fc1),
		  fc2Config(fc2),
		  fc3Config(fc3),
		  fc4Config(fc4)
	{}

	/**
	 * @brief Initialize the LP5890 driver
	 * @param hpc A reference to a high precision counter object
	 *
	 * @return bool true if initialization was successful or already initialized, false otherwise
	 */
	bool Init(HighPrecisionCounter& hpc);

	/// @brief Set the brightness of an LED and quantizes the color values
	/// @param index The index of the LED to set (0-LedCount-1)
	/// @param red The brightness of the red channel (0.0-1.0)
	/// @param green The brightness of the green channel (0.0-1.0)
	/// @param blue The brightness of the blue channel (0.0-1.0)
	void SetColor(size_t index, float red, float green, float blue)
	{
		if (index >= LedCount)
			return;

		this->red[index]   = QuantizeColor(red * brightness);
		this->green[index] = QuantizeColor(green * brightness);
		this->blue[index]  = QuantizeColor(blue * brightness);
	}
};

} // namespace LumiVoxel::Lp5890
