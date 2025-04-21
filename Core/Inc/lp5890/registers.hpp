/**
 * @file registers.hpp
 * @author Aidan Orr
 * @brief LP5890 LED driver register definitions
 * @version 0.1
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include <array>
#include <cstdint>

namespace LumiVoxel::Lp5890
{

/**
 * @brief LP5890 command IDs
 */
enum struct Command : uint16_t
{
	FC0_WRITE  = 0xAA00, ///< @brief Write to the FC0 register
	FC1_WRITE  = 0xAA01, ///< @brief Write to the FC1 register
	FC2_WRITE  = 0xAA02, ///< @brief Write to the FC2 register
	FC3_WRITE  = 0xAA03, ///< @brief Write to the FC3 register
	FC4_WRITE  = 0xAA04, ///< @brief Write to the FC4 register
	FC5_WRITE  = 0xAA05, ///< @brief Write to the FC5 register
	FC6_WRITE  = 0xAA06, ///< @brief Write to the FC6 register
	FC7_WRITE  = 0xAA07, ///< @brief Write to the FC7 register
	FC8_WRITE  = 0xAA08, ///< @brief Write to the FC8 register
	FC9_WRITE  = 0xAA09, ///< @brief Write to the FC9 register
	FC10_WRITE = 0xAA0A, ///< @brief Locate the line for LOD
	FC11_WRITE = 0xAA0B, ///< @brief Located the line for LSD

	CHIP_INDEX_WRITE = 0xAA10, ///< @brief Write to the Chip Index register
	VSYNC_WRITE      = 0xAAF0, ///< @brief Write VSYNC command
	SOFT_RESET       = 0xAA80, ///< @brief Reset all the registers except the SRAM

	SRAM_WRITE = 0xAA30, ///< @brief Write to the SRAM

	FC0_READ        = 0xAA60, ///< @brief Read from the FC0 register
	FC1_READ        = 0xAA61, ///< @brief Read from the FC1 register
	FC2_READ        = 0xAA62, ///< @brief Read from the FC2 register
	FC3_READ        = 0xAA63, ///< @brief Read from the FC3 register
	FC4_READ        = 0xAA64, ///< @brief Read from the FC4 register
	FC5_READ        = 0xAA65, ///< @brief Read from the FC5 register
	FC6_READ        = 0xAA66, ///< @brief Read from the FC6 register
	FC7_READ        = 0xAA67, ///< @brief Read from the FC7 register
	FC8_READ        = 0xAA68, ///< @brief Read from the FC8 register
	FC9_READ        = 0xAA69, ///< @brief Read from the FC9 register
	FC10_READ       = 0xAA6A, ///< @brief Read from the FC10 register
	FC11_READ       = 0xAA6B, ///< @brief Read from the FC11 register
	FC12_READ       = 0xAA6C, ///< @brief Read from the FC12 register
	FC13_READ       = 0xAA6D, ///< @brief Read from the FC13 register
	FC14_READ       = 0xAA6E, ///< @brief Read from the FC14 register
	FC15_READ       = 0xAA6F, ///< @brief Read from the FC15 register
	CHIP_INDEX_READ = 0xAA70, ///< @brief Read from the Chip Index register
}; // enum struct Command

/// @brief Represents a color value in the LP5890
struct Color
{
	uint16_t Blue;  ///< @brief Blue component of the color
	uint16_t Green; ///< @brief Green component of the color
	uint16_t Red;   ///< @brief Red component of the color
}; // struct Color

/// @brief LP5890 FC0 register structure
union FC0
{
	struct
	{
		uint16_t ChipNumber               : 5; ///< @brief Chip number (0 - 1 device, 1 - 2 devices, ..., 31 - 32 devices)
		uint16_t Reserved5                : 3; ///< @brief Reserved bits [7:5]
		uint16_t PreDischargeEnable       : 1; ///< @brief Pre-discharge enable (0 - Disabled, 1 - Enabled)
		uint16_t Reserved9                : 3; ///< @brief Reserved bits [11:9]
		uint16_t PowerSavingEnable        : 1; ///< @brief Power saving enable (0 - Disabled, 1 - Enabled)
		uint16_t PowerSavingPlusMode      : 2; ///< @brief Power saving plus mode (0 - Disabled, 1 - Save Power at High Level, 2 - Save Power at Medium Level, 3 - Save Power at Low Level)
		uint16_t LedOpenLoadRemovalEnable : 1; ///< @brief LED open load removal enable (0 - Disabled, 1 - Enabled)

