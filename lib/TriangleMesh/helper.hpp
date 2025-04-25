#include <stdint.h>
#include <exception>
#include <string>

/// @brief Struct to store information regarding the triangle mesh Status
enum struct Status : uint8_t
{
    None = 0,
    VertsAllocated = 1 << 0,
    TrianglesAllocated = 1 << 1,
    ColorsAllocated = 1 << 2,
};

constexpr Status operator &(Status l, Status r)
{
    return static_cast<Status>(static_cast<uint8_t>(l) & static_cast<uint8_t>(r));
}


constexpr Status operator |(Status l, Status r)
{
    return static_cast<Status>(static_cast<uint8_t>(l) | static_cast<uint8_t>(r));
}

Status& operator |=(Status& l, Status r)
{
    return l = l | r;
}

Status& operator &=(Status& l, Status r)
{
    return l = l & r;
}

constexpr Status operator ~(Status v)
{
    return static_cast<Status>(~static_cast<uint8_t>(v));
}

/// @brief Struct to store information regarding the triangle mesh options for drawing
enum struct DrawOptions : uint8_t
{
    None = 0,
    DrawVerticies = 1 << 0,
    DrawEdges = 1 << 1,
    DrawFill = 1 << 2,
    ProjectToUnitCube = 1 << 3,
    Round_trunc = 1 << 4,
    Clamp = 1 << 5,

};

constexpr DrawOptions operator &(DrawOptions l, DrawOptions r)
{
    return static_cast<DrawOptions>(static_cast<uint8_t>(l) & static_cast<uint8_t>(r));
}


constexpr DrawOptions operator |(DrawOptions l, DrawOptions r)
{
    return static_cast<DrawOptions>(static_cast<uint8_t>(l) | static_cast<uint8_t>(r));
}

DrawOptions& operator |=(DrawOptions& l, DrawOptions r)
{
    return l = l | r;
}

DrawOptions& operator &=(DrawOptions& l, DrawOptions r)
{
    return l = l & r;
}

constexpr DrawOptions operator ~(DrawOptions v)
{
    return static_cast<DrawOptions>(~static_cast<uint8_t>(v));
}

#ifndef STM32_PROCESSOR
/// @brief Exception to handle misconfiguration of the mesh class. For instance, attempting to rasterize before setting the cube space to rasterize to.
class MeshInitializationException : public std::exception
{
    public:
        std::string exceptionString = "";

        MeshInitializationException(Status s, Status compS)
        {
            Status missingStatus = (~s) & compS;

            if((missingStatus & Status::ColorsAllocated) == Status::ColorsAllocated)
            {
                exceptionString += "Error: Triangle Mesh colors have not been properly initialized with a call to AllocateColors()\n";
            }

            if((missingStatus & Status::VertsAllocated) == Status::VertsAllocated)
            {
                exceptionString += "Error: Triangle Mesh verticies have not been properly initialized with a call to AllocateVerts()\n";
            }

            if((missingStatus & Status::TrianglesAllocated) == Status::TrianglesAllocated)
            {
                exceptionString += "Error: Triangle Mesh triangles have not been properly initialized with a call to AllocateTriangles()\n";
            }
        }

        const char * what() const noexcept override
        {
            return exceptionString.c_str();
        }
};

/// @brief Exception to handle runtime related issues like colors and verticies being of different sizes when drawing verticies
class MeshRuntimeError : public std::exception
{
    public:
        std::string exceptionString = "";

        MeshRuntimeError(size_t colorSize, size_t vertSize)
        {
            exceptionString += "Color and Vertex size mismatch Color: " + std::to_string(colorSize) + " Vert: " + std::to_string(vertSize);
        }

        const char * what() const noexcept override
        {
            return exceptionString.c_str();
        }
};

#endif
