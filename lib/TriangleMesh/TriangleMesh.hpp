#ifndef _TRIANGLE_MESH_HPP
#define _TRIANGLE_MESH_HPP

#include "animator.hpp"
#include "helper.hpp"

#include <Eigen/Core>

#include <algorithm>
#include <functional>
#include <optional>
#include <span>
#include <vector>

#ifndef STM32_PROCESSOR
#include <chrono>
#include <iostream>
#endif

/// @brief Struct to store information regarding the triangle mesh Status
enum struct Status : uint8_t
{
	None               = 0,
	VertsAllocated     = 1 << 0,
	TrianglesAllocated = 1 << 1,
	ColorsAllocated    = 1 << 2,
};

constexpr Status operator&(Status l, Status r)
{
	return static_cast<Status>(static_cast<uint8_t>(l) & static_cast<uint8_t>(r));
}

constexpr Status operator|(Status l, Status r)
{
	return static_cast<Status>(static_cast<uint8_t>(l) | static_cast<uint8_t>(r));
}

inline Status& operator|=(Status& l, Status r)
{
	return l = l | r;
}

inline Status& operator&=(Status& l, Status r)
{
	return l = l & r;
}

constexpr Status operator~(Status v)
{
	return static_cast<Status>(~static_cast<uint8_t>(v));
}

/// @brief Struct to store information regarding the triangle mesh options for drawing
enum struct DrawOptions : uint8_t
{
	None              = 0,
	DrawVerticies     = 1 << 0,
	DrawEdges         = 1 << 1,
	DrawFill          = 1 << 2,
	ProjectToUnitCube = 1 << 3,
	Round_trunc       = 1 << 4,
	Clamp             = 1 << 5,
};

constexpr DrawOptions operator&(DrawOptions l, DrawOptions r)
{
	return static_cast<DrawOptions>(static_cast<uint8_t>(l) & static_cast<uint8_t>(r));
}

constexpr DrawOptions operator|(DrawOptions l, DrawOptions r)
{
	return static_cast<DrawOptions>(static_cast<uint8_t>(l) | static_cast<uint8_t>(r));
}

inline DrawOptions& operator|=(DrawOptions& l, DrawOptions r)
{
	return l = l | r;
}

inline DrawOptions& operator&=(DrawOptions& l, DrawOptions r)
{
	return l = l & r;
}

constexpr DrawOptions operator~(DrawOptions v)
{
	return static_cast<DrawOptions>(~static_cast<uint8_t>(v));
}

namespace LumiVoxel
{

/**
 * @brief Class representing a triangle mesh object with the ability to manipulate the object in 3D space before discretizing/rasterizing
 * the object to display on some LED cube matrix. A constraint can be set to the max number of verticies expected by this mesh object.
 *
 * @todo Where the hell should projectToUnitCube be set up. Figure that out. Also, document this class better. Clamping also straight up does not work, whoops
 *
 * Known Issues: What happens when x,y,z are less than one. Taking the max_coeff will lead to 1 because of the last componenet of the matrix
 *
 * The TriangleMesh can be dispayed with a call to Rasterize given proper initialization has been performed and DrawOptions has been set.
 */
template <size_t maxVertNum = 512>
class TriangleMesh : public Animator
{
  public:
	constexpr size_t MaxVertexCount() const { return maxVertNum; }

  private:
	/// @brief Position of each verticies loaded after call to AllocateVerts(), stored as x,y,z
	Eigen::Matrix<float, 4, Eigen::Dynamic, 0, 4, maxVertNum> verts;

	/// @brief Current position of the verticies
	Eigen::Matrix<float, 4, Eigen::Dynamic, 0, 4, maxVertNum> verts_tranformed;

	/// @brief Index position of triangles
	Eigen::Matrix<int, 3, Eigen::Dynamic, 0, 3, maxVertNum> triangles;

	/// @brief Color of each vertex, each column matches the vertex location in the same column of verts
	Eigen::Matrix<float, 4, Eigen::Dynamic, 0, 4, maxVertNum> colors;

	int numOfTriangles = 0; // number of triangles represented in the mesh

	/// @brief Base color to reset LEDs to. Default is zero
	Eigen::Vector4f fillColor = Eigen::Vector4f(0.0f, 0.0f, 0.0f, 1.0f);