		uint16_t ScanLineNumber      : 5; ///< @brief Scan line number (0 - 1 line, 1 - 2 lines, ..., 31 - 32 lines)
		uint16_t SubPeriodNumber     : 3; ///< @brief Sub-period number (0 - 16, 1 - 32, ..., 7 - 128)
		uint16_t Reserved24          : 3; ///< @brief Reserved bits [26:24]
		uint16_t FrequencyMode       : 1; ///< @brief GCLK Multiplier Mode (0 - 40MHz - 80MHz, 1 - 80MHz - 160MHz)
		uint16_t FrequencyMultiplier : 4; ///< @brief GCLK Multiplier (0 - 1x SCLK, 1 - 2x SCLK, 2 - 3x SCLK, 3 - 4x SCLK, 4 - 5x SCLK, 5 - 6x SCLK, 6 - 7x SCLK, 7 - 8x SCLK, 8 - 9x SCLK, 9 - 10x SCLK, 10 - 11x SCLK, 11 - 12x SCLK, 12 - 13x SCLK, 13 - 14x SCLK, 14 - 15x SCLK, 15 - 16x SCLK)

		uint16_t Reserved32      : 3; ///< @brief Reserved bits [34:32]
		uint16_t RedGroupDelay   : 3; ///< @brief Red group delay, forward PWM mode only (0 - no delay, 1 - 1 GCLK, 2 - 2 GCLK, 3 - 3 GCLK, 4 - 4 GCLK, 5 - 5 GCLK, 6 - 6 GCLK, 7 - 7 GCLK)
		uint16_t GreenGroupDelay : 3; ///< @brief Green group delay, forward PWM mode only (0 - no delay, 1 - 1 GCLK, 2 - 2 GCLK, 3 - 3 GCLK, 4 - 4 GCLK, 5 - 5 GCLK, 6 - 6 GCLK, 7 - 7 GCLK)
		uint16_t BlueGroupDelay  : 3; ///< @brief Blue group delay, forward PWM mode only (0 - no delay, 1 - 1 GCLK, 2 - 2 GCLK, 3 - 3 GCLK, 4 - 4 GCLK, 5 - 5 GCLK, 6 - 6 GCLK, 7 - 7 GCLK)
		uint16_t Reserved44      : 2; ///< @brief Reserved bits [45:44]
		uint16_t ModuleSize      : 2; ///< @brief Module Size (0 - 2 devices stackable, 1 - 1 device non-stackable, 2 - 2 devices stackable, 3 - 3 devices stackable)
	};

	std::array<uint16_t, 3> Value;

	/// @brief Creates the default FC0 register with the default values
	/// @return The default FC0 register
	static constexpr FC0 Default()
	{
		FC0 fc0;
		fc0.ChipNumber               = 7;
		fc0.Reserved5                = 0;
		fc0.PreDischargeEnable       = 1;
		fc0.Reserved9                = 0;
		fc0.PowerSavingEnable        = 0;
		fc0.PowerSavingPlusMode      = 0;
		fc0.LedOpenLoadRemovalEnable = 0;
		fc0.ScanLineNumber           = 15;
		fc0.SubPeriodNumber          = 0;
		fc0.Reserved24               = 0;
		fc0.FrequencyMode            = 0;
		fc0.FrequencyMultiplier      = 7;
		fc0.Reserved32               = 0;
		fc0.RedGroupDelay            = 0;
		fc0.GreenGroupDelay          = 0;
		fc0.BlueGroupDelay           = 0;
		fc0.Reserved44               = 1;
		fc0.ModuleSize               = 0;
		return fc0;
	}
};

