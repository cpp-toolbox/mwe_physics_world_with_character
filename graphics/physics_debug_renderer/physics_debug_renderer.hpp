#ifndef PHYSICS_DEBUG_RENDERER_HPP
#define PHYSICS_DEBUG_RENDERER_HPP

#include <Jolt/Jolt.h>

#include <Jolt/Renderer/DebugRenderer.h>
#include "Jolt/Geometry/Triangle.h"
#include "../shader_pipeline/shader_pipeline.hpp"

#include <glm/gtc/matrix_transform.hpp>

class PhysicsDebugRenderer final : public JPH::DebugRenderer {

public:
    PhysicsDebugRenderer(ShaderPipeline shader_pipeline);

    ~PhysicsDebugRenderer();

    /// Implementation of DebugRenderer interface
    virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo,
                          JPH::ColorArg inColor) override;

    virtual void
    DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3,
                 JPH::ColorArg inColor,
                 ECastShadow inCastShadow = ECastShadow::Off) override;

    virtual Batch CreateTriangleBatch(const Triangle *inTriangles,
                                      int inTriangleCount) override;

    virtual Batch CreateTriangleBatch(const Vertex *inVertices, int inVertexCount,
                                      const JPH::uint32 *inIndices,
                                      int inIndexCount) override;

    virtual void DrawGeometry(JPH::RMat44Arg inModelMatrix,
                              const JPH::AABox &inWorldSpaceBounds,
                              float inLODScaleSq, JPH::ColorArg inModelColor,
                              const GeometryRef &inGeometry, ECullMode inCullMode,
                              ECastShadow inCastShadow,
                              EDrawMode inDrawMode) override;

    virtual void DrawText3D(JPH::RVec3Arg inPosition,
                            const JPH::string_view &inString,
                            JPH::ColorArg inColor, float inHeight) override;

    // opengl related things
    unsigned int vbo, ibo, vao;

    ShaderPipeline shader_pipeline;
};

class ThatIHaveToMake : public JPH::RefTarget<ThatIHaveToMake> {

};

class TriangleData : public JPH::RefTargetVirtual ,public ThatIHaveToMake {
public:
    int num_triangles;
    std::vector<float> triangle_vertices;
    const JPH::DebugRenderer::Triangle *triangles;

    const JPH::DebugRenderer::Vertex *vertices;
    int num_vertices;
    const JPH::uint32 *indices;
    int num_indices;
    bool uses_indices;

    TriangleData(const JPH::DebugRenderer::Triangle *triangles, int num_triangles) {
        this->num_triangles = num_triangles;
        this->triangles = triangles;
        this->uses_indices = false;

        for (int i = 0; i < num_triangles; i += 1) {
            JPH::DebugRenderer::Triangle triangle = triangles[i];

            JPH::DebugRenderer::Vertex v1 = triangle.mV[0];
            JPH::DebugRenderer::Vertex v2 = triangle.mV[1];
            JPH::DebugRenderer::Vertex v3 = triangle.mV[2];

            triangle_vertices.push_back(v1.mPosition.x);
            triangle_vertices.push_back(v1.mPosition.y);
            triangle_vertices.push_back(v1.mPosition.z);

            triangle_vertices.push_back(v2.mPosition.x);
            triangle_vertices.push_back(v2.mPosition.y);
            triangle_vertices.push_back(v2.mPosition.z);

            triangle_vertices.push_back(v3.mPosition.x);
            triangle_vertices.push_back(v3.mPosition.y);
            triangle_vertices.push_back(v3.mPosition.z);
        }
    }

    TriangleData(const JPH::DebugRenderer::Vertex *vertices, int num_vertices, const JPH::uint32 *indices, int num_indices) {
        this->num_vertices = num_vertices;
        this->vertices = vertices;
        this->uses_indices = true;
        this->num_indices = num_indices;
        this->indices = indices;
    }

    virtual void AddRef() override { ThatIHaveToMake::AddRef(); }
    virtual void Release() override	{ if (--mRefCount == 0) delete this; }
};

#endif // PHYSICS_DEBUG_RENDERER_HPP
