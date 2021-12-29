#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H

#include <glad/glad.h>

#include <optional>

#include "../component/shader.h"
#include "../component/camera.h"
#include "../component/world.h"

class WorldRenderer {
   public:
    WorldRenderer(Camera* camera);
    WorldRenderer(WorldRenderer&) = delete;
    WorldRenderer(WorldRenderer&&) = delete;
    void render();

   private:
    World world;
    Camera *camera;
    std::optional<Shader> shader;

    GLuint VBO = 0;
    GLuint VAO = 0;
};

#endif  // !WORLD_RENDERER_H