/// @brief LP5890 FC1 register structure
union FC1
{
	struct
	{
		uint64_t SegmentLength                : 10; ///< @brief The GCLK number in each segment (0...127 - 128 GCLK, 128 - 129 GCLK, ..., 1023 - 1024 GCLK)
		uint64_t RedLowGrayscaleSmoothing     : 5;  ///< @brief Brightness smoothing for low grayscale red (0 - level 1, 1 - level 2, ..., 31 - level 32)
		uint64_t GreenLowGrayscaleSmoothing   : 5;  ///< @brief Brightness smoothing for low grayscale green (0 - level 1, 1 - level 2, ..., 31 - level 32)
		uint64_t BlueLowGrayscaleSmoothing    : 5;  ///< @brief Brightness smoothing for low grayscale blue (0 - level 1, 1 - level 2, ..., 31 - level 32)
		uint64_t RedLowGrayscaleEnhancement   : 4;  ///< @brief Brightness enhancement for low grayscale red (0 - level 0, 1 - level 1, ..., 15 - level 15)
		uint64_t GreenLowGrayscaleEnhancement : 4;  ///< @brief Brightness enhancement for low grayscale green (0 - level 0, 1 - level 1, ..., 15 - level 15)
		uint64_t BlueLowGrayscaleEnhancement  : 4;  ///< @brief Brightness enhancement for low grayscale blue (0 - level 0, 1 - level 1, ..., 15 - level 15)
		uint64_t ScanLineSwitchTime           : 4;  ///< @brief Scan line switch time (0 - 45 GCLK, 1 - 2x30 GCLK, 2 - 3x30 GCLK, ..., 15 - 16x30 GCLK)
		uint64_t BlackFieldAdjustment         : 6;  ///< @brief Black field adjustment (0 - 0 GCLK, 1 - 1 GCLK, ..., 63 - 63 GCLK)
		uint64_t Reserved47                   : 1;  ///< @brief Reserved bit 47

		uint64_t Unused : 16; ///< @brief Unused bits [63:48] (only used to fill the space not used by the LP5890)
	};

	std::array<uint16_t, 3> Value;

	/// @brief Creates the default FC1 register with the default values
	/// @return The default FC1 register
	static constexpr FC1 Default()
	{
		FC1 fc1;
		fc1.SegmentLength                = 0;
		fc1.RedLowGrayscaleSmoothing     = 9;
		fc1.GreenLowGrayscaleSmoothing   = 9;
		fc1.BlueLowGrayscaleSmoothing    = 9;
		fc1.RedLowGrayscaleEnhancement   = 0;
		fc1.GreenLowGrayscaleEnhancement = 0;
		fc1.BlueLowGrayscaleEnhancement  = 0;
		fc1.ScanLineSwitchTime           = 7;
		fc1.BlackFieldAdjustment         = 0;
		fc1.Reserved47                   = 0;
		return fc1;
	}
};

