/**
 * @file    tree_mesh_builder.cpp
 *
 * @author  Natália Holková <xholko02@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    17.12.2021
 **/

#include <iostream>
#include <math.h>
#include <limits>

#include "tree_mesh_builder.h"

TreeMeshBuilder::TreeMeshBuilder(unsigned gridEdgeSize)
    : BaseMeshBuilder(gridEdgeSize, "Octree")
{

}

unsigned TreeMeshBuilder::marchCubes(const ParametricScalarField &field)
{
    // Suggested approach to tackle this problem is to add new method to
    // this class. This method will call itself to process the children.
    // It is also strongly suggested to first implement Octree as sequential
    // code and only when that works add OpenMP tasks to achieve parallelism.

    Vec3_t<float> cubeOffset(0, 0, 0);
    unsigned totalTriangles = 0;

    #pragma omp parallel
    #pragma omp single nowait
    totalTriangles = recEvalCube(mGridSize, field, cubeOffset);

    return totalTriangles;
}

float TreeMeshBuilder::evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field)
{
    const Vec3_t<float> *pPoints = field.getPoints().data();
    const unsigned count = unsigned(field.getPoints().size());

    float value = std::numeric_limits<float>::max();

    for(unsigned i = 0; i < count; ++i) {
        float distanceSquared  = (pos.x - pPoints[i].x) * (pos.x - pPoints[i].x);
        distanceSquared       += (pos.y - pPoints[i].y) * (pos.y - pPoints[i].y);
        distanceSquared       += (pos.z - pPoints[i].z) * (pos.z - pPoints[i].z);

        value = std::min(value, distanceSquared);
    }

    return sqrt(value);
}

void TreeMeshBuilder::emitTriangle(const BaseMeshBuilder::Triangle_t &triangle)
{
    #pragma omp critical
    mTriangles.push_back(triangle);
}

// New method that will call itself to process the children
unsigned TreeMeshBuilder::recEvalCube(size_t gridSize, const ParametricScalarField &field, const Vec3_t<float> &cubeOffset) {
    unsigned totalTriangles = 0;

    // rekurzívna funkcia - musí byť ukončovacia podmienka
    // Skončí keď je hrana <= 1
    if (gridSize <= 1) {
        // cut-off
        return buildCube(cubeOffset, field);
    }

    // Novú veľkosť hrany
    size_t newGridSize = gridSize / 2;

    // overíme, či je možné aby prechádzal isosurface
    unsigned x_pos = cubeOffset.x + newGridSize;
    unsigned y_pos = cubeOffset.y + newGridSize;
    unsigned z_pos = cubeOffset.z + newGridSize;
    const Vec3_t<float> midCoords(x_pos * mGridResolution, y_pos * mGridResolution, z_pos * mGridResolution);

    float fp = evaluateFieldAt(midCoords, field);
    // Vyhodnotiť rovnicu: f(p) > l + sqrt(3)/2 * a -> ak platí blok je prázdny
    if (fp > mIsoLevel + ((sqrt(3.0)/2.0) * (gridSize * mGridResolution))) {
        // prázdny blok -> 0 triangles
        return 0;
    }

    // Teraz určite obsahuje niečo

    // Rozdeliť na 8 potomkov a rekurzia
    for (int x_index = 0; x_index < 2; x_index++) {
        for (int y_index = 0; y_index < 2; y_index++) {
            for(int z_index = 0; z_index < 2; z_index++) {

                #pragma omp task shared(totalTriangles)
                {
                    // vypočítať novú kocku
                    const Vec3_t<float> newCubeOffset(cubeOffset.x + x_index * newGridSize,
                                                cubeOffset.y + y_index * newGridSize,
                                                cubeOffset.z + z_index * newGridSize);

                    // zavolať rekurzívne túto funkciu
                    unsigned res = recEvalCube(newGridSize, field, newCubeOffset);
                    #pragma omp critical
                    totalTriangles += res;
                }
            }
        }
    }

    #pragma omp taskwait
    return totalTriangles;
}