	/// @brief Tracks the current state of the mesh. Alter state through calls to allocate functions and setProjectionSpace()
	Status meshState = Status::None;

	/// @brief Drawing options for rasterization and different drawing functions. The draw vertex, edge, and fill modes can be set together and will be performed together. However, setting any of these with the test options will cause them to be overridden. Calling multiple tests together will result in undefined behavior
	DrawOptions drawOptions = DrawOptions::None;

	/// @brief Number of verticies contained within the triangle mesh
	int meshVertexNum = 0;

	bool hasChanged = false;

	/**
	 * @brief Defined the clamping behavior when points lie beyond the boundaries defined
	 *
	 * By default no action is taking if the point is beyond the projection boundary
	 * If the clamp bit is set then any points lying beyond a boundary will be projected back onto the bounary
	 *
	 *
	 * @param v (x,y,z,1) vector to clamp
	 * @return std::optional which contains the clamped value if clamping is performed or no action is needed, and returns nothing if
	 * the point lies beyond the boundary and should not be clamped.
	 */
	template <size_t x = 8, size_t y = 8, size_t z = 8>
	std::optional<Eigen::Vector4f> Clamp(const Eigen::Vector4f& v)
	{
		if (((v[0] >= 0) && (v[0] < x)) && ((v[1] >= 0) && (v[1] < y)) && ((v[2] >= 0) && (v[2] < z)))
		{
			return v;
		}
		else
		{
			if ((drawOptions & DrawOptions::Clamp) == DrawOptions::Clamp)
			{
				Eigen::Vector4f r;

				r[0] = std::clamp((float)v[0], 0.0f, (float)x - 1);
				r[1] = std::clamp((float)v[1], 0.0f, (float)y - 1);
				r[2] = std::clamp((float)v[2], 0.0f, (float)z - 1);
				r[3] = 1.0f;

				return r;
			}
			return std::optional<Eigen::Vector4f>();
		}
	}

	/**
	 * @brief Rounds a point based on the drawing options bits set. If Round_trunc is set, then it will simply cast to int. If not, then it will
	 * round normally
	 *
	 * @param v (x,y,z,1) vector to test
	 * @return Rounded Vector
	 */
	Eigen::Vector4i Round(const Eigen::Vector4f& v)
	{
		if ((drawOptions & DrawOptions::Round_trunc) == DrawOptions::Round_trunc)
		{
			return Eigen::Vector4f(v).cast<int>();
		}
		else
		{

			Eigen::Vector4f ret = Eigen::Vector4f(v);

			ret[0] = std::round(ret[0]);
			ret[1] = std::round(ret[1]);
			ret[2] = std::round(ret[2]);
			ret[3] = 1.0f;

			return ret.cast<int>();
		}
	}

  public:
	constexpr TriangleMesh()
	{}

	bool Init(HighPrecisionCounter& hpCounter) override
	{
		hasChanged = true;
		return true;
	}

	/**
	 * @brief sets all of the colors in the LED array to a specific value
	 *
	 * @param LEDNum Number of LEDs in the array
	 * @param blueMatrix XxYxZ float matrix to represent the blue component of the LED matrix.
	 * @param greenMatrix XxYxZ float matrix to represent the green component of the LED matrix.
	 * @param redMatrix XxYxZ float matrix to represent the red component of the LED matrix.
	 *
	 * @param color Eigen Vector representing the color to set all LEDs to
	 */
	template <size_t LEDNum>
	void SetColor(std::array<float, LEDNum>& blueMatrix, std::array<float, LEDNum>& greenMatrix, std::array<float, LEDNum>& redMatrix, Eigen::Vector4f color)
	{
		for (int index = 0; index < LEDNum; index++)
		{
			redMatrix[index]   = color[0] * color[3];
			greenMatrix[index] = color[1] * color[3];
			blueMatrix[index]  = color[2] * color[3];
		}
	}

	/**
	 * @brief Sets the fill color of the mesh when displaying nothing
	 * @param color fill color
	 */
	void SetFillColor(Eigen::Vector4f color)
	{
		fillColor = color;
	}

