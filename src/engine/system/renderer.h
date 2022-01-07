#ifndef RENDERER_H
#define RENDERER_H

#include "camera_controller.h"
#include "world_renderer.h"

class Renderer {
   public:
    Renderer() = default;
    Renderer(Renderer &&) = delete;
    Renderer(const Renderer &) = delete;
    Renderer &operator=(Renderer &&) = delete;
    Renderer &operator=(const Renderer &) = delete;
    ~Renderer() = default;

    void render();
    void update(float dt);
    void setWindowSize(int width, int height);

   private:
    CameraController camera_controller;
    WorldRenderer world_renderer{&camera_controller};
    Logger log{Logger::get({"Renderer"})};
};

#endif  // !RENDERER_H
