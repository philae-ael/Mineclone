#ifndef WORLD_H_
#define WORLD_H_

#include <glad/glad.h>

#include <optional>

#include "../component/chunk.h"
#include "../component/renderer_context.h"
#include "../component/shader.h"
#include "../component/texture_atlas.h"
#include "camera_controller.h"
#include "player_controller.h"

class World {
   public:
    World(const CameraController* camera_controller,
          const PlayerController* player_controller);
    World(World&) = delete;
    World(World&&) = delete;
    void render();
    void update(float dt);

   private:
    using chunk_data = std::pair<ChunkSimplifyerProxy, RendererContext>;
    [[nodiscard]] chunk_data make_chunk_plus_context(int x, int z) const;

    chunk_array<chunk_data> world;
    const CameraController* camera_controller;
    const PlayerController* player_controller;
    std::optional<Shader> shader;
    std::optional<TextureAtlas> atlas;
};

#endif  // !WORLD_H_
