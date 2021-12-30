#include "world_renderer.h"

#include <glad/glad.h>

#include <cmath>
#include <iostream>
#include <optional>

#include "../asset.h"
#include "../component/shader.h"
#include "../utils/mat.h"
#include "../utils/mat_opengl.h"
#include "../utils/logging.h"

std::array vertices{
    std::array{math::vec3f{1, 1, 1}, math::vec3f{1, 0, 0}},
    std::array{math::vec3f{-1, 1, 1}, math::vec3f{0, 1, 0}},
    std::array{math::vec3f{-1, -1, 1}, math::vec3f{0, 0, 1}},

    std::array{math::vec3f{1, 1, 1}, math::vec3f{1, 0, 0}},
    std::array{math::vec3f{-1, -1, 1}, math::vec3f{0, 0, 1}},
    std::array{math::vec3f{1, -1, 1}, math::vec3f{0, 1, 0}},

    std::array{math::vec3f{1, 1, 1}, math::vec3f{1, 0, 0}},
    std::array{math::vec3f{1, -1, 1}, math::vec3f{0, 1, 0}},
    std::array{math::vec3f{1, -1, -1}, math::vec3f{0, 0, 1}},

    std::array{math::vec3f{1, 1, 1}, math::vec3f{1, 0, 0}},
    std::array{math::vec3f{1, -1, -1}, math::vec3f{0, 0, 1}},
    std::array{math::vec3f{1, 1, -1}, math::vec3f{0, 1, 0}},

    std::array{math::vec3f{1, 1, 1}, math::vec3f{1, 0, 0}},
    std::array{math::vec3f{1, 1, -1}, math::vec3f{0, 1, 0}},
    std::array{math::vec3f{-1, 1, -1}, math::vec3f{0, 0, 1}},

    std::array{math::vec3f{1, 1, 1}, math::vec3f{1, 0, 0}},
    std::array{math::vec3f{-1, 1, -1}, math::vec3f{0, 0, 1}},
    std::array{math::vec3f{-1, 1, 1}, math::vec3f{0, 1, 0}},

    std::array{math::vec3f{-1, -1, -1}, math::vec3f{1, 0, 0}},
    std::array{math::vec3f{1, -1, -1}, math::vec3f{0, 1, 0}},
    std::array{math::vec3f{1, 1, -1}, math::vec3f{0, 0, 1}},

    std::array{math::vec3f{-1, -1, -1}, math::vec3f{1, 0, 0}},
    std::array{math::vec3f{1, 1, -1}, math::vec3f{0, 0, 1}},
    std::array{math::vec3f{-1, 1, -1}, math::vec3f{0, 1, 0}},

    std::array{math::vec3f{-1, -1, -1}, math::vec3f{1, 0, 0}},
    std::array{math::vec3f{-1, 1, -1}, math::vec3f{0, 1, 0}},
    std::array{math::vec3f{-1, 1, 1}, math::vec3f{0, 0, 1}},

    std::array{math::vec3f{-1, -1, -1}, math::vec3f{1, 0, 0}},
    std::array{math::vec3f{-1, 1, 1}, math::vec3f{0, 0, 1}},
    std::array{math::vec3f{-1, -1, 1}, math::vec3f{0, 1, 0}},

    std::array{math::vec3f{-1, -1, -1}, math::vec3f{1, 0, 0}},
    std::array{math::vec3f{-1, -1, 1}, math::vec3f{0, 1, 0}},
    std::array{math::vec3f{1, -1, 1}, math::vec3f{0, 0, 1}},

    std::array{math::vec3f{-1, -1, -1}, math::vec3f{1, 0, 0}},
    std::array{math::vec3f{1, -1, 1}, math::vec3f{0, 0, 1}},
    std::array{math::vec3f{1, -1, -1}, math::vec3f{0, 1, 0}},

};

WorldRenderer::WorldRenderer(Camera *camera): camera(camera) {
    std::string vertex_source = get_asset(AssetKind::AK_Shader, "base.vert");
    std::string fragment_source = get_asset(AssetKind::AK_Shader, "base.frag");

    shader.emplace(vertex_source, fragment_source);
    auto with_shader = shader->use();

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
                 GL_STATIC_DRAW);

    GLint posAttrib = shader->getAttribLocation("in_position");
    GLint barAttrib = shader->getAttribLocation("in_barposition");

    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(0));  // NOLINT
    glEnableVertexAttribArray(posAttrib);

    glVertexAttribPointer(barAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));  // NOLINT
    glEnableVertexAttribArray(barAttrib);

    glEnable(GL_DEPTH_TEST);
}

void renderOneBlock(const Camera& camera, const Shader& shader, int x, int y,
                    int z, GLuint VAO, GLuint VBO) {
    glEnableVertexAttribArray(0);
    glBindVertexArray(VAO);
    auto with_shader = shader.use();

    const auto scale = math::scale<float>(0.5);             // NOLINT
    const auto transl = math::translation<float>(x, y, z);  // NOLINT
    auto model_trans = transl * scale;
    auto world_trans = math::translation<float>(0, 0, 0);
    auto proj = camera.getCameraMatrix();

    GLint modelUnif = shader.getUniformLocation("model");
    GLint worldUnif = shader.getUniformLocation("world");
    GLint projUnif = shader.getUniformLocation("proj");
    glUniformMatrix4fv(modelUnif, 1, GL_TRUE, model_trans.ptr());
    glUniformMatrix4fv(worldUnif, 1, GL_TRUE, world_trans.ptr());
    glUniformMatrix4fv(projUnif, 1,  GL_TRUE, proj.ptr());

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glDisableVertexAttribArray(0);
}

void WorldRenderer::render() {

    camera->position = {0, 5, 0};
    camera->lookAt({2, 0, 2});

    camera->persp = math::projection<float>(1.333, M_PI/2, 1, 30);
    for (auto&& chunk : world.data) {
        // TODO write an iterator for chunk
        for (int x = 0; x < chunkWidth; x++) {
            for (int z = 0; z < chunkWidth; z++) {
                for (int y = 0; y < chunkHeight; y++) {
                    auto block = chunk.getBlock(x, y, z);

                    if (block.type == BlockType::Empty) continue;

                    renderOneBlock(*camera, *shader,
                                   chunkWidth * chunk.getId().x + x, y,
                                   chunkWidth * chunk.getId().z + z, VAO, VBO);
                }
            }
        }
    }
}
