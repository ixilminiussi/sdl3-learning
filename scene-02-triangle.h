#ifndef SCENE_02_TRIANGLE
#define SCENE_02_TRIANGLE

#include "scene.h"
#include <SDL3/SDL_gpu.h>

class Scene02Triangle : public Scene
{
  public:
    void Load(Renderer &renderer) override;
    bool Update(float dt) override;
    void Draw(Renderer &renderer) override;
    void Unload(Renderer &renderer) override;

  private:
    InputState inputState;
    const char *basePath;
    SDL_GPUShader *vertexShader;
    SDL_GPUShader *fragmentShader;
    SDL_GPUGraphicsPipeline *fillPipeline;
    SDL_GPUGraphicsPipeline *linePipeline;
    SDL_GPUViewport smallViewport = {160, 120, 320, 240, 0.1f, 1.0f};
    SDL_Rect scissorRect = {320, 240, 320, 240};
    bool useWireframeMode = false;
    bool useSmallViewport = false;
    bool useScissorRect = false;
};

#endif