/// @brief LP5890 FC2 register structure
union FC2
{
	struct
	{
		uint16_t RedPreDischargeVoltage     : 4; ///< @brief Red pre-discharge voltage, must not be higher than (VR - 1.3V) (0 - 0.1V, 1 - 0.2V, 2 - 0.3V, 3 - 0.4V, 4 - 0.5V, 5 - 0.6V, 6 - 0.7V, 7 - 0.8V, 8 - 0.9V, 9 - 1.0V, 10 - 1.1V, 11 - 1.3V, 12 - 1.5V, 13 - 1.7V, 14 - 1.9V, 15 - 2.1V)
		uint16_t GreenPreDischargeVoltage   : 4; ///< @brief Green pre-discharge voltage, must not be higher than (VG - 1.3V) (0 - 0.1V, 1 - 0.2V, 2 - 0.3V, 3 - 0.4V, 4 - 0.5V, 5 - 0.6V, 6 - 0.7V, 7 - 0.8V, 8 - 0.9V, 9 - 1.0V, 10 - 1.1V, 11 - 1.3V, 12 - 1.5V, 13 - 1.7V, 14 - 1.9V, 15 - 2.1V)
		uint16_t BluePreDischargeVoltage    : 4; ///< @brief Blue pre-discharge voltage, must not be higher than (VB - 1.3V) (0 - 0.1V, 1 - 0.2V, 2 - 0.3V, 3 - 0.4V, 4 - 0.5V, 5 - 0.6V, 6 - 0.7V, 7 - 0.8V, 8 - 0.9V, 9 - 1.0V, 10 - 1.1V, 11 - 1.3V, 12 - 1.5V, 13 - 1.7V, 14 - 1.9V, 15 - 2.1V)
		uint16_t RedDecouplingLevel         : 4; ///< @brief Red decoupling level (0 - level 1 (lowest), 1 - level 2, ..., 15 - level 16 (highest))
		uint16_t GreenDecouplingLevel       : 4; ///< @brief Green decoupling level (0 - level 1 (lowest), 1 - level 2, ..., 15 - level 16 (highest))
		uint16_t BlueDecouplingLevel        : 4; ///< @brief Blue decoupling level (0 - level 1 (lowest), 1 - level 2, ..., 15 - level 16 (highest))
		uint16_t RedBrighnessCompensation   : 4; ///< @brief Red brightness compensation (0 - level 1 (lowest), 1 - level 2, ..., 15 - level 16 (highest))
		uint16_t GreenBrighnessCompensation : 4; ///< @brief Green brightness compensation (0 - level 1 (lowest), 1 - level 2, ..., 15 - level 16 (highest))
		uint16_t BlueBrighnessCompensation  : 4; ///< @brief Blue brightness compensation (0 - level 1 (lowest), 1 - level 2, ..., 15 - level 16 (highest))
		uint16_t Reserved36                 : 3; ///< @brief Reserved bits [38:36]
		uint16_t RedChannelImmunity         : 1; ///< @brief Red channel immunity (0 - high immunity, 1 - low immunity)
		uint16_t GreenChannelImmunity       : 1; ///< @brief Green channel immunity (0 - high immunity, 1 - low immunity)
		uint16_t BlueChannelImmunity        : 1; ///< @brief Blue channel immunity (0 - high immunity, 1 - low immunity)
		uint16_t MaximumSubPeriod256        : 1; ///< @brief Set the maximum sub-period to 256 GCLK (0 - disable, 1 - enable)
		uint16_t Reserved43                 : 5; ///< @brief Reserved bits [47:43]
	};

	std::array<uint16_t, 3> Value;

	/// @brief Creates the default FC2 register with the default values
	/// @return The default FC2 register
	static constexpr FC2 Default()
	{
		FC2 fc2;
		fc2.RedPreDischargeVoltage     = 6;
		fc2.GreenPreDischargeVoltage   = 6;
		fc2.BluePreDischargeVoltage    = 6;
		fc2.RedDecouplingLevel         = 0;
		fc2.GreenDecouplingLevel       = 0;
		fc2.BlueDecouplingLevel        = 0;
		fc2.RedBrighnessCompensation   = 0;
		fc2.GreenBrighnessCompensation = 0;
		fc2.BlueBrighnessCompensation  = 0;
		fc2.Reserved36                 = 0;
		fc2.RedChannelImmunity         = 1;
		fc2.GreenChannelImmunity       = 1;
		fc2.BlueChannelImmunity        = 1;
		fc2.MaximumSubPeriod256        = 0;
		fc2.Reserved43                 = 0;
		return fc2;
	}
};

/// @brief LP5890 FC3 register structure
union FC3
{
	struct
	{
		uint16_t Reserved0             : 8; ///< @brief Reserved bits [7:0]
		uint16_t RedColorBrightness    : 8; ///< @brief Red color brightness level (0 - level 0 (lowest), 1 - level 1, ..., 255 - level 256 (highest))
		uint16_t GreenColorBrightness  : 8; ///< @brief Green color brightness level (0 - level 0 (lowest), 1 - level 1, ..., 255 - level 256 (highest))
		uint16_t BlueColorBrightness   : 8; ///< @brief Blue color brightness level (0 - level 0 (lowest), 1 - level 1, ..., 255 - level 256 (highest))
		uint16_t GlobalBrightness      : 3; ///< @brief Global brightness level (0 - level 0 (lowest), 1 - level 1, ..., 7 - level 7 (highest))
		uint16_t LedShortRemovalLevel  : 4; ///< @brief LED short removal level (0 - level 1, 1 - level 2, ..., 15 - level 16)
		uint16_t RedLedWeakThreshold   : 3; ///< @brief Red LED weak/short detection threshold (0 - 0.2V, 1 - 0.4V, 2 - 0.8V, 3 - 1.0V, 4 - 1.2V, 5 - 1.4V, 6 - 1.6V, 7 - 1.8V)
		uint16_t GreenLedWeakThreshold : 3; ///< @brief Green LED weak/short detection threshold (0 - 0.2V, 1 - 0.4V, 2 - 0.8V, 3 - 1.2V, 4 - 1.6V, 5 - 2.0V, 6 - 2.4V, 7 - 2.8V)
		uint16_t BlueLedWeakThreshold  : 3; ///< @brief Blue LED weak/short detection threshold (0 - 0.2V, 1 - 0.4V, 2 - 0.8V, 3 - 1.2V, 4 - 1.6V, 5 - 2.0V, 6 - 2.4V, 7 - 2.8V)
	};

