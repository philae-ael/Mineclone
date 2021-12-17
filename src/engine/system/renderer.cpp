#include "renderer.h"

#include <glad/glad.h>
#include <optional>

#include "../utils/math.h"
#include "../component/shader.h"

GLuint VBO = 0;
GLuint VAO = 0;

std::array vertices{
    math::vec3f{-1, -1, 0},
    math::vec3f{1, -1, 0},
    math::vec3f{0, 1, 0},
};

const std::string VERTEX_SHADER_SOURCE_ENTITIES = R""""(
#version 460 core

in vec3 in_position;

void main(void) {
    gl_Position = vec4(in_position.x, in_position.y, 0.0, 1.0);
}
)"""";

const std::string FRAGMENT_SHADER_SOURCE_ENTITIES = R""""(
#version 460 core

out vec4 color;

void main()
{
    color = vec4(1.0, 0.0, 0.0, 0.0);
}
)"""";

std::optional<Shader> shader;

void drawCubeInit() {
    shader.emplace(VERTEX_SHADER_SOURCE_ENTITIES, FRAGMENT_SHADER_SOURCE_ENTITIES);

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices.data(), GL_STATIC_DRAW);

    GLint posAttrib = shader->getAttribLocation("in_position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(posAttrib);
}

Renderer::Renderer() { drawCubeInit(); }

void drawCube() {
    auto with_shader = shader->use();

    glEnableVertexAttribArray(0);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
}

void Renderer::render() const {
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawCube();
}
