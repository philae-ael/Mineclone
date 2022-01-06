#include "renderer.h"

#include <glad/glad.h>

#include "../utils/mat_opengl.h"
#include "world_renderer.h"

Renderer::Renderer() : world_renderer{&camera_controller} {}

void Renderer::setWindowSize(int width, int height) {
    camera_controller.updateCamera(width, height);
}

void Renderer::update(float dt) { camera_controller.update(dt); }

void Renderer::render() {
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    world_renderer.render();
}
