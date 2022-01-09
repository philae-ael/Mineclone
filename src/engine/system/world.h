#ifndef WORLD_H_
#define WORLD_H_

#include <glad/glad.h>

#include <optional>

#include "../component/chunk.h"
#include "../component/shader.h"
#include "../component/texture_atlas.h"
#include "camera_controller.h"

class World {
   public:
    World(CameraController* camera_controller);
    World(World&) = delete;
    World(World&&) = delete;
    void render();

   private:
    chunk_storage world;
    CameraController* camera_controller;
    std::optional<Shader> shader;
    std::optional<TextureAtlas> atlas;

    GLuint VBO = 0;
    GLuint VAO = 0;
};

#endif  // !WORLD_H_
