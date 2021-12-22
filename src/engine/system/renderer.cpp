#include "renderer.h"

#include <glad/glad.h>

#include <chrono>
#include <cmath>
#include <optional>

#include "../asset.h"
#include "../component/shader.h"
#include "../utils/math.h"

GLuint VBO = 0;
GLuint VAO = 0;

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

std::optional<Shader> shader;

void drawCubeInit() {
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
}

Renderer::Renderer() : begin{std::chrono::steady_clock::now()} {
    drawCubeInit();
}

void drawCube(double t) {
    auto with_shader = shader->use();
    glEnable(GL_DEPTH_TEST);

    const auto rot =
        math::rotate<float, 4>(M_PI / 2 * std::cos(t), math::vec3f{1, 1, 0});
    const auto transl = math::translation<float>(0, 0, -3);  // NOLINT
   auto model_trans = rot % math::scale<float>(0.5);
    auto world_trans = transl;
    auto proj = math::projection<float>(4. / 3., M_PI / 2, 1, 4);  // NOLINT

    GLint modelUnif = shader->getUniformLocation("model");
    GLint worldUnif = shader->getUniformLocation("world");
    GLint projUnif = shader->getUniformLocation("proj");
    glUniformMatrix4fv(modelUnif, 1, GL_TRUE, model_trans.ptr());
    glUniformMatrix4fv(worldUnif, 1, GL_TRUE, world_trans.ptr());
    glUniformMatrix4fv(projUnif, 1, GL_TRUE, proj.ptr());

    glEnableVertexAttribArray(0);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glDisableVertexAttribArray(0);
}

void Renderer::render() const {
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    std::chrono::duration<double> fp_ms = end - begin;

    double t = fp_ms.count();

    drawCube(t);
}
