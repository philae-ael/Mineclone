#ifndef RENDERER_CONTEXT_H_
#define RENDERER_CONTEXT_H_

#include <glad/glad.h>

class RendererContext {
    class RendererContextRAII {
       public:
        RendererContextRAII(GLuint vao, GLuint current_vao)
            : old_vao(current_vao) {
            if (vao) glBindVertexArray(vao);
        }
        ~RendererContextRAII() {
            if (old_vao) glBindVertexArray(old_vao);
        }

       private:
        GLuint old_vao;
    };

   public:
    RendererContext() { glGenVertexArrays(1, &vao); }
    [[nodiscard]] RendererContextRAII use() const {
        if (vao == current_vao) return {0, 0};

        current_vao = vao;
        return {vao, current_vao};
    }

   private:
    GLuint vao = 0;

    static GLuint current_vao;
};

#endif  // !RENDERER_CONTEXT_H_
