/**
 * @file registers.hpp
 * @author Aidan Orr
 * @brief LP5890 LED driver register definitions
 * @version 0.1
 * 
 * @copyright Copyright (c) 2025
 */

#pragma once

#include <cstdint>
#include <array>

namespace LumiVoxel::Lp5890
{

/**
 * @brief LP5890 command IDs
 */
enum struct Command : uint16_t
{
	FC0_WRITE = 0xAA00, ///< @brief Write to the FC0 register
	FC1_WRITE = 0xAA01, ///< @brief Write to the FC1 register
	FC2_WRITE = 0xAA02, ///< @brief Write to the FC2 register
	FC3_WRITE = 0xAA03, ///< @brief Write to the FC3 register
	FC4_WRITE = 0xAA04, ///< @brief Write to the FC4 register
	FC5_WRITE = 0xAA05, ///< @brief Write to the FC5 register
	FC6_WRITE = 0xAA06, ///< @brief Write to the FC6 register
	FC7_WRITE = 0xAA07, ///< @brief Write to the FC7 register
	FC8_WRITE = 0xAA08, ///< @brief Write to the FC8 register
	FC9_WRITE = 0xAA09, ///< @brief Write to the FC9 register
	FC10_WRITE = 0xAA0A, ///< @brief Locate the line for LOD
	FC11_WRITE = 0xAA0B, ///< @brief Located the line for LSD

	CHIP_INDEX_WRITE = 0xAA10, ///< @brief Write to the Chip Index register
	VSYNC_WRITE = 0xAAF0, ///< @brief Write VSYNC command
	SOFT_RESET = 0xAA80, ///< @brief Reset all the registers except the SRAM
	
	SRAM_WRITE = 0xAA30, ///< @brief Write to the SRAM

	FC0_READ = 0xAA60, ///< @brief Read from the FC0 register
	FC1_READ = 0xAA61, ///< @brief Read from the FC1 register
	FC2_READ = 0xAA62, ///< @brief Read from the FC2 register
	FC3_READ = 0xAA63, ///< @brief Read from the FC3 register
	FC4_READ = 0xAA64, ///< @brief Read from the FC4 register
	FC5_READ = 0xAA65, ///< @brief Read from the FC5 register
	FC6_READ = 0xAA66, ///< @brief Read from the FC6 register
	FC7_READ = 0xAA67, ///< @brief Read from the FC7 register
	FC8_READ = 0xAA68, ///< @brief Read from the FC8 register
	FC9_READ = 0xAA69, ///< @brief Read from the FC9 register
	FC10_READ = 0xAA6A, ///< @brief Read from the FC10 register
	FC11_READ = 0xAA6B, ///< @brief Read from the FC11 register
	FC12_READ = 0xAA6C, ///< @brief Read from the FC12 register
	FC13_READ = 0xAA6D, ///< @brief Read from the FC13 register
	FC14_READ = 0xAA6E, ///< @brief Read from the FC14 register
	FC15_READ = 0xAA6F, ///< @brief Read from the FC15 register
	CHIP_INDEX_READ = 0xAA70, ///< @brief Read from the Chip Index register
}; // enum struct Command

/// @brief Represents a color value in the LP5890
struct Color
{
	uint16_t Blue; ///< @brief Blue component of the color
	uint16_t Green; ///< @brief Green component of the color
	uint16_t Red; ///< @brief Red component of the color
}; // struct Color

} // namespace LumiVoxel::Lp5890