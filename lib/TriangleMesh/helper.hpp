#pragma once

#include <stdint.h>
#include <string>



#ifndef STM32_PROCESSOR
#include <exception>
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