#ifndef SHADER_H_
#define SHADER_H_

#define DEBUG

#include <glad/glad.h>

#include <iostream>
#include <string>

#include "../utils/logging.h"

class Shader {
   private:
    /* The aim of this class is to use RAII to when a shader is in use
     */
    class UseShaderWithRAII {
       public:
        UseShaderWithRAII(GLuint shader_program) {
#ifdef DEBUG
            if (in_use_shader_program != 0) {
                Logger::get() << LogLevel::Warning << "Shader program is being overwriten. Is it "
                             "intentionnal ?\n";
            }
            in_use_shader_program = shader_program;
#endif
            glUseProgram(shader_program);
        }

        ~UseShaderWithRAII() {
#ifdef DEBUG
            in_use_shader_program = 0;
#endif
        }

       private:
#ifdef DEBUG
        static GLuint in_use_shader_program;
#endif
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

   private:
    GLuint vertex_shader, fragment_shader, shader_program;
};

#endif  // !SHADER_H_
