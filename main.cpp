#include "renderer.h"
#include "scene-01-clear.h"
#include "scene-02-triangle.h"
#include "scene-03-triangle-vertex-buffer.h"
#include "scene-04-triangle-cull-modes.h"
#include "scene-06-triangle-indexed.h"
#include "scene-07-texture-quad.h"
#include "scene-08-texture-quad-moving.h"
#include "time.h"
#include "window.h"

#include <SDL3/SDL_main.h>
#include <iostream>
#include <memory>

using namespace std;

int main(int argc, char **argv)
{
    Window window{};
    Renderer renderer{};
    Time time{};
    window.Init();
    renderer.Init(window);
    auto scene = std::make_unique<Scene08TextureQuadMoving>();
    scene->Load(renderer);
    bool isRunning{true};
    while (isRunning)
    {
        const float dt = time.ComputeDeltaTime();
        isRunning = scene->Update(dt);
        scene->Draw(renderer);
        time.DelayTime();
    }
    scene->Unload(renderer);
    renderer.Close();
    window.Close();
    return 0;
}
