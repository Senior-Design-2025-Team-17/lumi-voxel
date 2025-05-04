#include "goldfish_animation.hpp"

#include <cstdlib>

using namespace LumiVoxel;

using Vector3f = Animator::Vector3f;
using Vector2f = Animator::Vector2f;
using Vector3i = Animator::Vector3i;

static void Wrap(float& x, float& vel, float min, float max)
{
	if (x < min)
	{
		vel = -vel;
		x   = min;
	}
	else if (x > max)
	{
		vel = -vel;
		x   = max;
	}
}

static float GetRandomFloat(float min, float max)
{
	float random = (float)rand() / (float)RAND_MAX;
	return min + random * (max - min);
}

Vector3f GoldfishAnimation::Goldfish::GetNextVelocity(float minSpeed, float maxSpeed)
{
	float dirX = GetRandomFloat(-1, 1);
	float dirY = GetRandomFloat(-1, 1);
	float dirZ = GetRandomFloat(-1, 1);

	Vector3f dir = { dirX, dirY, dirZ };
	dir.Normalize();

	float speed = GetRandomFloat(minSpeed, maxSpeed);

	float speedX = dir.x * speed;
	float speedY = dir.y * speed;
	float speedZ = dir.z * speed;

	return { speedX, speedY, speedZ };
}

uint64_t GoldfishAnimation::Goldfish::GetNextMoveTime(float minMoveTime, float maxMoveTime)
{
	return (uint64_t)(GetRandomFloat(minMoveTime, maxMoveTime) * HighPrecisionCounter::TimerFrequency);
}

std::pair<Vector3i, Vector2f> GoldfishAnimation::Goldfish::Update(HighPrecisionCounter& hpCounter)
{
	uint64_t now  = hpCounter.GetCount();
	int32_t delta = now - lastTime;

	float deltaTime = (float)delta / HighPrecisionCounter::TimerFrequency;

	position.x += velocity.x * deltaTime;
	position.y += velocity.y * deltaTime;
	position.z += velocity.z * deltaTime;

	Wrap(position.x, velocity.x, -0.5f, 7.5f);
	Wrap(position.y, velocity.y, -0.5f, 7.5f);
	Wrap(position.z, velocity.z, -0.5f, 7.5f);

	if (now > moveEndTime)
	{
		velocity    = GetNextVelocity(minSpeed, maxSpeed);
		moveEndTime = now + GetNextMoveTime(minMoveTime, maxMoveTime);
	}

	lastTime = now;

	Vector2f dir = { velocity.x, velocity.y };

	return {
		position.Round(),
		dir.Normalize()
	};
}

bool GoldfishAnimation::Init(HighPrecisionCounter& hpCounter)
{
	uint64_t now = hpCounter.GetCount();
	for (size_t i = 0; i < GoldfishCount; ++i)
	{
		goldfish[i] = Goldfish(minSpeed, maxSpeed, minMoveTime, maxMoveTime, now);

		goldfish[i].velocity    = Goldfish::GetNextVelocity(minSpeed, maxSpeed);
		goldfish[i].moveEndTime = now + Goldfish::GetNextMoveTime(minMoveTime, maxMoveTime);
		goldfish[i].lastTime    = now;
		goldfish[i].position    = { GetRandomFloat(0, 7), GetRandomFloat(0, 7), GetRandomFloat(0, 7) };
	}

	return true;
}

void GoldfishAnimation::Update(HighPrecisionCounter& hpCounter, std::array<float, 512>& red, std::array<float, 512>& green, std::array<float, 512>& blue)
{
	red.fill(WaterRedColor);
	green.fill(WaterGreenColor);
	blue.fill(WaterBlueColor);

	for (size_t i = 0; i < GoldfishCount; ++i)
	{
		constexpr Vector3i min = { 0, 0, 0 };
		constexpr Vector3i max = { 7, 7, 7 };
		auto [pos, dir] = goldfish[i].Update(hpCounter);

		Vector3i forwardPoint = ((Vector3f){ pos.x + dir.x, pos.y + dir.y, pos.z }).Round();
		Vector3i topPoint     = { pos.x, pos.y, pos.z + 1 };

		size_t centerIndex = Animator::ToIndex(pos.Clamp(min, max));
		red[centerIndex]   = GoldfishRedColor;
		green[centerIndex] = GoldfishGreenColor;
		blue[centerIndex]  = GoldfishBlueColor;

		size_t forwardIndex = Animator::ToIndex(forwardPoint.Clamp(min, max));
		red[forwardIndex]   = GoldfishRedColor;
		green[forwardIndex] = GoldfishGreenColor;
		blue[forwardIndex]  = GoldfishBlueColor;

		size_t topIndex = Animator::ToIndex(topPoint.Clamp(min, max));
		red[topIndex]   = GoldfishRedColor;
		green[topIndex] = GoldfishGreenColor;
		blue[topIndex]  = GoldfishBlueColor;
	}
}