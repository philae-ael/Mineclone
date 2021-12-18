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
    math::vec3f{-1, -1, 0},
    math::vec3f{1, -1, 0},
    math::vec3f{-1, 1, 0},
    math::vec3f{1, 1, 0},
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
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(posAttrib);
}

Renderer::Renderer() : begin{std::chrono::steady_clock::now()} {
    drawCubeInit();
}

void drawCube(double t) {
    auto with_shader = shader->use();

    math::mat3f scale =
        std::cos(t) * (1.f / 4.f) * math::identity<float, 3>();  // NOLINT
    math::mat3f rot =
        math::rotate(M_PI * std::cos(t), math::vec3f{0.f, 0.f, -1.f});
    auto transform3 = math::dot(rot, scale);

    math::mat4f transform4 = math::extend<float, 3, 3, 4>(transform3);

    GLint transformUnif = shader->getUniformLocation("transform");
    glUniformMatrix4fv(transformUnif, 1, GL_FALSE, transform4.ptr());

    glEnableVertexAttribArray(0);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);
}

void Renderer::render() const {
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    std::chrono::duration<double> fp_ms = end - begin;

    double t = fp_ms.count();

    drawCube(t);
}
