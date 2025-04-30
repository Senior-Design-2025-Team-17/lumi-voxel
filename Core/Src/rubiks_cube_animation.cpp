/**
 * @file rubiks_cube_animation.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2025
 */

#include "rubiks_cube_animation.hpp"

#include <cmath>
#include <numbers>

using namespace LumiVoxel;

static void DrawEdges(std::array<RubiksCubeAnimation::ColorName, 512>& currentColors)
{
	for (size_t i = 0; i < 8; ++i)
	{
		currentColors[Animator::ToIndex(i, 0, 0)] = RubiksCubeAnimation::ColorName::EDGE;
		currentColors[Animator::ToIndex(i, 7, 0)] = RubiksCubeAnimation::ColorName::EDGE;
		currentColors[Animator::ToIndex(i, 0, 7)] = RubiksCubeAnimation::ColorName::EDGE;
		currentColors[Animator::ToIndex(i, 7, 7)] = RubiksCubeAnimation::ColorName::EDGE;

		currentColors[Animator::ToIndex(0, i, 0)] = RubiksCubeAnimation::ColorName::EDGE;
		currentColors[Animator::ToIndex(7, i, 0)] = RubiksCubeAnimation::ColorName::EDGE;
		currentColors[Animator::ToIndex(0, i, 7)] = RubiksCubeAnimation::ColorName::EDGE;
		currentColors[Animator::ToIndex(7, i, 7)] = RubiksCubeAnimation::ColorName::EDGE;

		currentColors[Animator::ToIndex(0, 0, i)] = RubiksCubeAnimation::ColorName::EDGE;
		currentColors[Animator::ToIndex(7, 0, i)] = RubiksCubeAnimation::ColorName::EDGE;
		currentColors[Animator::ToIndex(0, 7, i)] = RubiksCubeAnimation::ColorName::EDGE;
		currentColors[Animator::ToIndex(7, 7, i)] = RubiksCubeAnimation::ColorName::EDGE;
	}
}

bool RubiksCubeAnimation::Init(HighPrecisionCounter& hpCounter)
{
	currentColors.fill(ColorName::EMPTY);

	// Initialize cube edges
	DrawEdges(currentColors);

	// Initialize cube faces
	// Yellow up face
	for (size_t i = 0; i < 36; ++i)
	{
		currentColors[ToIndex(1 + (i % 6), 1 + (i / 6), 7)] = ColorName::YELLOW;
		// currentColors[ToIndex(1 + (i % 6), 1 + (i / 6), 6)] = ColorName::YELLOW;
	}

	// White down face
	for (size_t i = 0; i < 36; ++i)
	{
		currentColors[ToIndex(1 + (i % 6), 1 + (i / 6), 0)] = ColorName::WHITE;
		// currentColors[ToIndex(1 + (i % 6), 1 + (i / 6), 1)] = ColorName::WHITE;
	}

	// Red right face
	for (size_t i = 0; i < 36; ++i)
	{
		currentColors[ToIndex(7, 1 + (i % 6), 1 + (i / 6))] = ColorName::RED;
		// currentColors[ToIndex(6, 1 + (i % 6), 1 + (i / 6))] = ColorName::RED;
	}

	// Orange left face
	for (size_t i = 0; i < 36; ++i)
	{
		currentColors[ToIndex(0, 1 + (i % 6), 1 + (i / 6))] = ColorName::ORANGE;
		// currentColors[ToIndex(1, 1 + (i % 6), 1 + (i / 6))] = ColorName::ORANGE;
	}

	// Green front face
	for (size_t i = 0; i < 36; ++i)
	{
		currentColors[ToIndex(1 + (i % 6), 7, 1 + (i / 6))] = ColorName::GREEN;
		// currentColors[ToIndex(1 + (i % 6), 6, 1 + (i / 6))] = ColorName::GREEN;
	}

	// Blue back face
	for (size_t i = 0; i < 36; ++i)
	{
		currentColors[ToIndex(1 + (i % 6), 0, 1 + (i / 6))] = ColorName::BLUE;
		// currentColors[ToIndex(1 + (i % 6), 1, 1 + (i / 6))] = ColorName::BLUE;
	}

	rotationStartTime = hpCounter.GetCount() + (uint64_t)(rotationDelay * HighPrecisionCounter::TimerFrequency);
	rotationEndTime   = rotationStartTime + (uint64_t)(rotationTime * HighPrecisionCounter::TimerFrequency);

	return true;
}

void RubiksCubeAnimation::Update(HighPrecisionCounter& hpCounter, std::array<float, 512>& red, std::array<float, 512>& green, std::array<float, 512>& blue)
{
	uint64_t now = hpCounter.GetCount();

	if (now < rotationStartTime)
	{
		// No rotation in progress
		DrawColors(currentColors, red, green, blue);
		return;
	}

	float rotationProgress = (float)(now - rotationStartTime) / (rotationEndTime - rotationStartTime);
	rotationProgress       = std::clamp(rotationProgress, 0.0f, 1.0f);

	if (rotationProgress >= 1.0f)
	{
		// Copy the rotated position to the colors array
		Rotate90(rotationAxis, rotationDirection, rotationLayer, currentColors, rotatedColors);

		currentColors = rotatedColors;

		rotationAxis = GetNextRotationAxis();
		rotationDirection = GetNextRotationDirection();
		rotationLayer = GetNextRotationLayer();

		rotationStartTime = now + (uint64_t)(rotationDelay * HighPrecisionCounter::TimerFrequency);
		rotationEndTime   = rotationStartTime + (uint64_t)(rotationTime * HighPrecisionCounter::TimerFrequency);
	}
	else
	{
		Rotate(rotationProgress * 90, rotationAxis, rotationDirection, rotationLayer, currentColors, rotatedColors);
	}

	DrawColors(rotatedColors, red, green, blue);
}

