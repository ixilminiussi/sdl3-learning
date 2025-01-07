#ifndef SCENE_03_TRIANGLE_VERTEX_BUFFER
#define SCENE_03_TRIANGLE_VERTEX_BUFFER

#include "scene.h"
#include <SDL3/SDL_gpu.h>

class Scene03TriangleVertexBuffer : public Scene
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
  SDL_GPUGraphicsPipeline *pipeline;
  SDL_GPUBuffer *vertexBuffer;
};

#endif