	/**
	 * @brief Set drawing options bits in the struct. Multiple drawing options can be set together, and will be
	 * performed sequentially. For instance, drawing all verticies, then triangle edges.
	 *
	 *   DrawVerticies: Draw each vertex in the vertex array. Must have vertex and colors allocated
	 *   DrawEdges: Draws all of the edges in the triangle array. Must have vertex, colors, and triangles allocated
	 *   DrawFill: Draws all of the colors in fill mode. Must have vertex, colors, and triangles allocated
	 *   ProjectToUnitCube: Projects initial triangle mesh to unit cube and then scales during rasterization. Tradeoff between more space to transform, and more detail of the rasterization
	 *   Round_trunc: If set it will truncate to round to an LED value. By default, it will perform defailt rounding
	 *   Clamp: If set, any vertex or line beyond the edges of the projection box will be pushed to the surface edges still being visible. Normal behavior will not draw these points that lie beyone the projection box
	 *
	 * @param d pattern to add to the struct
	 */

	void SetDrawOptions(DrawOptions d)
	{
		if ((d & DrawOptions::ProjectToUnitCube) == DrawOptions::ProjectToUnitCube)
		{
			// std::array<int, 3> ind{ 0, 1, 2 };
			// for (int i : ind)
			// {
			// 	verts_tranformed(i, Eigen::all) = verts_tranformed(i, Eigen::all) / verts_tranformed.maxCoeff();
			// }
		}
		else
		{
			verts_tranformed = verts;
		}
		drawOptions |= d;
	}

	/**
	 * @brief Clears the drawing options. Should always be called when attempting to modify the options of unknown value
	 *
	 * @param option draw option to be cleared
	 */

	void ClearDrawOption(DrawOptions option)
	{
		if ((option & DrawOptions::ProjectToUnitCube) == DrawOptions::ProjectToUnitCube)
		{
			verts_tranformed = verts / verts.maxCoeff();
		}
		else
		{
			verts_tranformed = verts;
		}

		drawOptions &= (~option);
	}

	/**
	 * @brief Allocates all of the verticies into a Eigen Matrix for simple and fast manipulation
	 *
	 * @param vertLength Number of elements in verts_sequential
	 * @param verts_sequential vector of verticies positions sequentially stored with three floats
	 *
	 * @return bool indicating if allocation was successful
	 */
	bool AllocateVerts(std::span<float> verts_sequential)
	{
		size_t vertLength = verts_sequential.size();
		if ((vertLength % 3) != 0) {
			return false;
		}

		verts.resize(4, (int)(vertLength / 3));
		meshVertexNum = (int)(vertLength / 3);

		for (int i = 0; i < vertLength; i++)
		{
			(verts)(i % 3, (int)(i / 3)) = verts_sequential.data()[i];
			(verts)(3, (int)(i / 3))     = 1;
		}

		if ((drawOptions & DrawOptions::ProjectToUnitCube) == DrawOptions::ProjectToUnitCube)
		{
			Eigen::Vector4f vertMin = std::numeric_limits<float>::max() * Eigen::Vector4f::Ones();
			vertMin[3]              = 1.0f; // Set the homogeneous coordinate to 1.0
			for (int i = 0; i < meshVertexNum; i++)
			{
				vertMin = vertMin.cwiseMin(verts.col(i));
			}

			Eigen::Vector4f vertMax = std::numeric_limits<float>::lowest() * Eigen::Vector4f::Ones();
			vertMax[3]              = 1.0f; // Set the homogeneous coordinate to 1.0

			for (int i = 0; i < meshVertexNum; i++)
			{
				vertMax = vertMax.cwiseMax(verts.col(i));
			}

			Eigen::Vector4f diff = vertMax - vertMin;

			float maxDiff = diff.maxCoeff();

			// for (size_t i = 0; i < verts.cols(); i++)
			// {
			// 	verts.col(i) = verts.col(i) - vertMin + Eigen::Vector4f(0.5f, 0.5f, 0.5f, 0.0f);
			// }
			Eigen::Matrix4f scale = Eigen::Matrix4f::Identity();
			scale(0, 0)           = 1.0f / maxDiff;
			scale(1, 1)           = 1.0f / maxDiff;
			scale(2, 2)           = 1.0f / maxDiff;
			scale(3, 3)           = 1.0f / maxDiff;

			verts = scale * verts;
		}

		verts_tranformed = verts;

		if (verts_tranformed.data() != nullptr)
		{
			meshState |= Status::VertsAllocated;
		}

		// hasChanged = true;
		return (verts_tranformed.data() != nullptr);
	}

