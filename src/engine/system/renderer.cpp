#include "./renderer.h"

#include <glad/glad.h>

void Renderer::render() const {
   glClearColor(0.35f, 0.25f, 0.15f, 0.5f);   
   glClear(GL_COLOR_BUFFER_BIT);
}
