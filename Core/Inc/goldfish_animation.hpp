/**
 * @file goldfish_animation.hpp
 * @author Aidan Orr
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "animator.hpp"

#include <cmath>
#include <tuple>

namespace LumiVoxel
{

	using Vector3f = Animator::Vector3f;
	using Vector2f = Animator::Vector2f;
	using Vector3i = Animator::Vector3i;

class GoldfishAnimation : public Animator
{
  public:
	static constexpr size_t GoldfishCount = 4;

  private:

	struct Goldfish
	{
	  private:
		float minSpeed;
		float maxSpeed;

		float minMoveTime;
		float maxMoveTime;

	  public:
		Vector3f velocity = { 0, 0, 0 };
		Vector3f position = { 0, 0, 0 };

		uint64_t lastTime    = 0;
		uint64_t moveEndTime = 0;

		static Vector3f GetNextVelocity(float minSpeed, float maxSpeed);
		static uint64_t GetNextMoveTime(float minMoveTime, float maxMoveTime);

	  public:
		constexpr Goldfish()
			: minSpeed(0.0f),
			  maxSpeed(0.0f),
			  minMoveTime(0.0f),
			  maxMoveTime(0.0f)
		{}

		Goldfish(float minSpeed, float maxSpeed, float minMoveTime, float maxMoveTime, uint64_t time)
			: minSpeed(minSpeed),
			  maxSpeed(maxSpeed),
			  minMoveTime(minMoveTime),
			  maxMoveTime(maxMoveTime),
			  lastTime(time)
		{}

		std::pair<Vector3i, Vector2f> Update(HighPrecisionCounter& hpCounter);
	};

	float minSpeed;
	float maxSpeed;

	float minMoveTime;
	float maxMoveTime;

	std::array<Goldfish, GoldfishCount> goldfish = { Goldfish() };

	static constexpr float GoldfishRedColor   = 1.0f;
	static constexpr float GoldfishGreenColor = 0.5f;
	static constexpr float GoldfishBlueColor  = 0.0f;

	static constexpr float WaterRedColor   = 0.0f;
	static constexpr float WaterGreenColor = 0.25f;
	static constexpr float WaterBlueColor  = 0.5f;

  public:
	GoldfishAnimation(float minSpeed, float maxSpeed, float minMoveTime, float maxMoveTime)
		: minSpeed(minSpeed),
		  maxSpeed(maxSpeed),
		  minMoveTime(minMoveTime),
		  maxMoveTime(maxMoveTime)
	{}

	bool Init(HighPrecisionCounter& hpCounter) override;

	void Update(HighPrecisionCounter& hpCounter, std::array<float, 512>& red, std::array<float, 512>& green, std::array<float, 512>& blue) override;
};

} // namespace LumiVoxel
