#ifndef RENDERER
#define RENDERER

#include <SDL3/SDL_gpu.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

class Window;

class Renderer
{
  public:
    void Init(Window &window);
    void Begin(SDL_GPUDepthStencilTargetInfo *depthStencilTargetInfo = nullptr);
    void End() const;
    void Close() const;
    SDL_GPUDevice *device{nullptr};
    SDL_Window *renderWindow{nullptr};
    SDL_GPUCommandBuffer *cmdBuffer{nullptr};
    SDL_GPUTexture *swapchainTexture{nullptr};
    SDL_GPURenderPass *renderPass{nullptr};

    SDL_GPUShader *LoadShader(const char *basePath, const char *shaderFilename, Uint32 samplerCount,
                              Uint32 uniformBufferCount, Uint32 storageBufferCount, Uint32 storageTextureCount);
    void ReleaseShader(SDL_GPUShader *shader) const;
    SDL_GPUGraphicsPipeline *CreateGPUGraphicsPipeline(const SDL_GPUGraphicsPipelineCreateInfo &createInfo) const;
    void BindGraphicsPipeline(SDL_GPUGraphicsPipeline *pipeline) const;
    void DrawPrimitives(int numVertices, int numInstances, int firstVertex, int firstInstance) const;
    void SetViewport(const SDL_GPUViewport &viewport) const;
    void SetScissorRect(const SDL_Rect &rect) const;
    void ReleaseGraphicsPipeline(SDL_GPUGraphicsPipeline *pipeline) const;
    SDL_GPUBuffer *CreateBuffer(const SDL_GPUBufferCreateInfo &createInfo) const;
    SDL_GPUTransferBuffer *CreateTransferBuffer(const SDL_GPUTransferBufferCreateInfo &createInfo) const;
    void *MapTransferBuffer(SDL_GPUTransferBuffer *transferBuffer, bool cycle) const;
    void UnmapTransferBuffer(SDL_GPUTransferBuffer *transferBuffer) const;
    void ReleaseTransferBuffer(SDL_GPUTransferBuffer *transferBuffer) const;
    void BeginUploadToBuffer();
    void UploadToBuffer(const SDL_GPUTransferBufferLocation &source, const SDL_GPUBufferRegion &destination,
                        bool cycle) const;
    void EndUploadToBuffer(SDL_GPUTransferBuffer *transferBuffer) const;
    void BindVertexBuffers(Uint32, const SDL_GPUBufferBinding &, Uint32) const;
    void ReleaseBuffer(SDL_GPUBuffer *) const;

    SDL_GPUCommandBuffer *uploadCmdBuf{nullptr};
    SDL_GPUCopyPass *copyPass{nullptr};
};

#endif // RENDERER