	/**
	 * @brief Allocates all of the colors into a Eigen Matrix
	 *
	 * @param colors_sequential vector of colors sequentially stored with three floats
	 *
	 * @return bool indicating if allocation was successful
	 */
	bool AllocateColors(std::span<float> colors_sequential)
	{
		size_t colorLength = colors_sequential.size();

		if ((colorLength % 4) != 0) {
			return false;
		}

		colors.resize(4, (int)(colorLength / 4));

		for (int i = 0; i < colorLength; i++)
		{
			(colors)(i % 4, (int)(i / 4)) = colors_sequential.data()[i];
		}

		if (colors.data() != nullptr)
		{
			meshState |= Status::ColorsAllocated;
		}

		hasChanged = true;
		return (colors.data() != nullptr);
	}

	/**
	 * @brief Allocates all of the triangle into a Eigen Matrix
	 *
	 * @param trianglesLength 3 * num of triangles - length of triangles_sequential
	 * @param triangles_sequential vector of colors sequentially stored with three floats
	 *
	 * @return bool indicating if allocation was successful
	 */
	bool AllocateTriangles(std::span<uint8_t> triangles_sequential)
	{
		size_t trianglesLength = triangles_sequential.size();

		if ((trianglesLength % 3) != 0) {
			return false;
		}

		numOfTriangles = (int)(trianglesLength / 3);
		triangles.resize(3, numOfTriangles);

		for (int i = 0; i < trianglesLength; i++)
		{
			(triangles)(i % 3, (int)(i / 3)) = triangles_sequential.data()[i];
		}

		if (triangles.data() != nullptr)
		{
			meshState |= Status::TrianglesAllocated;
		}

		hasChanged = true;
		return (triangles.data() != nullptr);
	}

	/**
	 * @brief Applies the given transformation to the triangle mesh and stores this in verts_transformed
	 *
	 * @param transform 4x4 transformation matrix to be applies
	 *
	 */
	void Transform(Eigen::Matrix<float, 4, 4>& transform)
	{
		verts_tranformed = transform * verts;
		hasChanged       = true;
	}

  private:
	template <size_t sx, size_t sy, size_t sz>
	void DrawEdge(std::array<float, sx * sy * sz>& blueMatrix, std::array<float, sx * sy * sz>& greenMatrix, std::array<float, sx * sy * sz>& redMatrix, Eigen::Vector4f v1, Eigen::Vector4f v2, Eigen::Vector4f c1, Eigen::Vector4f c2)
	{
		constexpr size_t maxSteps = (size_t)std::ceil(std::sqrt(sx * sx + sy * sy + sz * sz));

		for (size_t i = 0; i < maxSteps; ++i)
		{
			float t           = (float)i / (float)(maxSteps - 1);
			Eigen::Vector4f v = v1 * (1.0f - t) + v2 * t;
			Eigen::Vector4f c = c1 * (1.0f - t) + c2 * t;

			size_t x = (size_t)std::round(std::clamp(v[0], 0.0f, (float)sx - 1));
			size_t y = (size_t)std::round(std::clamp(v[1], 0.0f, (float)sy - 1));
			size_t z = (size_t)std::round(std::clamp(v[2], 0.0f, (float)sz - 1));

			size_t index = x + y * sx + z * sx * sy;
			if (index < sx * sy * sz)
			{
				redMatrix[index]   = c[0] * c[3];
				greenMatrix[index] = c[1] * c[3];
				blueMatrix[index]  = c[2] * c[3];
			}
		}
	}

  public:
	void Update(HighPrecisionCounter& hpCounter, std::array<float, 512>& red, std::array<float, 512>& green, std::array<float, 512>& blue) override
	{
		if (hasChanged)
		{
			Rasterize<8, 8, 8>(red, green, blue);
			hasChanged = false;
		}
	}

