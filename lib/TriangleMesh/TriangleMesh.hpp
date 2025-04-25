#include "helper.hpp"

#include <Eigen/Core>
#include <vector>
#include <chrono>
#include <iostream>
#include <unistd.h>
#include <optional>
#include <algorithm>
#include <functional>

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
class TriangleMesh
{
    private:
        /// @brief Position of each verticies loaded after call to AllocateVerts(), stored as x,y,z
        Eigen::Matrix<float,4,Eigen::Dynamic,0,4,maxVertNum> verts;

        /// @brief Current position of the verticies
        Eigen::Matrix<float,4,Eigen::Dynamic,0,4,maxVertNum> verts_tranformed;

        /// @brief Index position of triangles
        Eigen::Matrix<int,3,Eigen::Dynamic,0,3,maxVertNum> triangles;

        /// @brief Color of each vertex, each column matches the vertex location in the same column of verts
        Eigen::Matrix<float,4,Eigen::Dynamic,0,4,maxVertNum> colors;

        int numOfTriangles = 0; //number of triangles represented in the mesh

        /// @brief Base color to reset LEDs to. Default is zero
        Eigen::Vector4f fillColor = Eigen::Vector4f(0.0f,0.0f,0.0f,1.0f);

        /// @brief Tracks the current state of the mesh. Alter state through calls to allocate functions and setProjectionSpace()
        Status meshState = Status::None;

        /// @brief Drawing options for rasterization and different drawing functions. The draw vertex, edge, and fill modes can be set together and will be performed together. However, setting any of these with the test options will cause them to be overridden. Calling multiple tests together will result in undefined behavior
        DrawOptions drawOptions = DrawOptions::None;

        /// @brief Number of verticies contained within the triangle mesh
        int meshVertexNum = 0;

        //for test patten of in sequence drawing
        int sequenceDrawn = 0;

