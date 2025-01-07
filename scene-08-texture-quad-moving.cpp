#include "scene-08-texture-quad-moving.h"

#include "mat4.h"
#include "cube.h"
#include "renderer.h"
#include <SDL3/SDL.h>

void Scene08TextureQuadMoving::Load(Renderer &renderer)
{
    basePath = SDL_GetBasePath();
    vertexShader = renderer.LoadShader(basePath, "TexturedQuadWithMatrix.vert", 0, 1, 0,
                                       0);
    fragmentShader = renderer.LoadShader(basePath, "TexturedQuadWithMultiplyColor.frag",
                                         1, 1, 0, 0);
    SDL_Surface *imageData = renderer.LoadBMPImage(basePath, "cube0.bmp", 4);
    if (imageData == nullptr)
    {
        SDL_Log("Could not load image data!");
    }
    // Create the pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        // This is set up to match the vertex shader layout!
        .vertex_input_state = SDL_GPUVertexInputState{
            .vertex_buffer_descriptions = new SDL_GPUVertexBufferDescription[1]{{
                .slot = 0,
                .pitch = sizeof(PositionTextureVertex),
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
            }},
            .num_vertex_buffers = 1,
            .vertex_attributes = new SDL_GPUVertexAttribute[2]{{.location = 0,
                                                                .buffer_slot = 0,
                                                                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                                                                .offset = 0},
                                                               {.location = 1,
                                                                .buffer_slot = 0,
                                                                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                                                                .offset = sizeof(float) * 3}},
            .num_vertex_attributes = 2,
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .depth_stencil_state = {.compare_op = SDL_GPU_COMPAREOP_LESS, .enable_depth_test = true, .enable_depth_write = true},
        .target_info = {.color_target_descriptions = new SDL_GPUColorTargetDescription[1]{{.format = SDL_GetGPUSwapchainTextureFormat(renderer.device, renderer.renderWindow), .blend_state = {
                                                                                                                                                                                   .src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                                                                                                                                                                                   .dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                                                                                                                                                                                   .color_blend_op = SDL_GPU_BLENDOP_ADD,
                                                                                                                                                                                   .src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
                                                                                                                                                                                   .dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                                                                                                                                                                                   .alpha_blend_op = SDL_GPU_BLENDOP_ADD,
                                                                                                                                                                                   .enable_blend = true,
                                                                                                                                                                               }}},
                        .num_color_targets = 1,
                        .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
                        .has_depth_stencil_target = true}};
    pipeline = renderer.CreateGPUGraphicsPipeline(pipelineCreateInfo);
    // Clean up shader resources
    renderer.ReleaseShader(vertexShader);
    renderer.ReleaseShader(fragmentShader);
    // Texture sampler
    sampler = renderer.CreateSampler(SDL_GPUSamplerCreateInfo{
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
    });
    // Create the vertex buffer
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(PositionTextureVertex) * 24};
    vertexBuffer = renderer.CreateBuffer(vertexBufferCreateInfo);
    renderer.SetBufferName(vertexBuffer, "Ravioli Vertex Buffer");
    // Create the index buffer
    SDL_GPUBufferCreateInfo indexBufferCreateInfo = {
        .usage = SDL_GPU_BUFFERUSAGE_INDEX,
        .size = sizeof(Uint16) * 36};
    indexBuffer = renderer.CreateBuffer(indexBufferCreateInfo);
    // Create depth texture
    SDL_GPUTextureCreateInfo depthTextureInfo{
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
        .width = static_cast<Uint32>(640),
        .height = static_cast<Uint32>(480),
        .layer_count_or_depth = 1,
        .num_levels = 1,
    };
    depthTexture = renderer.CreateTexture(depthTextureInfo);
    // Create texture
    SDL_GPUTextureCreateInfo textureInfo{
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = static_cast<Uint32>(imageData->w),
        .height = static_cast<Uint32>(imageData->h),
        .layer_count_or_depth = 1,
        .num_levels = 1,
    };
    texture = renderer.CreateTexture(textureInfo);
    renderer.SetTextureName(texture, "Ravioli Texture");
    // Set the buffer data

    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = (sizeof(PositionTextureVertex) * 24) + (sizeof(Uint16) * 36),
    };
    SDL_GPUTransferBuffer *transferBuffer =
        renderer.CreateTransferBuffer(transferBufferCreateInfo);
    // Map the transfer buffer and fill it with data (data is bound to the transfer buffer)
    auto transferData = static_cast<PositionTextureVertex *>(
        renderer.MapTransferBuffer(transferBuffer, false));
    for (int i = 0; i < 24; i++)
    {
        transferData[i] = defaultCube.vertices[i];
    }
    auto indexData = reinterpret_cast<Uint16 *>(&transferData[24]);
    for (int i = 0; i < 36; i++)
    {
        indexData[i] = defaultCube.indices[i];
    }
    renderer.UnmapTransferBuffer(transferBuffer);
    // Setup texture transfer buffer
    Uint32 bufferSize = imageData->w * imageData->h * 4;
    SDL_GPUTransferBufferCreateInfo textureTransferBufferCreateInfo{
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = bufferSize};
    SDL_GPUTransferBuffer *textureTransferBuffer =
        renderer.CreateTransferBuffer(textureTransferBufferCreateInfo);
    auto textureTransferData = static_cast<PositionTextureVertex *>(
        renderer.MapTransferBuffer(textureTransferBuffer, false));
    std::memcpy(textureTransferData, imageData->pixels, bufferSize);
    renderer.UnmapTransferBuffer(textureTransferBuffer);
    renderer.BeginUploadToBuffer();
    // Upload the transfer data to the vertex and index buffer
    SDL_GPUTransferBufferLocation transferVertexBufferLocation{
        .transfer_buffer = transferBuffer,
        .offset = 0};
    SDL_GPUBufferRegion vertexBufferRegion{
        .buffer = vertexBuffer,
        .offset = 0,
        .size = sizeof(PositionTextureVertex) * 24};
    SDL_GPUTransferBufferLocation transferIndexBufferLocation{
        .transfer_buffer = transferBuffer,
        .offset = sizeof(PositionTextureVertex) * 24};
    SDL_GPUBufferRegion indexBufferRegion{
        .buffer = indexBuffer,
        .offset = 0,
        .size = sizeof(Uint16) * 36};
    SDL_GPUTextureTransferInfo textureBufferLocation{
        .transfer_buffer = textureTransferBuffer,
        .offset = 0};
    SDL_GPUTextureRegion textureBufferRegion{
        .texture = texture,
        .w = static_cast<Uint32>(imageData->w),
        .h = static_cast<Uint32>(imageData->h),
        .d = 1};
    renderer.UploadToBuffer(transferVertexBufferLocation, vertexBufferRegion, false);
    renderer.UploadToBuffer(transferIndexBufferLocation, indexBufferRegion, false);
    renderer.UploadToTexture(textureBufferLocation, textureBufferRegion, false);
    renderer.EndUploadToBuffer(transferBuffer);
    renderer.ReleaseTransferBuffer(textureTransferBuffer);
    renderer.ReleaseSurface(imageData);
}

