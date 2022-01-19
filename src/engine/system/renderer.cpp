#include "renderer.h"

#include <glad/glad.h>

#include "../utils/mat_opengl.h"
#include "world.h"

void Renderer::setWindowSize(int width, int height) {
    camera_controller.updateCameraViewport(width, height);
}

void Renderer::update(float dt) {
    camera_controller.update(dt);
    player_controller.update(dt);
    world_renderer.update(dt);
}

void Renderer::render() {
    PROFILE_SCOPED_FPS();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    world_renderer.render();
}
