#ifndef SHADER_LAYOUT_H_
#define SHADER_LAYOUT_H_

#include <glad/glad.h>

#include <cstddef>
#include <vector>

#include "../utils/logging.h"

enum class LayoutType { Float };

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
            return sizeof(float);
    }
    Logger::get() << LogLevel::Error << "Unknown LayoutType ?!";
    return GL_FLOAT;
}

inline int layoutTypeGL(LayoutType type) {
    switch (type) {
        case LayoutType::Float:
            return GL_FLOAT;
    }
    Logger::get() << LogLevel::Error << "Unknown LayoutType ?!";
    return GL_FLOAT;
}

struct Layout {
    Layout(std::vector<LayoutItem>&& layout_items)
        : items(layout_items) {
        
        size_t current_offset{};
        for (auto& item : items) {
            if (item.offset == 0)
                item.offset = current_offset;
            current_offset += item.length * layoutTypeSizeof(item.type);
        }

        size = current_offset;
    };

    std::vector<LayoutItem> items;
    std::size_t size;
};

#endif  // !SHADER_LAYOUT_H_
