#include "world_renderer.h"

#include <glad/glad.h>

#include <cmath>
#include <iostream>
#include <optional>

#include "../asset.h"
#include "../component/shader.h"
#include "../utils/mat.h"
#include "../utils/mat_opengl.h"

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

WorldRenderer::WorldRenderer() {
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

void renderOneBlock(const Shader& shader, int x, int y, int z, GLuint VAO,
                    GLuint VBO) {
    std::cout << x << " " << y << " " << z << std::endl;
    glEnableVertexAttribArray(0);
    glBindVertexArray(VAO);
    auto with_shader = shader.use();

    const auto transl = math::translation<float>(x, y, z);  // NOLINT
    auto model_trans = transl;
    auto world_trans = math::rotate<float, 4>(M_PI / 3, {1, 0, 0}) *
                       math::translation<float>(0, -5, -20);
    auto proj = math::projection<float>(4. / 3., M_PI / 2, 1, 40);  // NOLINT

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
    std::cout << "==" << std::endl;
    for (auto&& chunk : world.data) {
        // TODO write an iterator for chunk
        for (int x = 0; x < chunkWidth; x++) {
            for (int z = 0; z < chunkWidth; z++) {
                for (int y = 0; y < chunkHeight; y++) {
                    auto block = chunk.getBlock(x, y, z);

                    if (block.type == BlockType::Empty) continue;

                    renderOneBlock(*shader, chunkWidth * chunk.getId().x + x, y,
                                   chunkWidth * chunk.getId().z + z, VAO, VBO);
                }
            }
        }
    }
}