        //Stuff for game of life
        int numToReviveLowerBound = 0; //Lower bound for revival
        int numToReviveUpperBound = 0; //upper bound for revival
        int numToKill = 0; // Kills lower than this value

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
        template <size_t x, size_t y, size_t z>
        std::optional<Eigen::Vector4f> Clamp(const Eigen::Vector4f& v)
        {
            if(((v[0] >= 0) & (v[0] < x)) & ((v[1] >= 0) & (v[1] < y)) & ((v[2] >= 0) & (v[2] < z)))
            {
				return v;
            }
            else
            {
                if((drawOptions & DrawOptions::Clamp) == DrawOptions::Clamp)
                {
					Eigen::Vector4f r;

					r[0] = std::clamp((float)r[0],0.0f,(float)x - 1);
                    r[1] = std::clamp((float)r[1],0.0f,(float)y - 1);
                    r[2] = std::clamp((float)r[2],0.0f,(float)z) - 1;
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
            if((drawOptions & DrawOptions::Round_trunc) == DrawOptions::Round_trunc)
            {
                return Eigen::Vector4f(v).cast<int>();
            }
            else
            {

                Eigen::Vector4f ret = Eigen::Vector4f(v);

                ret[0] = std::round(ret[0]);
                ret[1] = std::round(ret[1]);
                ret[2] = std::round(ret[2]);

                return ret.cast<int>();
            }
        }

    public:
        constexpr TriangleMesh()
        { }

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
        void SetColor(std::array<float,LEDNum>& blueMatrix, std::array<float,LEDNum>& greenMatrix, std::array<float,LEDNum>& redMatrix, Eigen::Vector4f color)
        {
            for(int index = 0; index < LEDNum; index++)
            {
                redMatrix[index] = color[0] * color[3];
                greenMatrix[index] = color[1] * color[3];
                blueMatrix[index] = color[2] * color[3];
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
            if((d & DrawOptions::ProjectToUnitCube) == DrawOptions::ProjectToUnitCube)
            {
                std::array<int,3> ind {0,1,2};
                for(int i : ind)
                {
                    verts_tranformed(i,Eigen::all) = verts_tranformed(i,Eigen::all) / verts_tranformed.maxCoeff();
                }
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
            if((option & DrawOptions::ProjectToUnitCube) == DrawOptions::ProjectToUnitCube)
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
        template <size_t vertLength>
        bool AllocateVerts(const std::array<float,vertLength>& verts_sequential)
        {
            if((vertLength % 3) != 0){
                return false;
            }

            verts.resize(4,(int)(vertLength / 3));
            meshVertexNum = (int)(vertLength / 3);

            for(int i = 0 ; i < vertLength; i++)
            {
                (verts)(i%3,(int)(i/3)) = verts_sequential.at(i);
                (verts)(3,(int)(i/3)) = 1;
            }

            if ((drawOptions & DrawOptions::ProjectToUnitCube) == DrawOptions::ProjectToUnitCube)
            {
                verts /= verts.maxCoeff();
            }

            verts_tranformed = verts;


            if(verts_tranformed.data() != nullptr)
            {
                meshState |= Status::VertsAllocated;
            }

            return (verts_tranformed.data() != nullptr);
        }

        /**
         * @brief Allocates all of the colors into a Eigen Matrix
         *
         * @param colors_sequential vector of colors sequentially stored with three floats
         *
         * @return bool indicating if allocation was successful
         */
        template <size_t colorLength>
        bool AllocateColors(const std::array<float, colorLength>& colors_sequential)
        {
            if((colorLength % 4) != 0){
                return false;
            }

            colors.resize(4,(int)(colorLength / 4));

            for(int i = 0 ; i < colorLength; i++)
            {
                (colors)(i%4,(int)(i/4)) = colors_sequential.at(i);
            }

            if(colors.data() != nullptr)
            {
                meshState |= Status::ColorsAllocated;
            }

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
        template <size_t trianglesLength>
        bool AllocateTriangles(const std::array<int,trianglesLength>& triangles_sequential)
        {
            if((trianglesLength % 3) != 0){
                return false;
            }

            numOfTriangles = (int)(trianglesLength/3);
            triangles.resize(3,numOfTriangles);

            for(int i = 0 ; i < trianglesLength; i++)
            {
                (triangles)(i%3,(int)(i/3)) = triangles_sequential.at(i);
            }

            if(triangles.data() != nullptr)
            {
                meshState |= Status::TrianglesAllocated;
            }

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
        }

        /**
         * @brief Advances timestep for the Game of life
         *
         * @param updateStep number of advances to perform
         */
        void CellularAutomataAdvanceTimestep(int updateStep)
        {

        }

        /**
         * @brief Runs the specific testing algorithm
         *
         * @param blueMatrix XxYxZ float matrix to represent the blue component of the LED matrix.
         * @param greenMatrix XxYxZ float matrix to represent the green component of the LED matrix.
         * @param redMatrix XxYxZ float matrix to represent the red component of the LED matrix.
         */
        template <size_t LEDNum>
        void Test(std::array<float,LEDNum>& blueMatrix, std::array<float,LEDNum>& greenMatrix, std::array<float,LEDNum>& redMatrix)
        {

            // //generate "rand white lights"
            // if(drawOptions && DrawOptions::DrawTestFill)
            // {
            //     for(int index = 0; index < LEDNum; index++)
            //     {
            //         #ifndef STM32_PROCESSOR
            //         auto startTime = std::chrono::high_resolution_clock::now();

            //         auto currentTime = std::chrono::high_resolution_clock::now();
            //         float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            //         #else
            //         float time = 0.00000001;
            //         #endif

            //         blueMatrix[index] > 1.0f ? blueMatrix[index] = 0.0f : blueMatrix[index] += time * 10000.0f;
            //         greenMatrix[index] > 1.0f ? greenMatrix[index] = 0.0f : greenMatrix[index] += time * 10000.0f;
            //         redMatrix[index] > 1.0f ? redMatrix[index] = 0.0f : redMatrix[index] += time * 10000.0f;
            //     }

            //     return true;
            // }

            // //Flushes the frame before every draw.
            // SetColor(blueMatrix,greenMatrix,redMatrix,fillColor);

            // // ^Needs to be called before the sequential test to ensure a clean drawing. Inefficient, but should only be used for testing purposes
            // //Lights leds sequentially in order
            // if(drawOptions && DrawOptions::DrawTestSequential)
            // {

            //     redMatrix[sequenceDrawn] = 1.0f;
            //     greenMatrix[sequenceDrawn] = 1.0f;
            //     blueMatrix[sequenceDrawn] = 1.0f;

            //     sequenceDrawn++;
            //     if(sequenceDrawn >= LEDNum)
            //     {
            //         sequenceDrawn = 0;
            //     }
            //     return true;
            // }
        }

        /**
         * @brief Draws triangles on the LED matrix. The dimension of the LED matrix is templated, and the total number of LEDs in the matrix is
         * given by multiplying all template parameters together. The output colors channels should match this dimension.
         *
         * For Triangle fill, bilinear interpolation is used to determine the color
         * For Edge drawing a simple line interpolation method is used to determine the color
         * For Vertex drawing, there is no interpolation
         *
         * @param blueMatrix XxYxZ float matrix to represent the blue component of the LED matrix.
         * @param greenMatrix XxYxZ float matrix to represent the green component of the LED matrix.
         * @param redMatrix XxYxZ float matrix to represent the red component of the LED matrix.
         *
         * @return true if sucessfully wrote to color arrays
         */
        template <size_t x, size_t y, size_t z>
        bool Rasterize(std::array<float,x * y * z>& blueMatrix, std::array<float,x * y * z>& greenMatrix, std::array<float,x * y * z>& redMatrix )
        {
            int scale = (std::max(x,std::max(y,z)) - 1);

            const int LEDNum = x * y * z;

            SetColor<LEDNum>(blueMatrix,greenMatrix,redMatrix,fillColor);

            //Filled Mode -> Does not guarentee to lie on the same plane as three points due to rasterization
            if((drawOptions & DrawOptions::DrawFill) == DrawOptions::DrawFill)
            {
                if((meshState & (Status::TrianglesAllocated | Status::ColorsAllocated | Status::VertsAllocated)) != (Status::TrianglesAllocated | Status::ColorsAllocated | Status::VertsAllocated))
                {
                    #ifndef STM32_PROCESSOR
                    throw MeshInitializationException(meshState,(Status::TrianglesAllocated | Status::ColorsAllocated | Status::VertsAllocated));
                    #endif

                    return false;
                }
                //iterate over all triangles
                for(int triangleNum = 0; triangleNum < numOfTriangles; triangleNum++)
                {
                    Eigen::Vector4f v1 = verts_tranformed(Eigen::all,triangles(0,triangleNum));
                    Eigen::Vector4f v2 = verts_tranformed(Eigen::all,triangles(1,triangleNum));
                    Eigen::Vector4f v3 = verts_tranformed(Eigen::all,triangles(2,triangleNum));

                    Eigen::Vector4f c1 = colors(Eigen::all,triangles(0,triangleNum));
                    Eigen::Vector4f c2 = colors(Eigen::all,triangles(1,triangleNum));
                    Eigen::Vector4f c3 = colors(Eigen::all,triangles(2,triangleNum));

                    int maxNumOfSteps = (int)(std::max(std::max(x,y),z) * sqrt(3));

                    Eigen::Vector4f v1_v2 = v1;
                    Eigen::Vector4f c1_c2 = c1;

                    for(int i = 0; i < maxNumOfSteps; i++)
                    {
                        Eigen::Vector4f v1_v2_v3 = v1_v2;
                        Eigen::Vector4f c1_c2_c3 = c1_c2;

                        for(int i = 0; i < maxNumOfSteps; i++)
                        {
                            std::optional<Eigen::Vector4f> v1_v2_v3_clamp = Clamp<x,y,z>(v1_v2_v3 * scale);

                            if(v1_v2_v3_clamp.has_value())
                            {
                                Eigen::Vector4i v1_v2_v3_trunc = Round(v1_v2_v3_clamp.value());
                                redMatrix[v1_v2_v3_trunc[0] + y * v1_v2_v3_trunc[1] + (z * y) * v1_v2_v3_trunc[2]] = c1_c2_c3[0] * c1_c2_c3[3];
                                greenMatrix[v1_v2_v3_trunc[0] + y * v1_v2_v3_trunc[1] + (z * y) * v1_v2_v3_trunc[2]] = c1_c2_c3[1] * c1_c2_c3[3];
                                blueMatrix[v1_v2_v3_trunc[0] + y * v1_v2_v3_trunc[1] + (z * y) * v1_v2_v3_trunc[2]] = c1_c2_c3[2] * c1_c2_c3[3];
                            }

                            v1_v2_v3 += (v3 - v1_v2) * (1.0f/((float)(maxNumOfSteps)));
                            c1_c2_c3 += (c3 - c1_c2) * (1.0f/((float)(maxNumOfSteps)));
                        }

                        v1_v2 += (v2 - v1) * (1.0f/((float)(maxNumOfSteps)));
                        c1_c2 += (c2 - c1) * (1.0f/((float)(maxNumOfSteps)));
                    }
                }
            }

            //Edge Mode
            if((drawOptions & DrawOptions::DrawEdges) == DrawOptions::DrawEdges)
            {
                if((meshState & (Status::TrianglesAllocated | Status::ColorsAllocated | Status::VertsAllocated)) != (Status::TrianglesAllocated | Status::ColorsAllocated | Status::VertsAllocated))
                {
                    #ifndef STM32_PROCESSOR
                    throw MeshInitializationException(meshState,(Status::TrianglesAllocated | Status::ColorsAllocated | Status::VertsAllocated));
                    #endif

                    return false;
                }
                //iterate over all triangles
                for(int triangleNum = 0; triangleNum < numOfTriangles; triangleNum++)
                {
                    auto& t1 = triangles(0,triangleNum);
                    auto& t2 = triangles(1,triangleNum);
                    auto& t3 = triangles(2,triangleNum);

                    Eigen::Vector4f v1 = verts_tranformed(Eigen::all,t1);
                    Eigen::Vector4f v2 = verts_tranformed(Eigen::all,t2);
                    Eigen::Vector4f v3 = verts_tranformed(Eigen::all,t3);

                    Eigen::Vector4f c1 = colors(Eigen::all,t1);
                    Eigen::Vector4f c2 = colors(Eigen::all,t2);
                    Eigen::Vector4f c3 = colors(Eigen::all,t3);


                    // To calculate edge first we need to determine the step size
                    // Each time take a step and determine closest LED by rounding down
                    // Take n number of steps where n is number of verticies

                    int maxNumOfSteps = (int)(std::max(std::max(x,y),z) * sqrt(3));

                    //Edge 1 v1 -> v2
                    Eigen::Vector4f v1_v2 = v1;
                    Eigen::Vector4f c1_c2 = c1;
                    for(int i = 0; i <= maxNumOfSteps; i++)
                    {
                        std::optional<Eigen::Vector4f> v1_v2_clamp = Clamp<x,y,z>(v1_v2 * scale);

                        if(v1_v2_clamp.has_value())
                        {
                            Eigen::Vector4i v1_v2_trunc = Round(v1_v2_clamp.value());
                            redMatrix[v1_v2_trunc[0] + y * v1_v2_trunc[1] + (z * y) * v1_v2_trunc[2]] = c1_c2[0] * c1_c2[3];
                            greenMatrix[v1_v2_trunc[0] + y * v1_v2_trunc[1] + (z * y) * v1_v2_trunc[2]] = c1_c2[1] * c1_c2[3];
                            blueMatrix[v1_v2_trunc[0] + y * v1_v2_trunc[1] + (z * y) * v1_v2_trunc[2]] = c1_c2[2] * c1_c2[3];
                        }

                        v1_v2 += (v2 - v1) * (1.0f/((float)(maxNumOfSteps)));
                        c1_c2 += (c2 - c1) * (1.0f/((float)(maxNumOfSteps)));

                    }


                    //Edge 2 v1 -> v3
                    Eigen::Vector4f v1_v3 = v1;
                    Eigen::Vector4f c1_c3 = c1;
                    for(int i = 0; i <= maxNumOfSteps; i++)
                    {
                        std::optional<Eigen::Vector4f> v1_v3_clamp = Clamp<x,y,z>(v1_v3 * scale);

                        if(v1_v3_clamp.has_value())
                        {
                            Eigen::Vector4i v1_v3_trunc = Round(v1_v3_clamp.value());
                            redMatrix[v1_v3_trunc[0] + y * v1_v3_trunc[1] + (z * y) * v1_v3_trunc[2]] = c1_c3[0] * c1_c3[3];
                            greenMatrix[v1_v3_trunc[0] + y * v1_v3_trunc[1] + (z * y) * v1_v3_trunc[2]] = c1_c3[1] * c1_c3[3];
                            blueMatrix[v1_v3_trunc[0] + y * v1_v3_trunc[1] + (z * y) * v1_v3_trunc[2]] = c1_c3[2] * c1_c3[3];
                        }


                        v1_v3 += (v3 - v1) * (1.0f/((float)(maxNumOfSteps)));
                        c1_c3 += (c3 - c1) * (1.0f/((float)(maxNumOfSteps)));
                     }


                    //Edge 3 v2 -> v3
                    Eigen::Vector4f v2_v3 = v2;
                    Eigen::Vector4f c2_c3 = c2;
                    for(int i = 0; i <= maxNumOfSteps; i++)
                    {
                        std::optional<Eigen::Vector4f> v2_v3_clamp = Clamp<x,y,z>(v2_v3 * scale);

                        if(v2_v3_clamp.has_value())
                        {
                            Eigen::Vector4i v2_v3_trunc = Round(v2_v3_clamp.value());
                            redMatrix[v2_v3_trunc[0] + y * v2_v3_trunc[1] + (z * y) * v2_v3_trunc[2]] = c2_c3[0] * c2_c3[3];
                            greenMatrix[v2_v3_trunc[0] + y * v2_v3_trunc[1] + (z * y) * v2_v3_trunc[2]] = c2_c3[1] * c2_c3[3];
                            blueMatrix[v2_v3_trunc[0] + y * v2_v3_trunc[1] + (z * y) * v2_v3_trunc[2]] = c2_c3[2] * c2_c3[3];
                        }

                        v2_v3 += (v3 - v2) * (1.0f/((float)(maxNumOfSteps)));
                        c2_c3 += (c3 - c2) * (1.0f/((float)(maxNumOfSteps)));
                     }
                }
            }

            //Vertex Mode
            if((drawOptions & DrawOptions::DrawVerticies) == DrawOptions::DrawVerticies)
            {
                if((meshState & (Status::ColorsAllocated | Status::VertsAllocated)) != (Status::ColorsAllocated | Status::VertsAllocated))
                {
                    #ifndef STM32_PROCESSOR
                    throw MeshInitializationException(meshState,(Status::ColorsAllocated | Status::VertsAllocated));
                    #endif

                    return false;
                }
                if(colors.cols() != verts_tranformed.cols())
                {
                    #ifndef STM32_PROCESSOR
                    throw MeshRuntimeError(colors.cols(),verts_tranformed.cols());
                    #endif

                    return false;
                }

                //iterate over all verticies and draws them
                for(int vertNum = 0; vertNum < verts_tranformed.cols(); vertNum++)
                {
                    std::optional<Eigen::Vector4f> v = Clamp<x,y,z>((verts_tranformed * scale)(Eigen::all,vertNum));

					//vertex 1
                    if(v.has_value())
                    {
                        Eigen::Vector4i v_trunc = Round(v.value());

                        redMatrix[v_trunc[0] + y * v_trunc[1] + (z * y) * v_trunc[2]] = colors(0,vertNum) * colors(3,vertNum);
                        greenMatrix[v_trunc[0] + y * v_trunc[1] + (z * y) * v_trunc[2]] = colors(1,vertNum) * colors(3,vertNum);
                        blueMatrix[v_trunc[0] + y * v_trunc[1] + (z * y) * v_trunc[2]] = colors(2,vertNum) * colors(3,vertNum);
                    }
                }
            }

            return true;
        }
};
