/**
 * @file mappings.hpp
 * @author Aidan Orrr
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include <array>
#include <functional>
#include <tuple>

#include "lp5890.hpp"

namespace LumiVoxel::Lp5890
{

struct DriverMapping
{
  private:
	static constexpr Driver* Empty = nullptr;

  public:
	std::reference_wrapper<Driver> LedDriver;
	size_t Index;

	/// @brief Default constructor for DriverMapping
	/// @warning This is only used internally to allow default construction, but should not be used in user code.
	constexpr DriverMapping() : LedDriver(std::ref(*Empty)), Index(0) {}

	constexpr DriverMapping(std::reference_wrapper<Driver> driver, size_t index) : LedDriver(driver), Index(index) {}

	// clang-format off
	static constexpr std::array<size_t, 256> LocalToDeviceMapping = {
		255, 254, 252, 253, 251, 250, 249, 248, // 0 - 7
		127, 126, 124, 125, 123, 122, 121, 120, // 8 - 15
		240, 241, 242, 243, 244, 245, 246, 247, // 16 - 23
		112, 113, 114, 115, 116, 117, 118, 119, // 24 - 31
		239, 238, 236, 237, 235, 234, 233, 232, // 32 - 39
		111, 110, 108, 109, 107, 106, 105, 104, // 40 - 47
		224, 225, 226, 227, 228, 229, 230, 231, // 48 - 55
		 96,  97,  98,  99, 100, 101, 102, 103, // 56 - 63
		223, 222, 220, 221, 219, 218, 217, 216, // 64 - 71
		 95,  94,  92,  93,  91,  90,  89,  88, // 72 - 79
		208, 209, 210, 211, 212, 213, 214, 215, // 80 - 87
		 80,  81,  82,  83,  84,  85,  86,  87, // 88 - 95
		207, 206, 204, 205, 203, 202, 201, 200, // 96 - 103
		 79,  78,  76,  77,  75,  74,  73,  72, // 104 - 111
		192, 193, 194, 195, 196, 197, 198, 199, // 112 - 119
		 64,  65,  66,  67,  68,  69,  70,  71, // 120 - 127
		191, 190, 188, 189, 187, 186, 185, 184, // 128 - 135
		 63,  62,  60,  61,  59,  58,  57,  56, // 136 - 143
		176, 177, 178, 179, 180, 181, 182, 183, // 144 - 151
		 48,  49,  50,  51,  52,  53,  54,  55, // 152 - 159
		175, 174, 172, 173, 171, 170, 169, 168, // 160 - 167
		 47,  46,  44,  45,  43,  42,  41,  40, // 168 - 175
		160, 161, 162, 163, 164, 165, 166, 167, // 176 - 183
		 32,  33,  34,  35,  36,  37,  38,  39, // 184 - 191
		159, 158, 156, 157, 155, 154, 153, 152, // 192 - 199
		 31,  30,  28,  29,  27,  26,  25,  24, // 200 - 207
		144, 145, 146, 147, 148, 149, 150, 151, // 208 - 215
		 16,  17,  18,  19,  20,  21,  22,  23, // 216 - 223
		143, 142, 140, 141, 139, 138, 137, 136, // 224 - 231
		 15,  14,  12,  13,  11,  10,   9,   8, // 232 - 239
		128, 129, 130, 131, 132, 133, 134, 135, // 240 - 247
		  0,   1,   2,   3,   4,   5,   6,   7  // 248 - 255
	};
	// clang-format on
};

constexpr std::array<DriverMapping, 512> CreateDriverMappings(Driver& driver1, Driver& driver2)
{
	std::array<std::reference_wrapper<Driver>, 2> drivers = { std::ref(driver1), std::ref(driver2) };

	std::array<DriverMapping, 512> mappings;

	for (size_t i = 0; i < 512; ++i)
	{
		std::array<size_t, 3> coords = { i % 8, (i / 8) % 8, (i / 64) % 8 };

		// Y - [0:3] = driver1, [4:7] = driver2
		std::reference_wrapper<Driver> driver = drivers[coords[1] < 4 ? 0 : 1];

		std::array<size_t, 3> driverCoords = { coords[0], coords[1] % 4, coords[2] };
		size_t localIndex                  = driverCoords[0] + (driverCoords[1] * 8) + (driverCoords[2] * 32);

		size_t index = DriverMapping::LocalToDeviceMapping[localIndex];

		mappings[i] = DriverMapping(driver, index);
	}

	return mappings;
}

} // namespace LumiVoxel::Lp5890