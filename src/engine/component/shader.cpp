#include "shader.h"

#include <glad/glad.h>

#include <stdexcept>

#include "shader_layout.h"

#ifdef DEBUG
GLuint Shader::UseShaderWithRAII::in_use_shader_program = 0;
#endif

Shader::Shader(const std::string& vertex_shader_source,
               const std::string& fragment_shader_source) {
    enum class ShaderCreationError {
        SCE_SUCCESS,
        SCE_VERTEX_SHADER,
        SCE_FRAGMENT_SHADER,
        SCE_PROGRAM_SHADER,
    };
    using enum ShaderCreationError;

    auto compileShader = [](const std::string& source, GLuint& dest,
                            GLenum kind) {
        int iscompiled;
        dest = glCreateShader(kind);

        const char* csource = source.c_str();
        glShaderSource(dest, 1, (const GLchar**)&csource, nullptr);
        glCompileShader(dest);

        glGetShaderiv(dest, GL_COMPILE_STATUS, &iscompiled);

        return iscompiled == GL_TRUE;
    };

    auto errorCompileStr = [](GLuint shaderid) -> std::string {
        std::string err_str;
        int max_length;
        glGetShaderiv(shaderid, GL_INFO_LOG_LENGTH, &max_length);
        err_str.resize(max_length);

        glGetShaderInfoLog(shaderid, max_length, &max_length, err_str.data());
        return err_str;
    };

    auto initShader = [&, this]() -> ShaderCreationError {
        if (!compileShader(vertex_shader_source, vertex_shader,
                           GL_VERTEX_SHADER))
            return SCE_VERTEX_SHADER;
        if (!compileShader(fragment_shader_source, fragment_shader,
                           GL_FRAGMENT_SHADER))
            return SCE_FRAGMENT_SHADER;

        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);

        // TODO: glBindWhatNeeded, When needed

        glLinkProgram(shader_program);

        int islinked;
        glGetProgramiv(shader_program, GL_LINK_STATUS, &islinked);
        if (islinked == GL_FALSE) return SCE_PROGRAM_SHADER;

        return SCE_SUCCESS;
    };

    auto err = initShader();
    switch (err) {
        case SCE_VERTEX_SHADER:
            throw std::runtime_error(errorCompileStr(vertex_shader));
            break;
        case SCE_FRAGMENT_SHADER:
            glDeleteShader(vertex_shader);
            throw std::runtime_error(errorCompileStr(fragment_shader));
            break;
        case SCE_PROGRAM_SHADER: {
            // cleanup
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);

            std::string err_str;
            int max_length;
            glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &max_length);

            err_str.resize(max_length);

            glGetProgramInfoLog(shader_program, max_length, &max_length,
                                err_str.data());
            throw std::runtime_error(err_str);
        } break;
        case SCE_SUCCESS:
            break;
    }
}

Shader::~Shader() {
    glDetachShader(shader_program, vertex_shader);
    glDetachShader(shader_program, fragment_shader);
    glDeleteProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::useLayout(const Layout& layout) {
    auto self = use();

    for (const auto& item : layout.items) {
        GLint attrib = getAttribLocation(item.attibute_name);

        glVertexAttribPointer(attrib, static_cast<GLint>(item.length),
                              layoutTypeGL(item.type), GL_FALSE, 
                              static_cast<GLsizei>(layout.size),
                              (void*)item.offset);  // NOLINT
        glEnableVertexAttribArray(attrib);
    }

    current_layout = &layout;
}
