/**
 * @file animator.hpp
 * @author Aidan Orr
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "high_precision_counter.hpp"

#include <algorithm>
#include <array>
#include <tuple>

namespace LumiVoxel
{

class Animator
{

  public:
	static constexpr size_t ToIndex(size_t x, size_t y, size_t z)
	{
		x = std::clamp(x, (size_t)0, (size_t)7);
		y = std::clamp(y, (size_t)0, (size_t)7);
		z = std::clamp(z, (size_t)0, (size_t)7);

		return x + (y * 8) + (z * 64);
	}

	static constexpr std::tuple<size_t, size_t, size_t> ToXYZ(size_t index)
	{
		index = std::clamp(index, (size_t)0, (size_t)511);

		size_t x = index % 8;
		size_t y = (index / 8) % 8;
		size_t z = index / 64;

		return { x, y, z };
	}

	virtual bool Init(HighPrecisionCounter& hpCounter) = 0;

	virtual void Update(HighPrecisionCounter& hpCounter, std::array<float, 512>& red, std::array<float, 512>& green, std::array<float, 512>& blue) = 0;

	virtual ~Animator() {};
};

} // namespace LumiVoxel