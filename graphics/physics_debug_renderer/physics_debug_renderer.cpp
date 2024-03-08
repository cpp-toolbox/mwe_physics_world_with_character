#include "physics_debug_renderer.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

PhysicsDebugRenderer::PhysicsDebugRenderer(ShaderPipeline shader_pipeline) {

    this->shader_pipeline = shader_pipeline;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    Initialize();

}

PhysicsDebugRenderer::~PhysicsDebugRenderer() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
//    glDeleteProgram(shader_program);
}

void PhysicsDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) {

//    glUseProgram(shader_program);
//
//    float vertices[6] = {
//            inFrom.GetX(), inFrom.GetY(), inFrom.GetZ(),
//            inFrom.GetX(), inFrom.GetY(), inFrom.GetZ(),
//    };
//
//    glBindVertexArray(vao);
//
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    JPH::Vec4 color = inColor.ToVec4();
//    float rgb_color[3] = {color.GetX(), color.GetY(), color.GetZ()};
////    glUniform3fv(glGetUniformLocation(shader_program, "color"), 1, &rgb_color[0]);
//    glUniform3fv(glGetUniformLocation(shader_program, "color"), 1, rgb_color);
//
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
//    glEnableVertexAttribArray(0);
//
//    glDrawArrays(GL_LINES, 0, 2);
}

JPH::DebugRenderer::Batch PhysicsDebugRenderer::CreateTriangleBatch(const Triangle *inTriangles, int inTriangleCount) {
    auto *triangle_data = new TriangleData( inTriangles, inTriangleCount);
    return triangle_data;
}

JPH::DebugRenderer::Batch PhysicsDebugRenderer::CreateTriangleBatch(const Vertex *inVertices, int inVertexCount, const JPH::uint32 *inIndices, int inIndexCount) {
    auto *triangle_data = new TriangleData(inVertices, inVertexCount, inIndices, inIndexCount);
    return triangle_data;
}

void PhysicsDebugRenderer::DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox &inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef &inGeometry, ECullMode inCullMode, ECastShadow inCastShadow, EDrawMode inDrawMode) {
    /*
     * the geometry contains a list of lods, each lod contains a triangleBatch
     * which can be cast to BatchImpl. These triangle batches were created with
     * the createTriangleBatch function, so those must be implemented before you implement
     * this or else it will not work, each BatchImpl which impelments RenderPrimitive
     */
    const JPH::Array<LOD> &geometry_lods = inGeometry->mLODs;
    // use lod 0 because our game doesn't use LOD at all
    TriangleData * triangle_batch = static_cast<TriangleData *>(geometry_lods[0].mTriangleBatch.GetPtr());

    void * ptr = reinterpret_cast<void *>(0x20);
    if (triangle_batch == ptr or triangle_batch == NULL) {
        return;
    }

    if (triangle_batch->uses_indices)  {
        printf("this triangle batch uses indices\n");
    } else {
        printf("this triangle batch does not use indices\n");

        glUseProgram(shader_pipeline.shader_program_id);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, triangle_batch->triangle_vertices.size() * sizeof(float), &triangle_batch->triangle_vertices.front(), GL_STATIC_DRAW);

        GLuint position_location = glGetAttribLocation(shader_pipeline.shader_program_id, "position");
        glEnableVertexAttribArray(position_location);
        glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void *) 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, triangle_batch->num_triangles * 3);
    }
}

//void PhysicsDebugRenderer::FinalizePrimitive()
//{
//    JPH_PROFILE_FUNCTION();
//
//    if (mLockedPrimitive != nullptr)
//    {
//        BatchImpl *primitive = static_cast<BatchImpl *>(mLockedPrimitive.GetPtr());
//
//        // Unlock the primitive
//        primitive->UnlockVertexBuffer();
//
//        // Set number of indices to draw
//        primitive->SetNumVtxToDraw(int(mLockedVertices - mLockedVerticesStart));
//
//        // Add to draw list
//        mTempPrimitives[new Geometry(mLockedPrimitive, mLockedPrimitiveBounds)].mInstances.push_back({ Mat44::sIdentity(), Mat44::sIdentity(), Color::sWhite, mLockedPrimitiveBounds, 1.0f });
//        ++mNumInstances;
//
//        // Clear pointers
//        mLockedPrimitive = nullptr;
//        mLockedVerticesStart = nullptr;
//        mLockedVertices = nullptr;
//        mLockedVerticesEnd = nullptr;
//        mLockedPrimitiveBounds = AABox();
//    }
//}
//
//void PhysicsDebugRenderer::EnsurePrimitiveSpace(int inVtxSize)
//{
//    const int cVertexBufferSize = 10240;
//
//    if (mLockedPrimitive == nullptr
//        || mLockedVerticesEnd - mLockedVertices < inVtxSize)
//    {
//        FinalizePrimitive();
//
//        // Create new
//        BatchImpl *primitive = new BatchImpl(mRenderer, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//        primitive->CreateVertexBuffer(cVertexBufferSize, sizeof(Vertex));
//        mLockedPrimitive = primitive;
//
//        // Lock buffers
//        mLockedVerticesStart = mLockedVertices = (Vertex *)primitive->LockVertexBuffer();
//        mLockedVerticesEnd = mLockedVertices + cVertexBufferSize;
//    }
//}

void PhysicsDebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) {
}

//void PhysicsDebugRenderer::DrawInstances(const Geometry *inGeometry, const Array<int> &inStartIdx)
//{
//    RenderInstances *instances_buffer = mInstancesBuffer[mRenderer->GetCurrentFrameIndex()];
//
//    if (!inStartIdx.empty())
//    {
//        // Get LODs
//        const Array<LOD> &geometry_lods = inGeometry->mLODs;
//
//        // Write instances for all LODS
//        int next_start_idx = inStartIdx.front();
//        for (size_t lod = 0; lod < geometry_lods.size(); ++lod)
//        {
//            int start_idx = next_start_idx;
//            next_start_idx = inStartIdx[lod + 1];
//            int num_instances = next_start_idx - start_idx;
//            instances_buffer->Draw(static_cast<BatchImpl *>(geometry_lods[lod].mTriangleBatch.GetPtr()), start_idx, num_instances);
//        }
//    }
//}

void PhysicsDebugRenderer::DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view &inString, JPH::ColorArg inColor, float inHeight) {
}

//void PhysicsDebugRenderer::DrawLines()
//{
//    JPH_PROFILE_FUNCTION();
//
//    lock_guard lock(mLinesLock);
//
//    // Draw the lines
//    if (!mLines.empty())
//    {
//        RenderPrimitive primitive(mRenderer, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
//        primitive.CreateVertexBuffer((int)mLines.size() * 2, sizeof(Line) / 2);
//        void *data = primitive.LockVertexBuffer();
//        memcpy(data, &mLines[0], mLines.size() * sizeof(Line));
//        primitive.UnlockVertexBuffer();
//        mLineState->Activate();
//        primitive.Draw();
//    }
//}
//
//void PhysicsDebugRenderer::DrawTriangles()
//{
//    JPH_PROFILE_FUNCTION();
//
//    lock_guard lock(mPrimitivesLock);
//
//    // Finish the last primitive
//    FinalizePrimitive();
//
//    // Render to shadow map texture first
//    mRenderer->SetRenderTarget(mDepthTexture);
//
//    // Clear the shadow map texture to max depth
//    mDepthTexture->ClearRenderTarget();
//
//    // Get the camera and light frustum for culling
//    Vec3 camera_pos(mRenderer->GetCameraState().mPos - mRenderer->GetBaseOffset());
//    const Frustum &camera_frustum = mRenderer->GetCameraFrustum();
//    const Frustum &light_frustum = mRenderer->GetLightFrustum();
//
//    // Resize instances buffer and copy all visible instance data into it
//    if (mNumInstances > 0)
//    {
//        // Create instances buffer
//        RenderInstances *instances_buffer = mInstancesBuffer[mRenderer->GetCurrentFrameIndex()];
//        instances_buffer->CreateBuffer(2 * mNumInstances, sizeof(Instance));
//        Instance *dst_instance = reinterpret_cast<Instance *>(instances_buffer->Lock());
//
//        // Next write index
//        int dst_index = 0;
//
//        // This keeps track of which instances use which lod, first array: 0 = light pass, 1 = geometry pass
//        Array<Array<int>> lod_indices[2];
//
//        for (InstanceMap *primitive_map : { &mPrimitives, &mTempPrimitives, &mPrimitivesBackFacing, &mWireframePrimitives })
//            for (InstanceMap::value_type &v : *primitive_map)
//            {
//                // Get LODs
//                const Array<LOD> &geometry_lods = v.first->mLODs;
//                size_t num_lods = geometry_lods.size();
//                JPH_ASSERT(num_lods > 0);
//
//                // Ensure that our lod index array is big enough (to avoid reallocating memory too often)
//                if (lod_indices[0].size() < num_lods)
//                    lod_indices[0].resize(num_lods);
//                if (lod_indices[1].size() < num_lods)
//                    lod_indices[1].resize(num_lods);
//
//                // Iterate over all instances
//                const Array<InstanceWithLODInfo> &instances = v.second.mInstances;
//                for (size_t i = 0; i < instances.size(); ++i)
//                {
//                    const InstanceWithLODInfo &src_instance = instances[i];
//
//                    // Check if it overlaps with the light or camera frustum
//                    bool light_overlaps = light_frustum.Overlaps(src_instance.mWorldSpaceBounds);
//                    bool camera_overlaps = camera_frustum.Overlaps(src_instance.mWorldSpaceBounds);
//                    if (light_overlaps || camera_overlaps)
//                    {
//                        // Figure out which LOD to use
//                        float dist_sq = src_instance.mWorldSpaceBounds.GetSqDistanceTo(camera_pos);
//                        for (size_t lod = 0; lod < num_lods; ++lod)
//                            if (dist_sq <= src_instance.mLODScaleSq * Square(geometry_lods[lod].mDistance))
//                            {
//                                // Store which index goes in which LOD
//                                if (light_overlaps)
//                                    lod_indices[0][lod].push_back((int)i);
//                                if (camera_overlaps)
//                                    lod_indices[1][lod].push_back((int)i);
//                                break;
//                            }
//                    }
//                }
//
//                // Loop over both passes: 0 = light, 1 = geometry
//                Array<int> *start_idx[] = { &v.second.mLightStartIdx, &v.second.mGeometryStartIdx };
//                for (int type = 0; type < 2; ++type)
//                {
//                    // Reserve space for instance indices
//                    Array<int> &type_start_idx = *start_idx[type];
//                    type_start_idx.resize(num_lods + 1);
//
//                    // Write out geometry pass instances
//                    for (size_t lod = 0; lod < num_lods; ++lod)
//                    {
//                        // Write start index for this LOD
//                        type_start_idx[lod] = dst_index;
//
//                        // Copy instances
//                        Array<int> &this_lod_indices = lod_indices[type][lod];
//                        for (int i : this_lod_indices)
//                        {
//                            const Instance &src_instance = instances[i];
//                            dst_instance[dst_index++] = src_instance;
//                        }
//
//                        // Prepare for next iteration (will preserve memory)
//                        this_lod_indices.clear();
//                    }
//
//                    // Write out end of last LOD
//                    type_start_idx.back() = dst_index;
//                }
//            }
//
//        instances_buffer->Unlock();
//    }
//
//    if (!mPrimitives.empty() || !mTempPrimitives.empty())
//    {
//        // Front face culling, we want to render the back side of the geometry for casting shadows
//        mShadowStateFF->Activate();
//
//        // Draw all primitives as seen from the light
//        if (mNumInstances > 0)
//            for (InstanceMap::value_type &v : mPrimitives)
//                DrawInstances(v.first, v.second.mLightStartIdx);
//        for (InstanceMap::value_type &v : mTempPrimitives)
//            DrawInstances(v.first, v.second.mLightStartIdx);
//    }
//
//    if (!mPrimitivesBackFacing.empty())
//    {
//        // Back face culling, we want to render the front side of back facing geometry
//        mShadowStateBF->Activate();
//
//        // Draw all primitives as seen from the light
//        for (InstanceMap::value_type &v : mPrimitivesBackFacing)
//            DrawInstances(v.first, v.second.mLightStartIdx);
//    }
//
//    if (!mWireframePrimitives.empty())
//    {
//        // Switch to wireframe mode
//        mShadowStateWire->Activate();
//
//        // Draw all wireframe primitives as seen from the light
//        for (InstanceMap::value_type &v : mWireframePrimitives)
//            DrawInstances(v.first, v.second.mLightStartIdx);
//    }
//
//    // Switch to the main render target
//    mRenderer->SetRenderTarget(nullptr);
//
//    // Bind the shadow map texture
//    mDepthTexture->Bind(2);
//
//    if (!mPrimitives.empty() || !mTempPrimitives.empty())
//    {
//        // Bind the normal shader, back face culling
//        mTriangleStateBF->Activate();
//
//        // Draw all primitives
//        if (mNumInstances > 0)
//            for (InstanceMap::value_type &v : mPrimitives)
//                DrawInstances(v.first, v.second.mGeometryStartIdx);
//        for (InstanceMap::value_type &v : mTempPrimitives)
//            DrawInstances(v.first, v.second.mGeometryStartIdx);
//    }
//
//    if (!mPrimitivesBackFacing.empty())
//    {
//        // Front face culling, the next batch needs to render inside out
//        mTriangleStateFF->Activate();
//
//        // Draw all back primitives
//        for (InstanceMap::value_type &v : mPrimitivesBackFacing)
//            DrawInstances(v.first, v.second.mGeometryStartIdx);
//    }
//
//    if (!mWireframePrimitives.empty())
//    {
//        // Wire frame mode
//        mTriangleStateWire->Activate();
//
//        // Draw all wireframe primitives
//        for (InstanceMap::value_type &v : mWireframePrimitives)
//            DrawInstances(v.first, v.second.mGeometryStartIdx);
//    }
//}