void RubiksCubeAnimation::GetXLayer(size_t index, std::array<uint16_t, 64>& indices)
{
	for (size_t i = 0; i < 8; ++i)
	{
		for (size_t j = 0; j < 8; ++j)
		{
			indices[i + j * 8] = ToIndex(index, i, j);
		}
	}
}

void RubiksCubeAnimation::GetYLayer(size_t index, std::array<uint16_t, 64>& indices)
{
	for (size_t i = 0; i < 8; ++i)
	{
		for (size_t j = 0; j < 8; ++j)
		{
			indices[i + j * 8] = ToIndex(i, index, j);
		}
	}
}

void RubiksCubeAnimation::GetZLayer(size_t index, std::array<uint16_t, 64>& indices)
{
	for (size_t i = 0; i < 8; ++i)
	{
		for (size_t j = 0; j < 8; ++j)
		{
			indices[i + j * 8] = ToIndex(i, j, index);
		}
	}
}

struct Vector2i
{
	int x;
	int y;
};

struct Vector2f
{
	float x;
	float y;
};

static int Round(float value)
{
	return static_cast<int>(std::round(value));
}

static Vector2i ShearX(Vector2i p, Vector2f c, float shear)
{
	int x = p.x + Round((p.y - c.y) * shear);
	int y = p.y;

	return { x, y };
}

static Vector2i ShearY(Vector2i p, Vector2f c, float shear)
{
	int x = p.x;
	int y = p.y + Round((p.x - c.x) * shear);

	return { x, y };
}

constexpr float centerX = 3.4f;
constexpr float centerY = 3.4f;

void RubiksCubeAnimation::RotateLayer(float angle, std::array<uint16_t, 64>& indices, std::array<ColorName, 512>& inColors, std::array<ColorName, 512>& outColors)
{
	constexpr float degToRad = std::numbers::pi_v<float> / 180.0f;

	Vector2f center = { centerX, centerY };

	float progress = angle / 90.0f;

	float s1 = -std::tan(angle * degToRad / 2);
	float s2 = std::sin(angle * degToRad);

	for (size_t i = 0; i < 64; ++i)
	{
		size_t startIndex = indices[i];

		Vector2i p = { (int)(i % 8), (int)((i / 8) % 8) };

		Vector2i p1 = ShearX(p, center, s1);
		Vector2i p2 = ShearY(p1, center, s2);
		Vector2i p3 = ShearX(p2, center, s1);

		if (progress > 0)
			p3.x = Round(p3.x + progress);
		else
			p3.y = Round(p3.y - progress);

		if (p3.x < 0 || p3.x >= 8 || p3.y < 0 || p3.y >= 8)
		{
			outColors[startIndex] = ColorName::EMPTY;
		}
		else
		{
			size_t newIndex     = indices[(size_t)p3.x + (size_t)p3.y * 8];
			outColors[newIndex] = inColors[startIndex];
		}
	}
}

void RubiksCubeAnimation::Rotate(float angle, RotationAxis axis, RotationDirection direction, RotationLayer layer, std::array<ColorName, 512>& inColors, std::array<ColorName, 512>& outColors)
{
	outColors = inColors;
	for (size_t i = 0; i < RotateLayerCount; ++i)
	{
		std::array<uint16_t, 64> indices;
		GetLayer(axis, layer, i, indices);
		RotateLayer(angle * (direction == RotationDirection::CW ? 1 : -1), indices, inColors, outColors);
	}
}

void RubiksCubeAnimation::RotateLayer90(RotationDirection dir, std::array<uint16_t, 64>& indices, std::array<ColorName, 512>& inColors, std::array<ColorName, 512>& outColors)
{
	Vector2f center = { centerX, centerY };

	float progress = dir == RotationDirection::CW ? 1 : -1;

	float s1 = dir == RotationDirection::CW ? -1 : 1;

	for (size_t i = 0; i < 64; ++i)
	{
		size_t startIndex = indices[i];

		Vector2i p = { (int)(i % 8), (int)((i / 8) % 8) };
		Vector2i p3 = p;

		if (dir == RotationDirection::CCW)
		{
			p3.x = Round((p.y - center.y) + center.x);
			p3.y = Round(-(p.x - center.x) + center.y);
		}
		else
		{
			p3.x = Round(-(p.y - center.y) + center.x);
			p3.y = Round((p.x - center.x) + center.y);
		}

		size_t newIndex     = indices[(size_t)p3.x + (size_t)p3.y * 8];
		outColors[newIndex] = inColors[startIndex];
	}
}

void RubiksCubeAnimation::Rotate90(RotationAxis axis, RotationDirection direction, RotationLayer layer, std::array<ColorName, 512>& inColors, std::array<ColorName, 512>& outColors)
{
	outColors = inColors;
	for (size_t i = 0; i < RotateLayerCount; ++i)
	{
		std::array<uint16_t, 64> indices;
		GetLayer(axis, layer, i, indices);
		RotateLayer90(direction, indices, inColors, outColors);
		DrawEdges(outColors);
	}
}

void RubiksCubeAnimation::DrawColors(const std::array<ColorName, 512>& colors, std::array<float, 512>& red, std::array<float, 512>& green, std::array<float, 512>& blue)
{
	for (size_t i = 0; i < 512; ++i)
	{
		ColorName color        = colors[i];
		const auto& colorValue = ColorMap[static_cast<size_t>(color)];

		red[i]   = colorValue[0];
		green[i] = colorValue[1];
		blue[i]  = colorValue[2];
	}
}