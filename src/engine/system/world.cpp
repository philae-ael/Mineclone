#include "world.h"

#include <glad/glad.h>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <optional>

#include "../component/chunk.h"
#include "../component/mesh.h"
#include "../component/shader.h"
#include "../data/asset.h"
#include "../data/block_texture.h"
#include "../data/shader_layout.h"
#include "../utils/logging.h"
#include "../utils/mat.h"
#include "../utils/mat_opengl.h"
#include "camera_controller.h"

typename World::chunk_data World::make_chunk_plus_context(int x, int z) const {
    ChunkSimplifyerProxy chunk{{x, z}};
    RendererContext renderer_context;

    const auto& mesh = chunk.mesh;
    const auto ctx = renderer_context.use();

    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(mesh.bytes()),
                 mesh.data(), GL_STATIC_DRAW);

    auto with_shader = shader->use();
    shader->useLayout(decltype(chunk)::mesh_type::layout);

    atlas->bind();

    return {std::move(chunk), renderer_context};
}

const std::size_t max_size = 30000 * sizeof(BlockVertex);
World::World(const CameraController* camera_controller,
             const PlayerController* player_controller)
    : camera_controller(camera_controller),
      player_controller(player_controller) {
    shader.emplace(get_asset<AssetKind::Shader>("base"));
    atlas.emplace(get_asset<AssetKind::TextureAtlas>("minecraft.png", 16, 16));

    world.set_factory(std::bind_front(&World::make_chunk_plus_context, this));
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

chunk_identifier get_chunk_id(const math::vec3f& pos) {
    return {.x = static_cast<int>(pos[0] / chunkWidth),
            .z = static_cast<int>(pos[2] / chunkWidth)};
}

void World::update(float /*dt*/) {
    chunk_identifier player_chunk_id =
        get_chunk_id(player_controller->player.self.position);

    world.set_current_position(player_chunk_id.x, player_chunk_id.z);
}

void renderMeshChunk(const CameraController* camera_controller, Shader& shader,
                     const ChunkSimplifyerProxy& chunk,
                     const RendererContext& renderer_context) {
    const auto& mesh = chunk.mesh;
    auto ctx = renderer_context.use();
    // atlas->bind();

    auto with_shader = shader.use();

    auto model_trans = mesh.getModelMatrix();
    auto [chunk_x, chunk_z] = chunk.getId();
    auto world_trans =
        math::translation<float>(static_cast<float>(chunkWidth * chunk_x), 0,
                                 static_cast<float>(chunkWidth * chunk_z));
    auto proj = camera_controller->getCameraMatrix();

    GLint modelUnif = shader.getUniformLocation("model");
    GLint worldUnif = shader.getUniformLocation("world");
    GLint projUnif = shader.getUniformLocation("proj");
    glUniformMatrix4fv(modelUnif, 1, GL_TRUE, model_trans.ptr());
    glUniformMatrix4fv(worldUnif, 1, GL_TRUE, world_trans.ptr());
    glUniformMatrix4fv(projUnif, 1, GL_TRUE, proj.ptr());

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh.size()));
}

void World::render() {
    for (auto& [chunk, renderer_context] : world) {
        assert(chunk.mesh.bytes() < max_size);
        renderMeshChunk(camera_controller, *shader, chunk, renderer_context);
    }
}
