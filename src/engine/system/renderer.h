#ifndef RENDERER_H
#define RENDERER_H

#include "world_renderer.h"
#include "../component/camera.h"

class Renderer {
   public:
    Renderer();
    Renderer(Renderer &&) = delete;
    Renderer(const Renderer &) = delete;
    Renderer &operator=(Renderer &&) = delete;
    Renderer &operator=(const Renderer &) = delete;
    ~Renderer() = default;

    void render();

   private:
    WorldRenderer world_renderer;
    Camera camera;
};

#endif  // !RENDERER_H
