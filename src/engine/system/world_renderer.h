#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H

#include <glad/glad.h>

#include <optional>

#include "../component/shader.h"
#include "camera_controller.h"
#include "../component/world.h"

class WorldRenderer {
   public:
    WorldRenderer(CameraController* camera_controller);
    WorldRenderer(WorldRenderer&) = delete;
    WorldRenderer(WorldRenderer&&) = delete;
    void render();

   private:
    World world;
    CameraController *camera_controller;
    std::optional<Shader> shader;

    GLuint VBO = 0;
    GLuint VAO = 0;
    GLuint texture = 0;
};

#endif  // !WORLD_RENDERER_H
