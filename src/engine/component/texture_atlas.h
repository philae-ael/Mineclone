#ifndef TEXTURE_ATLAS_H_
#define TEXTURE_ATLAS_H_

#include <glad/glad.h>
#include <stb_image.h>

#include <cstddef>
#include <filesystem>

namespace fs = std::filesystem;

// TODO: throw(?) if we cant load the texure
class TextureAtlas {
   public:
    TextureAtlas(const fs::path& path, std::size_t horizontal_tiles,
                 std::size_t vertical_tiles) {
        int x, y, n;
        unsigned char* data = stbi_load(path.c_str(), &x, &y, &n, 0);

        std::size_t subWidth = x / horizontal_tiles;
        std::size_t subHeight = y / vertical_tiles;

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_SRGB8_ALPHA8,
                     static_cast<GLsizei>(subWidth),
                     static_cast<GLsizei>(subHeight),
                     static_cast<GLsizei>(horizontal_tiles * vertical_tiles), 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glPixelStorei(GL_UNPACK_ROW_LENGTH, x);
        for (std::size_t i = 0; i < vertical_tiles; i++)
            for (std::size_t j = 0; j < horizontal_tiles; j++)
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0,
                                static_cast<GLsizei>(vertical_tiles * j + i),
                                static_cast<GLsizei>(subWidth),
                                static_cast<GLsizei>(subHeight), 1, GL_RGBA,
                                GL_UNSIGNED_BYTE,
                                (void*)(data + j * x + i * subWidth * n));

        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S,
                        GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T,
                        GL_CLAMP_TO_EDGE);
        stbi_image_free(data);
    }

    TextureAtlas(TextureAtlas&& other) noexcept : texture(other.texture) {
        other.texture = 0;
    }

    void bind() const { glBindTexture(GL_TEXTURE_2D_ARRAY, texture); }

    ~TextureAtlas() {
        if (texture) glDeleteTextures(1, &texture);
    }

   private:
    GLuint texture = 0;
};

#endif  // !TEXTURE_ATLAS_H_
