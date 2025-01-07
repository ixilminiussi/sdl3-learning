#ifndef SCENE_06_TRIANGLE_INDEXED
#define SCENE_06_TRIANGLE_INDEXED

#include <SDL3/SDL_gpu.h>
#include "scene.h"

#include <array>
#include <string>

using std::array;
using std::string;

class Scene06TriangleIndexed : public Scene
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
    SDL_GPUBuffer *indexBuffer;
    bool useIndexBuffer{true};
};

#endif