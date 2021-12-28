#include "renderer.h"

#include <glad/glad.h>

Renderer::Renderer() = default;

void Renderer::render() {
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    world_renderer.render();

}
