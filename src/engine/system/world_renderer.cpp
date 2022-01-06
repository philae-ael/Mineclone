#include "world_renderer.h"

#include <glad/glad.h>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <optional>

#define STB_IMAGE_IMPLEMENTATION
#include "../../stb_image.h"
#include "../asset.h"
#include "../component/block_texture.h"
#include "../component/mesh.h"
#include "../component/shader.h"
#include "../component/shader_layout.h"
#include "../component/world.h"
#include "../utils/logging.h"
#include "../utils/mat.h"
#include "../utils/mat_opengl.h"

const std::size_t max_size = 30000 * sizeof(BlockVertex);
WorldRenderer::WorldRenderer(CameraController* camera_controller)
    : camera_controller(camera_controller) {
    camera_controller->getCamera().position = {4, 5, 4};
    camera_controller->lookAt({0, 0, 0});

    shader.emplace(get_asset<AssetKind::Shader>("base"));
    auto with_shader = shader->use();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferData(GL_ARRAY_BUFFER, max_size, nullptr, GL_DYNAMIC_DRAW);

    Layout layout{{
        {"in_position", LayoutType::Float, 3},
        {"in_texposition", LayoutType::Float, 2},
        {"in_textid", LayoutType::Int, 1},
    }};

    shader->useLayout(layout);

    {  // LOAD TEXTURE
        std::string path = get_asset_path(AssetKind::Texture, "minecraft.png");
        int x, y, n;
        unsigned char* data = stbi_load(path.c_str(), &x, &y, &n, 0);

        const int tiles = 16;
        int subWidth = x / tiles;
        int subHeight = y / tiles;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_SRGB8_ALPHA8, subWidth,
                     subHeight, tiles * tiles, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     nullptr);

        glPixelStorei(GL_UNPACK_ROW_LENGTH, x);
        for (int i = 0; i < tiles; i++)
            for (int j = 0; j < tiles; j++)
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, tiles * j + i,
                                subWidth, subHeight, 1, GL_RGBA,
                                GL_UNSIGNED_BYTE,
                                data + j * x * n + i * subWidth * n);

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        stbi_image_free(data);
    }  // END LAOD TEXTURE

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void renderMeshChunk(const CameraController* camera_controller, Shader& shader,
                     const ChunkSimplifyerProxy& chunk, GLuint VAO) {
    const auto& mesh = chunk.mesh;

    glEnableVertexAttribArray(0);
    glBindVertexArray(VAO);

    glBufferSubData(GL_ARRAY_BUFFER, 0, mesh.bytes(), mesh.data());
    auto with_shader = shader.use();

    auto model_trans = mesh.getModelMatrix();
    auto [chunk_x, chunk_z] = chunk.getId();
    auto world_trans =
        math::translation<float>(chunkWidth * chunk_x, 0, chunkWidth * chunk_z);
    auto proj = camera_controller->getCameraMatrix();

    GLint modelUnif = shader.getUniformLocation("model");
    GLint worldUnif = shader.getUniformLocation("world");
    GLint projUnif = shader.getUniformLocation("proj");
    glUniformMatrix4fv(modelUnif, 1, GL_TRUE, model_trans.ptr());
    glUniformMatrix4fv(worldUnif, 1, GL_TRUE, world_trans.ptr());
    glUniformMatrix4fv(projUnif, 1, GL_TRUE, proj.ptr());

    glDrawArrays(GL_TRIANGLES, 0, mesh.size());
    glDisableVertexAttribArray(0);
}

void WorldRenderer::render() {
    for (auto&& chunk : world.data) {
        assert(chunk.mesh.bytes() < max_size);
        renderMeshChunk(camera_controller, *shader, chunk, VAO);
    }
}