	std::array<uint16_t, 3> Value;

	/// @brief Creates the default FC3 register with the default values
	/// @return The default FC3 register
	static constexpr FC3 Default()
	{
		FC3 fc3;
		fc3.Reserved0             = 0;
		fc3.RedColorBrightness    = 127;
		fc3.GreenColorBrightness  = 127;
		fc3.BlueColorBrightness   = 127;
		fc3.GlobalBrightness      = 3;
		fc3.LedShortRemovalLevel  = 7;
		fc3.RedLedWeakThreshold   = 0;
		fc3.GreenLedWeakThreshold = 0;
		fc3.BlueLedWeakThreshold  = 0;
		return fc3;
	}
};

/// @brief LP5890 FC4 register structure
union FC4
{
	struct
	{
		uint16_t Reserved0                   : 3;  ///< @brief Reserved bits [2:0]
		uint16_t MaxCurrent                  : 1;  ///< @brief Maximum current of each channel (0 - 10mA, 1 - 20mA)
		uint16_t Reserved4                   : 10; ///< @brief Reserved bits [13:4]
		uint16_t ScanLineReverse             : 1;  ///< @brief When 2 or 3 device stackable mode is selected, reverse the scan lines on the PCB (0 - sequence is L0-L15, L16-L31, 1 - sequence is L0-L15, L31-L16)
		uint16_t RedFineCompensation         : 1;  ///< @brief Red brightness compensation level fine range (0 - disable, 1 - enable)
		uint16_t GreenFineCompensation       : 1;  ///< @brief Green brightness compensation level fine range (0 - disable, 1 - enable)
		uint16_t BlueFineCompensation        : 1;  ///< @brief Blue brightness compensation level fine range (0 - disable, 1 - enable)
		uint16_t RedOffSlewRateControl       : 1;  ///< @brief Slew rate control when Red channel turns off operation (0 - slow slew rate, 1 - fast slew rate)
		uint16_t GreenOffSlewRateControl     : 1;  ///< @brief Slew rate control when Green channel turns off operation (0 - slow slew rate, 1 - fast slew rate)
		uint16_t BlueOffSlewRateControl      : 1;  ///< @brief Slew rate control when Blue channel turns off operation (0 - slow slew rate, 1 - fast slew rate)
		uint16_t RedOnSlewRateControl        : 2;  ///< @brief Slew rate control when Red channel turns on operation (0 - slower slew rate, 1 - slow slew rate, 2 - fast slew rate, 3 - faster slew rate)
		uint16_t GreenOnSlewRateControl      : 2;  ///< @brief Slew rate control when Green channel turns on operation (0 - slower slew rate, 1 - slow slew rate, 2 - fast slew rate, 3 - faster slew rate)
		uint16_t BlueOnSlewRateControl       : 2;  ///< @brief Slew rate control when Blue channel turns on operation (0 - slower slew rate, 1 - slow slew rate, 2 - fast slew rate, 3 - faster slew rate)
		uint16_t Reserved28                  : 4;  ///< @brief Reserved bits [31:28]
		uint16_t RedCaurseCompensation       : 1;  ///< @brief Red brightness compensation level coarse range (0 - disable, 1 - enable)
		uint16_t GreenCaurseCompensation     : 1;  ///< @brief Green brightness compensation level coarse range (0 - disable, 1 - enable)
		uint16_t BlueCaurseCompensation      : 1;  ///< @brief Blue brightness compensation level coarse range (0 - disable, 1 - enable)
		uint16_t FirstLineDimming            : 4;  ///< @brief First line dimming level (0 - level 1, 1 - level 2, ..., 15 - level 16)
		uint16_t DecoupleOnOffChannels       : 1;  ///< @brief Decoupling between on and off channels (0 - disable, 1 - enable)
		uint16_t DecouplingEnhancementLevel  : 4;  ///< @brief Decoupling enhancement level (0 - level 1, 1 - level 2, ..., 15 - level 16)
		uint16_t DecouplingEnhancementEnable : 1;  ///< @brief Decoupling enhancement enable (0 - disable, 1 - enable)
		uint16_t Reserved45                  : 3;  ///< @brief Reserved bit [47:45]
	};

