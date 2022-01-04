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
            log << "Swapping from shader " << old_shader_in_use << " to "
                << shader_program;

            glUseProgram(shader_program);
        }

        ~UseShaderWithRAII() {
            log << "Swapping back to shader " << old_shader_in_use;
            if (old_shader_in_use) glUseProgram(old_shader_in_use);
        }

       private:
        GLint old_shader_in_use = 0;
        Logger log{Logger::get("Shader")};
    };

   public:
    Shader(const std::string &vertex_shader_source,
           const std::string &fragment_shader_source);
    Shader(Shader &&other) noexcept {
        vertex_shader = other.vertex_shader;
        fragment_shader = other.fragment_shader;
        shader_program = other.shader_program;
        current_layout = other.current_layout;

        other.shader_program = other.fragment_shader = other.vertex_shader = 0;
        other.current_layout = nullptr;
    }
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
