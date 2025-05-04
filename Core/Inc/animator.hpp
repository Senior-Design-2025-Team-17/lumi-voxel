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
	struct Vector3i
	{
		int32_t x;
		int32_t y;
		int32_t z;

		Vector3i Clamp(const Vector3i& min, const Vector3i& max) const
		{
			return {
				std::clamp(x, min.x, max.x),
				std::clamp(y, min.y, max.y),
				std::clamp(z, min.z, max.z),
			};
		}
	};

	struct Vector2f
	{
		float x;
		float y;

		Vector2f& Normalize()
		{
			float length = std::sqrt(x * x + y * y);
			if (length > 0.0f)
			{
				x /= length;
				y /= length;
			}

			return *this;
		}

		Vector2f Clamp(const Vector2f& min, const Vector2f& max) const
		{
			return {
				std::clamp(x, min.x, max.x),
				std::clamp(y, min.y, max.y),
			};
		}
	};

	struct Vector3f
	{
		float x;
		float y;
		float z;

		Vector3f& Normalize()
		{
			float length = std::sqrt(x * x + y * y + z * z);
			if (length > 0.0f)
			{
				x /= length;
				y /= length;
				z /= length;
			}

			return *this;
		}

		Vector3i Round() const
		{
			return { (int32_t)std::round(x), (int32_t)std::round(y), (int32_t)std::round(z) };
		}

		Vector3f Clamp(const Vector3f& min, const Vector3f& max) const
		{
			return {
				std::clamp(x, min.x, max.x),
				std::clamp(y, min.y, max.y),
				std::clamp(z, min.z, max.z),
			};
		}
	};

  public:

	static constexpr size_t ToIndex(size_t x, size_t y, size_t z)
	{
		x = std::clamp(x, (size_t)0, (size_t)7);
		y = std::clamp(y, (size_t)0, (size_t)7);
		z = std::clamp(z, (size_t)0, (size_t)7);

		return x + (y * 8) + (z * 64);
	}

	static constexpr size_t ToIndex(const Vector3i& vec)
	{
		return ToIndex(vec.x, vec.y, vec.z);
	}

	static constexpr std::tuple<size_t, size_t, size_t> ToXYZ(size_t index)
	{
		index = std::clamp(index, (size_t)0, (size_t)511);

		size_t x = index % 8;
		size_t y = (index / 8) % 8;
		size_t z = index / 64;

		return { x, y, z };
	}

	virtual bool Init(HighPrecisionCounter& hpCounter) { return true; };

	virtual void Update(HighPrecisionCounter& hpCounter, std::array<float, 512>& red, std::array<float, 512>& green, std::array<float, 512>& blue) {};

	virtual ~Animator() {};
};

} // namespace LumiVoxel