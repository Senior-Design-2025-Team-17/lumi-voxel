/**
 * @file rubiks_cube_animation.hpp
 * @author Aidan Orr
 * @brief
 * @version 0.1
 *
 * @copyright Copyright (c) 2025
 */

#pragma once

#include "animator.hpp"

#include <array>

namespace LumiVoxel
{

class RubiksCubeAnimation : public Animator
{
  private:
	enum struct RotationAxis
	{
		X,
		Y,
		Z
	};

	enum struct RotationDirection
	{
		CW,
		CCW
	};

	enum struct RotationLayer
	{
		LAYER_1,
		LAYER_2
	};

  public:
	enum struct ColorName
	{
		EMPTY  = 0,
		YELLOW = 1,
		WHITE  = 2,
		RED    = 3,
		ORANGE = 4,
		GREEN  = 5,
		BLUE   = 6,
		EDGE   = 7,
	};

  private:
	static constexpr std::array<std::array<float, 3>, 8> ColorMap = {
		{ { 0.0f, 0.0f, 0.0f },
         { 1.0f, 0.5f, 0.0f },
         { 1.0f, 1.0f, 1.0f },
         { 1.0f, 0.0f, 0.0f },
         { 1.0f, 0.0f, 1.0f },
         { 0.0f, 1.0f, 0.0f },
         { 0.0f, 0.0f, 1.0f },
         { 0.125f, 0.125f, 0.125f } }
	};

	static constexpr std::array<uint8_t, 4> Ring90DegreeLengths = { 8, 6, 4, 2 };
	static constexpr std::array<uint8_t, 4> RingLengths = { 28, 20, 12, 4 };

	// clang-format off
	static constexpr std::array<uint8_t, 64> SlideRings = {
		0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 2, 2, 2, 2, 1, 0,
		0, 1, 2, 3, 3, 2, 1, 0,
		0, 1, 2, 3, 3, 2, 1, 0,
		0, 1, 2, 2, 2, 2, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
	};

	static constexpr std::array<uint8_t, 64> SlideRingReverse = {
		0, 1, 2, 3, 4, 5, 6, 7,
	   27, 0, 1, 2, 3, 4, 5, 8,
	   26,19, 0, 1, 2, 3, 6, 9,
	   25,18,11, 0, 1, 4, 7,10,
	   24,17,10, 3, 2, 5, 8,11,
	   23,16, 9, 8, 7, 6, 9,12,
	   22,15,14,13,12,11,10,13,
	   21,20,19,18,17,16,15,14,
   };

	static constexpr std::array<uint8_t, 28> SlideRing0 = {
		0, 1, 2, 3, 4, 5, 6, 7,
		15, 23, 31, 39, 47, 55, 63,
		62, 61, 60, 59, 58, 57, 56,
		48, 40, 32, 24, 16, 8
	};

	static constexpr std::array<uint8_t, 20> SlideRing1 = {
		9, 10, 11, 12, 13, 14,
		22, 30, 38, 46, 54,
		53, 52, 51, 50, 49,
		41, 33, 25, 17
	};

	static constexpr std::array<uint8_t, 12> SlideRing2 = {
		18, 19, 20, 21,
		29, 37, 45,
		44, 43, 42,
		34, 26
	};

	static constexpr std::array<uint8_t, 4> SlideRing3 = {
		27, 28,
		36, 35
	};
	// clang-format on

	static constexpr uint8_t GetRingIndex(size_t ring, size_t index)
	{
		switch (ring)
		{
		case 0:
			return SlideRing0[index];
		case 1:
			return SlideRing1[index];
		case 2:
			return SlideRing2[index];
		case 3:
			return SlideRing3[index];
		default:
			return 0;
		}
	}

	static constexpr size_t RotateLayerCount                              = 3;
	static constexpr std::array<uint8_t, RotateLayerCount> RotationLayer1 = { 7, 6, 5 };
	static constexpr std::array<uint8_t, RotateLayerCount> RotationLayer2 = { 0, 1, 2 };

	static void GetXLayer(size_t index, std::array<uint16_t, 64>& indices);
	static void GetYLayer(size_t index, std::array<uint16_t, 64>& indices);
	static void GetZLayer(size_t index, std::array<uint16_t, 64>& indices);

	static void GetLayer(RotationAxis axis, RotationLayer layer, size_t index, std::array<uint16_t, 64>& indices)
	{
		size_t layerIndex = (layer == RotationLayer::LAYER_1 ? RotationLayer1 : RotationLayer2)[index];

		switch (axis)
		{
		case RotationAxis::X:
			GetXLayer(layerIndex, indices);
			break;
		case RotationAxis::Y:
			GetYLayer(layerIndex, indices);
			break;
		case RotationAxis::Z:
			GetZLayer(layerIndex, indices);
			break;
		default:
			break;
		}
	}

	static RotationAxis GetNextRotationAxis()
	{
		int randomAxis = rand() % 3;
		switch (randomAxis)
		{
		case 0:
			return RotationAxis::X;
		case 1:
			return RotationAxis::Y;
		case 2:
			return RotationAxis::Z;
		default:
			return RotationAxis::Z;
		}
	}

	static RotationDirection GetNextRotationDirection()
	{
		int randomDir = rand() % 2;
		switch (randomDir)
		{
		case 0:
			return RotationDirection::CW;
		case 1:
			return RotationDirection::CCW;
		default:
			return RotationDirection::CW;
		}
	}

	static RotationLayer GetNextRotationLayer()
	{
		int randomLayer = rand() % 2;
		switch (randomLayer)
		{
		case 0:
			return RotationLayer::LAYER_1;
		case 1:
			return RotationLayer::LAYER_2;
		default:
			return RotationLayer::LAYER_1;
		}
	}

	float rotationTime;
	float rotationDelay;

	uint64_t rotationStartTime = 0;
	uint64_t rotationEndTime   = 0;

	RotationAxis rotationAxis           = RotationAxis::Z;
	RotationDirection rotationDirection = RotationDirection::CW;
	RotationLayer rotationLayer         = RotationLayer::LAYER_1;

	std::array<ColorName, 512> currentColors = {};
	std::array<ColorName, 512> rotatedColors = {};

	static void RotateLayer(float angle, std::array<uint16_t, 64>& indices, std::array<ColorName, 512>& inColors, std::array<ColorName, 512>& outColors);
	static void RotateLayer90(RotationDirection dir, std::array<uint16_t, 64>& indices, std::array<ColorName, 512>& inColors, std::array<ColorName, 512>& outColors);

	static void Rotate(float angle, RotationAxis axis, RotationDirection direction, RotationLayer layer, std::array<ColorName, 512>& inColors, std::array<ColorName, 512>& outColors);
	static void Rotate90(RotationAxis axis, RotationDirection direction, RotationLayer layer, std::array<ColorName, 512>& inColors, std::array<ColorName, 512>& outColors);

	static void DrawColors(const std::array<ColorName, 512>& colors, std::array<float, 512>& red, std::array<float, 512>& green, std::array<float, 512>& blue);

  public:
	RubiksCubeAnimation(
		float rotationTime,
		float rotationDelay)
		: rotationTime(rotationTime),
		  rotationDelay(rotationDelay) {};

	bool Init(LumiVoxel::HighPrecisionCounter& hpCounter) override;

	void Update(LumiVoxel::HighPrecisionCounter& hpCounter, std::array<float, 512>& red, std::array<float, 512>& green, std::array<float, 512>& blue) override;

	~RubiksCubeAnimation() override {};
};

} // namespace LumiVoxel