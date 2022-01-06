#ifndef RENDERER_H
#define RENDERER_H

#include "camera_controller.h"
#include "world_renderer.h"

class Renderer {
   public:
    Renderer();
    Renderer(Renderer &&) = delete;
    Renderer(const Renderer &) = delete;
    Renderer &operator=(Renderer &&) = delete;
    Renderer &operator=(const Renderer &) = delete;
    ~Renderer() = default;

    void render();
    void update(float dt);
    void setWindowSize(int width, int height);

   private:
    // Relative position is important! camera_controller
    // will be created before world_renderer
    CameraController camera_controller;
    WorldRenderer world_renderer;
    Logger log{Logger::get({"Renderer"})};
};

#endif  // !RENDERER_H
