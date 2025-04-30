/**
 * @file droplet_animation.cpp
 * @author Aidan Orr
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2025
 */

#include "droplet_animation.hpp"

#include "high_precision_counter.hpp"

#include <cmath>
#include <cstdlib>

using namespace LumiVoxel;

void DropletAnimation::SpawnDroplets(uint64_t now)
{
	if (activeDroplets.all())
		return; // All droplets are already active

	int random = rand();
	float v    = random / (float)RAND_MAX;

	size_t dropIndex = 0;
	do
	{
		random    = rand();
		v         = random / (float)RAND_MAX;
		dropIndex = std::clamp((size_t)(v * 63), (size_t)0, (size_t)63);
	} while (activeDroplets[dropIndex]);

	activeDroplets[dropIndex] = true;
	dropStartTimes[dropIndex] = now;
	dropEndTimes[dropIndex]   = GetNextDropEndTime(now);
	trailLengths[dropIndex]   = GetNextTrailLength();
}

bool DropletAnimation::Init(HighPrecisionCounter& hpCounter)
{
	// Initialize the random number generator
	srand(hpCounter.GetCount());

	// Set the next spawn time to a random value between minSpawnDelay and maxSpawnDelay

	int random = rand();
	float v    = random / (float)RAND_MAX;

	nextSpawnTime = hpCounter.GetCount() + ((maxSpawnDelay - minSpawnDelay) * v + minSpawnDelay) * 1000000;

	return true;
}

void DropletAnimation::Update(HighPrecisionCounter& hpCounter, std::array<float, 512>& red, std::array<float, 512>& green, std::array<float, 512>& blue)
{
	uint64_t now = hpCounter.GetCount();
	if (now >= nextSpawnTime)
	{
		int spawnCount = rand() % (maxSpawnCount - minSpawnCount + 1) + minSpawnCount;
		for (int i = 0; i < spawnCount; ++i)
		{
			SpawnDroplets(now);
		}
		
		nextSpawnTime = GetNextSpawnTime(now);
	}

	red.fill(0.0f);
	green.fill(0.0f);
	blue.fill(0.0f);

	constexpr int startZ = 7;

	for (size_t i = 0; i < 64; ++i)
	{
		if (activeDroplets[i])
		{
			uint64_t start = dropStartTimes[i];

			size_t trailLength = trailLengths[i];
			int endZ           = -trailLengths[i];

			float progress = (now - start) / (float)(dropEndTimes[i] - start);

			if (progress >= 1.0f)
			{
				activeDroplets[i] = false;
				continue;
			}

			size_t x  = i % 8;
			size_t y  = (i / 8) % 8;
			int dropZ = (int)((endZ - startZ) * progress + startZ);

			for (size_t j = 0; j < trailLength; ++j)
			{
				int z = dropZ + j;
				if (z < 0 || z > 7)
					continue;

				float trailProgress = j / (float)trailLength;
				float brightness    = std::exp(-3 * trailProgress);

				size_t pos = Animator::ToIndex(x, y, z);

				red[pos]   = brightness;
				green[pos] = brightness;
				blue[pos]  = brightness;
			}
		}
	}
}