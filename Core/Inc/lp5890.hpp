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
template <size_t ledCount>
class Driver
{
  public:
	static constexpr uint16_t ColorMax = 65535;

  private:
	Lp5899& interface;
	std::array<uint16_t, ledCount> red   = { 0 };
	std::array<uint16_t, ledCount> green = { 0 };
	std::array<uint16_t, ledCount> blue  = { 0 };
	float& brightness;

	uint8_t globalBrightness     = 7;
	uint8_t redGroupBrightness   = 255;
	uint8_t greenGroupBrightness = 255;
	uint8_t blueGroupBrightness  = 255;

	bool initialized = false;

	/// @brief Quantizes a color value to the range of 0-ColorMax
	/// @param color The color value to quantize (0.0-1.0)
	/// @return The quantized color value (0-ColorMax)
	static uint16_t QuantizeColor(float color)
	{
		float v = color * ColorMax;
		return std::clamp((uint16_t)v, (uint16_t)0, ColorMax);
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
		float& brightness)
		: interface(interface),
		  brightness(brightness)
	{}

	/**
	 * @brief Initialize the LP5890 driver
	 * @param hpc A reference to a high precision counter object
	 *
	 * @return bool true if initialization was successful or already initialized, false otherwise
	 */
	bool Init(HighPrecisionCounter& hpc)
	{
		if (initialized)
			return true;

		// Initialize the LP5899 driver
		if (!interface.Init(hpc))
		{
			ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5899 Interface initialization failed");
			return false;
		}

		Lp5899::CcsiControl ccsiControl{
			.CcsiDataRate       = 0xD, // 10Mbps
			.CcsiSpreadSpectrum = 0,
		};

		puts("LP5890 - Setting LP5899 CCSI control register...");
		if (!interface.TryWriteCcsiControl(ccsiControl, true))
		{
			ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5899 CCSI control write failed");
			return false;
		}

		
		Lp5899::TxFifoControl txFifoControl{
			.TxFifoLevel = 0x1FF, // 512 words
			.TxFifoClear = 0,
		};
		
		puts("LP5890 - Setting LP5899 TX FIFO control register...");
		if (!interface.TryWriteTxFifoControl(txFifoControl, true))
		{
			ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5899 TX FIFO control write failed");
			return false;
		}

		Lp5899::RxFifoControl rxFifoControl{
			.RxFifoLevel = 0x1FF, // 512 words
			.RxFifoClear = 0,
		};

		puts("LP5890 - Setting LP5899 RX FIFO control register...");
		if (!interface.TryWriteRxFifoControl(rxFifoControl, true))
		{
			ErrorMessage::WrapMessage("LP5890 - Initialization failed: LP5899 RX FIFO control write failed");
			return false;
		}

		return true;
	}

	/// @brief Set the brightness of an LED and quantizes the color values
	/// @param index The index of the LED to set (0-ledCount-1)
	/// @param red The brightness of the red channel (0.0-1.0)
	/// @param green The brightness of the green channel (0.0-1.0)
	/// @param blue The brightness of the blue channel (0.0-1.0)
	void SetColor(size_t index, float red, float green, float blue)
	{
		if (index >= ledCount)
			return;

		this->red[index]   = QuantizeColor(red * brightness);
		this->green[index] = QuantizeColor(green * brightness);
		this->blue[index]  = QuantizeColor(blue * brightness);
	}
};

} // namespace LumiVoxel::Lp5890