bool Scene08TextureQuadMoving::Update(float dt)
{
    const bool isRunning = ManageInput(inputState);
    time += 2 * dt;
    return isRunning;
}

void Scene08TextureQuadMoving::Unload(Renderer &renderer)
{
    renderer.ReleaseSampler(sampler);
    renderer.ReleaseBuffer(vertexBuffer);
    renderer.ReleaseBuffer(indexBuffer);
    renderer.ReleaseTexture(texture);
    renderer.ReleaseTexture(depthTexture);
    renderer.ReleaseGraphicsPipeline(pipeline);
}

void Scene08TextureQuadMoving::Draw(Renderer &renderer)
{
    SDL_zero(depth_target);
    depth_target.clear_depth = 1.0f;
    depth_target.load_op = SDL_GPU_LOADOP_CLEAR;
    depth_target.store_op = SDL_GPU_STOREOP_DONT_CARE;
    depth_target.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
    depth_target.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
    depth_target.texture = depthTexture;

    depth_target.cycle = true;

    renderer.Begin(&depth_target);
    renderer.BindGraphicsPipeline(pipeline);
    SDL_GPUBufferBinding vertexBindings{.buffer = vertexBuffer, .offset = 0};
    renderer.BindVertexBuffers(0, vertexBindings, 1);
    SDL_GPUBufferBinding indexBindings{.buffer = indexBuffer, .offset = 0};
    renderer.BindIndexBuffer(indexBindings, SDL_GPU_INDEXELEMENTSIZE_16BIT);
    SDL_GPUTextureSamplerBinding textureSamplerBinding{.texture = texture, .sampler = sampler};
    renderer.BindFragmentSamplers(0, textureSamplerBinding, 1);

    Mat4 matrixUniform =
        Mat4::CreateRotationX(time) *
        Mat4::CreateRotationY(time) *
        Mat4::CreateRotationZ(time) *
        Mat4::CreateTranslation(0, 0, 0);
    renderer.PushVertexUniformData(0, &matrixUniform, sizeof(matrixUniform));
    FragMultiplyUniform fragMultiplyUniform{0.5f + SDL_sinf(time) * 0.5f, 0.5f + SDL_sinf(time) * 0.5f, 0.5f + SDL_sinf(time) * 0.5f,
                                            1.0f};
    renderer.PushFragmentUniformData(0, &fragMultiplyUniform,
                                     sizeof(FragMultiplyUniform));
    renderer.DrawIndexedPrimitives(36, 1, 0, 0, 0);
    renderer.End();
}