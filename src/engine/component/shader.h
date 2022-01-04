#ifndef SHADER_H_
#define SHADER_H_

#include <glad/glad.h>

#include <iostream>
#include <string>

#include "../utils/logging.h"
#include "shader_layout.h"

class Shader {
   private:
    class UseShaderWithRAII {
       public:
        UseShaderWithRAII(GLuint shader_program) {
            glGetIntegerv(GL_CURRENT_PROGRAM, &old_shader_in_use);
            glUseProgram(shader_program);
        }

        ~UseShaderWithRAII() {
            if (old_shader_in_use) glUseProgram(old_shader_in_use);
        }

       private:
        GLint old_shader_in_use = 0;
    };

   public:
    Shader(const std::string &vertex_shader_source,
           const std::string &fragment_shader_source);
    Shader(Shader &&) = default;
    Shader(const Shader &) = default;
    Shader &operator=(Shader &&) = default;
    Shader &operator=(const Shader &) = default;
    ~Shader();

    GLint getAttribLocation(const char *attrib) const {
        return glGetAttribLocation(shader_program, attrib);
    }
    GLint getUniformLocation(const char *attrib) const {
        return glGetUniformLocation(shader_program, attrib);
    }

    [[nodiscard]] UseShaderWithRAII use() const {
        return UseShaderWithRAII{shader_program};
    }

    void useLayout(const Layout &);

   private:
    GLuint vertex_shader, fragment_shader, shader_program;

    // We are storing the current layout only to reflect that we are changing
    // some "internal"
    const Layout *current_layout = nullptr;
};

#endif  // !SHADER_H_
