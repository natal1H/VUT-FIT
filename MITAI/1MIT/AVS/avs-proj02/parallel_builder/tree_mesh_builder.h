/**
 * @file    tree_mesh_builder.h
 *
 * @author  Natália Holková <xholko02@stud.fit.vutbr.cz>
 *
 * @brief   Parallel Marching Cubes implementation using OpenMP tasks + octree early elimination
 *
 * @date    17.12.2021
 **/

#ifndef TREE_MESH_BUILDER_H
#define TREE_MESH_BUILDER_H

#include <vector>
#include "base_mesh_builder.h"

class TreeMeshBuilder : public BaseMeshBuilder
{
public:
    TreeMeshBuilder(unsigned gridEdgeSize);

protected:
    unsigned marchCubes(const ParametricScalarField &field);
    float evaluateFieldAt(const Vec3_t<float> &pos, const ParametricScalarField &field);
    void emitTriangle(const Triangle_t &triangle);
    const Triangle_t *getTrianglesArray() const { return mTriangles.data(); }
    unsigned recEvalCube(size_t gridSize, const ParametricScalarField &field, const Vec3_t<float> &cubeOffset);
    std::vector<Triangle_t> mTriangles; ///< Temporary array of triangles
    unsigned evalTree(const Vec3_t<float> &position, const ParametricScalarField &field, size_t cubeSize);
};

#endif // TREE_MESH_BUILDER_H
