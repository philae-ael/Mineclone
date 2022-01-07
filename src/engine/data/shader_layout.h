#ifndef SHADER_LAYOUT_H_
#define SHADER_LAYOUT_H_

#include <glad/glad.h>

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <vector>

#include "../utils/logging.h"

enum class LayoutType { Int, Uint, Float, Ushort };

inline const char* layout_as_string(const LayoutType& l) {
    switch (l) {
        case LayoutType::Uint:
            return "unsigned int";
        case LayoutType::Ushort:
            return "unsigned short";
        case LayoutType::Int:
            return "int";
        default:
        case LayoutType::Float:
            return "float";
    }
}

inline std::ostream& operator<<(std::ostream& s, const LayoutType& l) {
    s << layout_as_string(l);
    return s;
}

struct LayoutItem {
    const char* attibute_name;
    const LayoutType type;
    const std::size_t length;

    // if 0, layout is considered tight
    // else use offesetof(VertexType, vertex_attribute)
    std::size_t offset = 0;
};

inline std::size_t layoutTypeSizeof(LayoutType type) {
    switch (type) {
        case LayoutType::Float:
            return sizeof(GLfloat);
        case LayoutType::Uint:
            return sizeof(GLuint);
        case LayoutType::Ushort:
            return sizeof(GLushort);
        case LayoutType::Int:
            return sizeof(int32_t);
    }
    Logger::get() << LogLevel::Error << "Unknown LayoutType ?!";
    return GL_FLOAT;
}

inline int layoutTypeGL(LayoutType type) {
    switch (type) {
        case LayoutType::Float:
            return GL_FLOAT;
        case LayoutType::Uint:
            return GL_UNSIGNED_INT;
        case LayoutType::Ushort:
            return GL_UNSIGNED_SHORT;
        case LayoutType::Int:
            return GL_INT;
    }
    Logger::get() << LogLevel::Error << "Unknown LayoutType ?!";
    return GL_FLOAT;
}

struct Layout {
    Layout(std::vector<LayoutItem>&& layout_items) : items(layout_items) {
        size_t current_offset{};
        for (auto& item : items) {
            if (item.offset == 0) item.offset = current_offset;
            current_offset += item.length * layoutTypeSizeof(item.type);
        }

        size = current_offset;
    };

    std::vector<LayoutItem> items;
    std::size_t size;
};

#endif  // !SHADER_LAYOUT_H_