	std::array<uint16_t, 3> Value;

	/// @brief Creates the default FC4 register with the default values
	/// @return The default FC4 register
	static constexpr FC4 Default()
	{
		FC4 fc4;
		fc4.Reserved0                   = 0;
		fc4.MaxCurrent                  = 0;
		fc4.Reserved4                   = 0;
		fc4.ScanLineReverse             = 1;
		fc4.RedFineCompensation         = 0;
		fc4.GreenFineCompensation       = 0;
		fc4.BlueFineCompensation        = 0;
		fc4.RedOffSlewRateControl       = 0;
		fc4.GreenOffSlewRateControl     = 0;
		fc4.BlueOffSlewRateControl      = 0;
		fc4.RedOnSlewRateControl        = 1;
		fc4.GreenOnSlewRateControl      = 1;
		fc4.BlueOnSlewRateControl       = 1;
		fc4.Reserved28                  = 0;
		fc4.RedCaurseCompensation       = 0;
		fc4.GreenCaurseCompensation     = 0;
		fc4.BlueCaurseCompensation      = 0;
		fc4.FirstLineDimming            = 0;
		fc4.DecoupleOnOffChannels       = 0;
		fc4.DecouplingEnhancementLevel  = 8;
		fc4.DecouplingEnhancementEnable = 0;
		fc4.Reserved45                  = 0;
		return fc4;
	}
};

/// @brief LP5890 FC10 register structure
union FC10
{
	struct
	{
		uint16_t LedOpenLoadCommand : 5; ///< @brief Locate the line with LED open warnings (0 - line 0, 1 - line 1, ..., 31 - line 31)
	};

	std::array<uint16_t, 3> Value;
};

/// @brief LP5890 FC11 register structure
union FC11
{
	struct
	{
		uint16_t LedShortCircuitCommand : 5; ///< @brief Locate the line with LED short circuit warnings (0 - line 0, 1 - line 1, ..., 31 - line 31)
	};

	std::array<uint16_t, 3> Value;
};

/// @brief LP5890 FC12 register structure
union FC12
{
	struct
	{
		uint32_t LedOpenLineWarning = 0; ///< @brief The line with LED open warnings (bit 0 - line 0, bit 1 - line 1, ..., bit 31 - line 31)
	};

	std::array<uint16_t, 3> Value;
};

/// @brief LP5890 FC13 register structure
union FC13
{
	struct
	{
		uint32_t LedShortLineWarning = 0; ///< @brief The line with LED short circuit warnings (bit 0 - line 0, bit 1 - line 1, ..., bit 31 - line 31)
	};

	std::array<uint16_t, 3> Value;
};

/// @brief LP5890 FC14 register structure
union FC14
{
	struct
	{
		uint64_t LedOpenLoadChannel : 48 = 0; ///< @brief The channel with LED open load warnings (bit 0 - channel 0, bit 1 - channel 1, ..., bit 47 - channel 47)
	};

	std::array<uint16_t, 3> Value;
};

/// @brief LP5890 FC15 register structure
union FC15
{
	struct
	{
		uint64_t LedShortCircuitChannel : 48; ///< @brief The channel with LED short circuit warnings (bit 0 - channel 0, bit 1 - channel 1, ..., bit 47 - channel 47)
	};

	std::array<uint16_t, 3> Value;
};

} // namespace LumiVoxel::Lp5890