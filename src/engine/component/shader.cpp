#include "shader.h"

#include <glad/glad.h>

#include <stdexcept>

#include "../data/shader_layout.h"

GLuint Shader::current_shader = 0;

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
            log << LogLevel::Error << "Fatal error in vertex shader";
            throw std::runtime_error(errorCompileStr(vertex_shader));
            break;
        case SCE_FRAGMENT_SHADER:
            glDeleteShader(vertex_shader);
            log << LogLevel::Error << "Fatal error in fragment shader";
            throw std::runtime_error(errorCompileStr(fragment_shader));
            break;
        case SCE_PROGRAM_SHADER: {
            // cleanup
            glDeleteShader(vertex_shader);
            glDeleteShader(fragment_shader);

            log << LogLevel::Error << "Fatal error in shader linking";
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
    if (shader_program) {
        if (vertex_shader) glDetachShader(shader_program, vertex_shader);
        if (fragment_shader) glDetachShader(shader_program, fragment_shader);

        glDeleteProgram(shader_program);
    }
    if (vertex_shader) glDeleteShader(vertex_shader);
    if (fragment_shader) glDeleteShader(fragment_shader);
}

void Shader::useLayout(const Layout& layout) {
    auto self = use();

    for (const auto& item : layout.items) {
        GLint attrib = getAttribLocation(item.attibute_name);

        log << LogLevel::Info << "Using layout item with"
            << " attribute name: " << item.attibute_name
            << " length: " << item.length << " offset: " << item.offset
            << " type_id: " << item.type;
        auto type = layoutTypeGL(item.type);
        switch (type) {
            case GL_BYTE:
            case GL_SHORT:
            case GL_INT:
            case GL_UNSIGNED_BYTE:
            case GL_UNSIGNED_SHORT:
            case GL_UNSIGNED_INT:
                glVertexAttribIPointer(attrib, static_cast<GLint>(item.length),
                                       type, static_cast<GLsizei>(layout.size),
                                       (void*)item.offset);  // NOLINT
                break;
            default:
                glVertexAttribPointer(attrib, static_cast<GLint>(item.length),
                                      layoutTypeGL(item.type), GL_FALSE,
                                      static_cast<GLsizei>(layout.size),
                                      (void*)item.offset);  // NOLINT
        }
        glEnableVertexAttribArray(attrib);
    }

    current_layout = &layout;
}
