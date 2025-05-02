#include "meshwrapper.h"
#include "TriangleMesh.hpp"
#include "animator.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <span>

extern LumiVoxel::TriangleMesh<256> triangleMesh;

extern std::array<float, 512> red;
extern std::array<float, 512> green;
extern std::array<float, 512> blue;

extern float brightness;
extern uint32_t animationIndex;
extern LumiVoxel::HighPrecisionCounter hpCounter;
extern std::array<std::reference_wrapper<LumiVoxel::Animator>, 3> animations;

extern "C" void SetRainbowPresetColors();

extern "C" bool meshTransform(uint8_t* data_buffer, uint8_t Nb_bytes)
{
	Eigen::Matrix4f transform;

	for (int i = 0; i < 16; i++)
	{
		transform((int)(i / 4), i % 4) = *((float*)data_buffer + i);
	}

	triangleMesh.Transform(transform);
	triangleMesh.Rasterize<8, 8, 8>(red, green, blue);
	return true;
}

extern "C" bool meshTris(uint8_t* data_buffer, uint8_t Nb_bytes)
{
	// Allocate tris
	if (triangleMesh.AllocateTriangles(std::span<uint8_t>{ data_buffer, Nb_bytes }))
	{
		triangleMesh.Rasterize<8, 8, 8>(red, green, blue);
		return true;
	}
	return false;
}

extern "C" bool meshVerts(uint8_t* data_buffer, uint8_t Nb_bytes)
{
	return triangleMesh.AllocateVerts(std::span<float>{ (float*)data_buffer, Nb_bytes / sizeof(float) });
}

static void SetColor(float red, float green, float blue)
{
	std::array<float, 4 * triangleMesh.MaxVertexCount()> colors;

	for (size_t i = 0; i < triangleMesh.MaxVertexCount(); ++i)
	{
		colors[i * 4 + 0] = red;
		colors[i * 4 + 1] = green;
		colors[i * 4 + 2] = blue;
		colors[i * 4 + 3] = 1.0f;
	}

	triangleMesh.AllocateColors(colors);
}

extern "C" bool colorMode(uint8_t* data, uint8_t count)
{
	if (count != 8)
	{
		return false;
	}

	uint32_t colorMode     = *(uint32_t*)data;
	uint32_t animationMode = *(uint32_t*)(data + 4);

	switch (colorMode)
	{
	case 1:
		SetColor(1.0f, 0.0f, 0.0f);
		break;
	case 2:
		SetColor(0.0f, 1.0f, 0.0f);
		break;
	case 3:
		SetColor(0.0f, 0.0f, 1.0f);
		break;
	default:
		SetRainbowPresetColors();
		break;
	}

	animationIndex = animationMode;
	animations[animationIndex].get().Init(hpCounter);

	return true;
}