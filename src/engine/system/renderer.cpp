#include "renderer.h"

#include <glad/glad.h>

#include "world_renderer.h"
#include "../utils/mat_opengl.h"

Renderer::Renderer() : world_renderer{&camera} {}

void Renderer::setWindowSize(int width, int height){
    camera.persp = math::projection<float>((float)width/(float)height, M_PI/2, 1, 40);
}

void Renderer::render() {
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    world_renderer.render();
}