	/**
	 * @brief Draws triangles on the LED matrix. The dimension of the LED matrix is templated, and the total number of LEDs in the matrix is
	 * given by multiplying all template parameters together. The output colors channels should match this dimension.
	 *
	 * For Triangle fill, bilinear interpolation is used to determine the color
	 * For Edge drawing a simple line interpolation method is used to determine the color
	 * For Vertex drawing, there is no interpolation
	 *
	 * @param redMatrix XxYxZ float matrix to represent the red component of the LED matrix.
	 * @param greenMatrix XxYxZ float matrix to represent the green component of the LED matrix.
	 * @param blueMatrix XxYxZ float matrix to represent the blue component of the LED matrix.
	 *
	 * @return true if sucessfully wrote to color arrays
	 */
	template <size_t x, size_t y, size_t z>
	bool Rasterize(std::array<float, x * y * z>& redMatrix, std::array<float, x * y * z>& greenMatrix, std::array<float, x * y * z>& blueMatrix)
	{
		int scale = (std::max(x, std::max(y, z)) - 1);

		const int LEDNum = x * y * z;

		SetColor<LEDNum>(blueMatrix, greenMatrix, redMatrix, fillColor);

		// Filled Mode -> Does not guarentee to lie on the same plane as three points due to rasterization
		if ((drawOptions & DrawOptions::DrawFill) == DrawOptions::DrawFill)
		{
			if ((meshState & (Status::TrianglesAllocated | Status::ColorsAllocated | Status::VertsAllocated)) != (Status::TrianglesAllocated | Status::ColorsAllocated | Status::VertsAllocated))
			{
#ifndef STM32_PROCESSOR
				throw MeshInitializationException(meshState, (Status::TrianglesAllocated | Status::ColorsAllocated | Status::VertsAllocated));
#endif

				return false;
			}
			// iterate over all triangles
			for (int triangleNum = 0; triangleNum < numOfTriangles; triangleNum++)
			{
				Eigen::Vector4f v1 = verts_tranformed(Eigen::all, triangles(0, triangleNum));
				Eigen::Vector4f v2 = verts_tranformed(Eigen::all, triangles(1, triangleNum));
				Eigen::Vector4f v3 = verts_tranformed(Eigen::all, triangles(2, triangleNum));

				Eigen::Vector4f c1 = colors(Eigen::all, triangles(0, triangleNum));
				Eigen::Vector4f c2 = colors(Eigen::all, triangles(1, triangleNum));
				Eigen::Vector4f c3 = colors(Eigen::all, triangles(2, triangleNum));

				int maxNumOfSteps = (int)(std::max(std::max(x, y), z) * sqrt(3));

				Eigen::Vector4f v1_v2 = v1;
				Eigen::Vector4f c1_c2 = c1;

				for (int i = 0; i < maxNumOfSteps; i++)
				{
					Eigen::Vector4f v1_v2_v3 = v1_v2;
					Eigen::Vector4f c1_c2_c3 = c1_c2;

					for (int i = 0; i < maxNumOfSteps; i++)
					{
						std::optional<Eigen::Vector4f> v1_v2_v3_clamp = Clamp<x, y, z>(v1_v2_v3 * scale);

						if (v1_v2_v3_clamp.has_value())
						{
							Eigen::Vector4i v1_v2_v3_trunc                                                       = Round(v1_v2_v3_clamp.value());
							redMatrix[v1_v2_v3_trunc[0] + y * v1_v2_v3_trunc[1] + (z * y) * v1_v2_v3_trunc[2]]   = c1_c2_c3[0] * c1_c2_c3[3];
							greenMatrix[v1_v2_v3_trunc[0] + y * v1_v2_v3_trunc[1] + (z * y) * v1_v2_v3_trunc[2]] = c1_c2_c3[1] * c1_c2_c3[3];
							blueMatrix[v1_v2_v3_trunc[0] + y * v1_v2_v3_trunc[1] + (z * y) * v1_v2_v3_trunc[2]]  = c1_c2_c3[2] * c1_c2_c3[3];
						}

						v1_v2_v3 += (v3 - v1_v2) * (1.0f / ((float)(maxNumOfSteps)));
						c1_c2_c3 += (c3 - c1_c2) * (1.0f / ((float)(maxNumOfSteps)));
					}

					v1_v2 += (v2 - v1) * (1.0f / ((float)(maxNumOfSteps)));
					c1_c2 += (c2 - c1) * (1.0f / ((float)(maxNumOfSteps)));
				}
			}
		}

		// Edge Mode
		if ((drawOptions & DrawOptions::DrawEdges) == DrawOptions::DrawEdges)
		{
			if ((meshState & (Status::TrianglesAllocated | Status::ColorsAllocated | Status::VertsAllocated)) != (Status::TrianglesAllocated | Status::ColorsAllocated | Status::VertsAllocated))
			{
#ifndef STM32_PROCESSOR
				throw MeshInitializationException(meshState, (Status::TrianglesAllocated | Status::ColorsAllocated | Status::VertsAllocated));
#endif

				return false;
			}
			// iterate over all triangles
			for (int triangleNum = 0; triangleNum < numOfTriangles; triangleNum++)
			{
				auto& t1 = triangles(0, triangleNum);
				auto& t2 = triangles(1, triangleNum);
				auto& t3 = triangles(2, triangleNum);

				Eigen::Vector4f v1 = verts_tranformed(Eigen::all, t1);
				Eigen::Vector4f v2 = verts_tranformed(Eigen::all, t2);
				Eigen::Vector4f v3 = verts_tranformed(Eigen::all, t3);

				Eigen::Vector4f c1 = colors(Eigen::all, t1);
				Eigen::Vector4f c2 = colors(Eigen::all, t2);
				Eigen::Vector4f c3 = colors(Eigen::all, t3);

				// To calculate edge first we need to determine the step size
				// Each time take a step and determine closest LED by rounding down
				// Take n number of steps where n is number of verticies

				// int maxNumOfSteps = (int)(std::max(std::max(x, y), z) * sqrt(3));

				DrawEdge<x, y, z>(blueMatrix, greenMatrix, redMatrix, scale * v1, scale * v2, c1, c2);
				DrawEdge<x, y, z>(blueMatrix, greenMatrix, redMatrix, scale * v1, scale * v3, c1, c3);
				DrawEdge<x, y, z>(blueMatrix, greenMatrix, redMatrix, scale * v2, scale * v3, c2, c3);

				// // Edge 1 v1 -> v2
				// Eigen::Vector4f v1_v2 = v1;
				// Eigen::Vector4f c1_c2 = c1;
				// for (int i = 0; i <= maxNumOfSteps; i++)
				// {
				// 	std::optional<Eigen::Vector4f> v1_v2_clamp = Clamp<x, y, z>(v1_v2 * scale);

				// 	if (v1_v2_clamp.has_value())
				// 	{
				// 		Eigen::Vector4i v1_v2_trunc                                                 = Round(v1_v2_clamp.value());
				// 		redMatrix[v1_v2_trunc[0] + y * v1_v2_trunc[1] + (z * y) * v1_v2_trunc[2]]   = c1_c2[0] * c1_c2[3];
				// 		greenMatrix[v1_v2_trunc[0] + y * v1_v2_trunc[1] + (z * y) * v1_v2_trunc[2]] = c1_c2[1] * c1_c2[3];
				// 		blueMatrix[v1_v2_trunc[0] + y * v1_v2_trunc[1] + (z * y) * v1_v2_trunc[2]]  = c1_c2[2] * c1_c2[3];
				// 	}

				// 	float t = (float)i / ((float)(maxNumOfSteps));
				// 	v1_v2   = v2 * t + v1 * (1.0f - t);
				// 	c1_c2   = c2 * t + c1 * (1.0f - t);
				// }

				// // Edge 2 v1 -> v3
				// Eigen::Vector4f v1_v3 = v1;
				// Eigen::Vector4f c1_c3 = c1;
				// for (int i = 0; i <= maxNumOfSteps; i++)
				// {
				// 	std::optional<Eigen::Vector4f> v1_v3_clamp = Clamp<x, y, z>(v1_v3 * scale);

				// 	if (v1_v3_clamp.has_value())
				// 	{
				// 		Eigen::Vector4i v1_v3_trunc                                                 = Round(v1_v3_clamp.value());
				// 		redMatrix[v1_v3_trunc[0] + y * v1_v3_trunc[1] + (z * y) * v1_v3_trunc[2]]   = c1_c3[0] * c1_c3[3];
				// 		greenMatrix[v1_v3_trunc[0] + y * v1_v3_trunc[1] + (z * y) * v1_v3_trunc[2]] = c1_c3[1] * c1_c3[3];
				// 		blueMatrix[v1_v3_trunc[0] + y * v1_v3_trunc[1] + (z * y) * v1_v3_trunc[2]]  = c1_c3[2] * c1_c3[3];
				// 	}

				// 	float t = (float)i / ((float)(maxNumOfSteps));
				// 	v1_v3   = v3 * t + v1 * (1.0f - t);
				// 	c1_c3   = c3 * t + c1 * (1.0f - t);
				// }

				// // Edge 3 v2 -> v3
				// Eigen::Vector4f v2_v3 = v2;
				// Eigen::Vector4f c2_c3 = c2;
				// for (int i = 0; i <= maxNumOfSteps; i++)
				// {
				// 	std::optional<Eigen::Vector4f> v2_v3_clamp = Clamp<x, y, z>(v2_v3 * scale);

				// 	if (v2_v3_clamp.has_value())
				// 	{
				// 		Eigen::Vector4i v2_v3_trunc                                                 = Round(v2_v3_clamp.value());
				// 		redMatrix[v2_v3_trunc[0] + y * v2_v3_trunc[1] + (z * y) * v2_v3_trunc[2]]   = c2_c3[0] * c2_c3[3];
				// 		greenMatrix[v2_v3_trunc[0] + y * v2_v3_trunc[1] + (z * y) * v2_v3_trunc[2]] = c2_c3[1] * c2_c3[3];
				// 		blueMatrix[v2_v3_trunc[0] + y * v2_v3_trunc[1] + (z * y) * v2_v3_trunc[2]]  = c2_c3[2] * c2_c3[3];
				// 	}

				// 	float t = (float)i / ((float)(maxNumOfSteps));
				// 	v2_v3   = v3 * t + v2 * (1.0f - t);
				// 	c2_c3   = c3 * t + c2 * (1.0f - t);
				// }
			}
		}

		// Vertex Mode
		if ((drawOptions & DrawOptions::DrawVerticies) == DrawOptions::DrawVerticies)
		{
			if ((meshState & (Status::ColorsAllocated | Status::VertsAllocated)) != (Status::ColorsAllocated | Status::VertsAllocated))
			{
#ifndef STM32_PROCESSOR
				throw MeshInitializationException(meshState, (Status::ColorsAllocated | Status::VertsAllocated));
#endif

				return false;
			}
			if (colors.cols() < verts_tranformed.cols())
			{
#ifndef STM32_PROCESSOR
				throw MeshRuntimeError(colors.cols(), verts_tranformed.cols());
#endif

				return false;
			}

			// iterate over all verticies and draws them
			for (int vertNum = 0; vertNum < verts_tranformed.cols(); vertNum++)
			{
				Eigen::Vector4f v = (verts_tranformed * scale)(Eigen::all, vertNum);
				Eigen::Vector4f c = colors(Eigen::all, vertNum);

				size_t px = (size_t)std::round(std::clamp(v[0], 0.0f, (float)x - 1));
				size_t py = (size_t)std::round(std::clamp(v[1], 0.0f, (float)y - 1));
				size_t pz = (size_t)std::round(std::clamp(v[2], 0.0f, (float)z - 1));

				// std::optional<Eigen::Vector4f> v = Clamp<x, y, z>();

				size_t index = px + py * x + pz * x * y;

				if (index < LEDNum)
				{
					redMatrix[index]   = c[0] * c[3];
					greenMatrix[index] = c[1] * c[3];
					blueMatrix[index]  = c[2] * c[3];
				}

				// // vertex 1
				// if (v.has_value())
				// {
				// 	Eigen::Vector4i v_trunc = Round(v.value());

				// 	redMatrix[v_trunc[0] + y * v_trunc[1] + (z * y) * v_trunc[2]]   = colors(0, vertNum) * colors(3, vertNum);
				// 	greenMatrix[v_trunc[0] + y * v_trunc[1] + (z * y) * v_trunc[2]] = colors(1, vertNum) * colors(3, vertNum);
				// 	blueMatrix[v_trunc[0] + y * v_trunc[1] + (z * y) * v_trunc[2]]  = colors(2, vertNum) * colors(3, vertNum);
				// }
			}
		}

		return true;
	}
};

} // namespace LumiVoxel

#endif // _TRIANGLE_MESH_HPP