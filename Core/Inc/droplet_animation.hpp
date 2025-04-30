/**
 * @file droplet_animation.hpp
 * @author Aidan Orr
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "animator.hpp"

#include <bitset>

namespace LumiVoxel
{

class DropletAnimation : public Animator
{
  private:
	float minSpawnDelay;
	float maxSpawnDelay;
	float minDropTime;
	float maxDropTime;

	size_t minTrailLength;
	size_t maxTrailLength;

	size_t minSpawnCount;
	size_t maxSpawnCount;

	uint64_t nextSpawnTime = 0;

	std::bitset<64> activeDroplets;
	std::array<uint64_t, 64> dropStartTimes;
	std::array<uint64_t, 64> dropEndTimes;
	std::array<size_t, 64> trailLengths;

	void SpawnDroplets(uint64_t now);

	uint64_t GetNextSpawnTime(uint64_t now)
	{
		int random = rand();
		float v = random / (float)RAND_MAX;
		return now + ((maxSpawnDelay - minSpawnDelay) * v + minSpawnDelay) * 1000000;
	}

	uint64_t GetNextDropEndTime(uint64_t now)
	{
		int random = rand();
		float v = random / (float)RAND_MAX;
		return now + ((maxDropTime - minDropTime) * v + minDropTime) * 1000000;
	}

	size_t GetNextTrailLength()
	{
		int random = rand();
		return random % (maxTrailLength - minTrailLength + 1) + minTrailLength;
	}

  public:
	DropletAnimation(
		float minSpawnTime,
		float maxSpawnTime,
		float minDropTime,
		float maxDropTime,
		size_t minTrailLength,
		size_t maxTrailLength,
		size_t minSpawnCount,
		size_t maxSpawnCount)
		: minSpawnDelay(minSpawnTime),
		  maxSpawnDelay(maxSpawnTime),
		  minDropTime(minDropTime),
		  maxDropTime(maxDropTime),
		  minTrailLength(minTrailLength),
		  maxTrailLength(maxTrailLength),
		  minSpawnCount(minSpawnCount),
		  maxSpawnCount(maxSpawnCount)
	{}

	bool Init(HighPrecisionCounter& hpCounter) override;

	void Update(HighPrecisionCounter& hpCounter, std::array<float, 512>& red, std::array<float, 512>& green, std::array<float, 512>& blue) override;

	~DropletAnimation() override {};

}; // class DropletAnimation

} // namespace LumiVoxel