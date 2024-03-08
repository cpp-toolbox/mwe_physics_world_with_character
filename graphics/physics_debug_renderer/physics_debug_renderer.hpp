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
    int shader_program;
    unsigned int vbo, ibo, vao;

    ShaderPipeline shader_pipeline;

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
};

class ThatIHaveToMake : public JPH::RefTarget<ThatIHaveToMake> {

};

class TriangleData : public JPH::RefTargetVirtual ,public ThatIHaveToMake {
public:
    const JPH::DebugRenderer::Triangle *triangles;
    int num_triangles;

    const JPH::DebugRenderer::Vertex *vertices;
    int num_vertices;
    const JPH::uint32 *indices;
    int num_indices;
    bool uses_indices;

    TriangleData(const JPH::DebugRenderer::Triangle *triangles, int num_triangles) {
        this->num_triangles = num_triangles;
        this->triangles = triangles;
        this->uses_indices = false;
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
