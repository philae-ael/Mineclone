#ifndef SHADER_H_
#define SHADER_H_

#include <glad/glad.h>

#include <iostream>
#include <string>

#include "../data/shader_layout.h"
#include "../utils/logging.h"

class Shader {
   private:
    class UseShaderWithRAII {
       public:
        UseShaderWithRAII(GLuint shader_program, GLuint current_shader_): old_shader_in_use(current_shader_) {
            if(shader_program) glUseProgram(shader_program);
        }

        ~UseShaderWithRAII() {
            if (old_shader_in_use) glUseProgram(old_shader_in_use);
        }

       private:
        GLuint old_shader_in_use;
        Logger log = Logger::get({"Shader"});
    };

   public:
    Shader(const std::string &vertex_shader_source,
           const std::string &fragment_shader_source);
    Shader(Shader &) = delete;
    Shader(Shader &&other) noexcept {
        vertex_shader = other.vertex_shader;
        fragment_shader = other.fragment_shader;
        shader_program = other.shader_program;

        other.shader_program = other.fragment_shader = other.vertex_shader = 0;
    }
    ~Shader();

    GLint getAttribLocation(const char *attrib) const {
        return glGetAttribLocation(shader_program, attrib);
    }
    GLint getUniformLocation(const char *attrib) const {
        return glGetUniformLocation(shader_program, attrib);
    }

    [[nodiscard]] UseShaderWithRAII use() const {
        if (shader_program == current_shader)
            return {0, 0}; // Do nothing shader swap
        current_shader = shader_program;
        return {shader_program, current_shader};
    }

    void useLayout(const Layout &) const;

   private:
    GLuint vertex_shader, fragment_shader, shader_program;
    Logger log = Logger::get({"Shader"});

    static GLuint current_shader;
};

#endif  // !SHADER_H_
