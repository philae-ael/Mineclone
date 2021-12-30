#include "world_renderer.h"

#include <glad/glad.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <optional>

#define STB_IMAGE_IMPLEMENTATION
#include "../../stb_image.h"

#include "../asset.h"
#include "../component/shader.h"
#include "../utils/logging.h"
#include "../utils/mat.h"
#include "../utils/mat_opengl.h"

enum FaceKind { Top = 1, Bottom = 2, Side = 0 };

struct Vertex {
    math::vec3f position;
    math::vec2f textpos;
    float face;
};

std::array vertices{
    Vertex{{1, 1, 1}, {1, 0}, FaceKind::Side},
    Vertex{{-1, 1, 1}, {0, 0}, FaceKind::Side},
    Vertex{{-1, -1, 1}, {0, 1}, FaceKind::Side},

    Vertex{{1, 1, 1}, {1, 0}, FaceKind::Side},
    Vertex{{-1, -1, 1}, {0, 1}, FaceKind::Side},
    Vertex{{1, -1, 1}, {1, 1}, FaceKind::Side},

    Vertex{{1, 1, 1}, {0, 0}, FaceKind::Side},
    Vertex{{1, -1, 1}, {0, 1}, FaceKind::Side},
    Vertex{{1, -1, -1}, {1, 1}, FaceKind::Side},

    Vertex{{1, 1, 1}, {0, 0}, FaceKind::Side},
    Vertex{{1, -1, -1}, {1, 1}, FaceKind::Side},
    Vertex{{1, 1, -1}, {1, 0}, FaceKind::Side},

    Vertex{{1, 1, 1}, {1, 1}, FaceKind::Top},
    Vertex{{1, 1, -1}, {1, 0}, FaceKind::Top},
    Vertex{{-1, 1, -1}, {0, 0}, FaceKind::Top},

    Vertex{{1, 1, 1}, {1, 1}, FaceKind::Top},
    Vertex{{-1, 1, -1}, {0, 0}, FaceKind::Top},
    Vertex{{-1, 1, 1}, {0, 1}, FaceKind::Top},

    Vertex{{-1, -1, -1}, {1, 1}, FaceKind::Side},
    Vertex{{1, -1, -1}, {0, 1}, FaceKind::Side},
    Vertex{{1, 1, -1}, {0, 0}, FaceKind::Side},

    Vertex{{-1, -1, -1}, {1, 1}, FaceKind::Side},
    Vertex{{1, 1, -1}, {0, 0}, FaceKind::Side},
    Vertex{{-1, 1, -1}, {1, 0}, FaceKind::Side},

    Vertex{{-1, -1, -1}, {0, 1}, FaceKind::Side},
    Vertex{{-1, 1, -1}, {0, 0}, FaceKind::Side},
    Vertex{{-1, 1, 1}, {1, 0}, FaceKind::Side},

    Vertex{{-1, -1, -1}, {0, 1}, FaceKind::Side},
    Vertex{{-1, 1, 1}, {1, 0}, FaceKind::Side},
    Vertex{{-1, -1, 1}, {1, 1}, FaceKind::Side},

    Vertex{{-1, -1, -1}, {1, 1}, FaceKind::Bottom},
    Vertex{{-1, -1, 1}, {1, 0}, FaceKind::Bottom},
    Vertex{{1, -1, 1}, {0, 0}, FaceKind::Bottom},

    Vertex{{-1, -1, -1}, {1, 1}, FaceKind::Bottom},
    Vertex{{1, -1, 1}, {0, 0}, FaceKind::Bottom},
    Vertex{{1, -1, -1}, {0, 1}, FaceKind::Bottom},
};

WorldRenderer::WorldRenderer(Camera* camera) : camera(camera) {
    std::string vertex_source = get_asset(AssetKind::Shader, "base.vert");
    std::string fragment_source = get_asset(AssetKind::Shader, "base.frag");

    shader.emplace(vertex_source, fragment_source);
    auto with_shader = shader->use();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
                 GL_STATIC_DRAW);

    GLint posAttrib = shader->getAttribLocation("in_position");
    GLint texAttrib = shader->getAttribLocation("in_texposition");
    GLint faceAttrib = shader->getAttribLocation("in_facekind");

    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));  // NOLINT

    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, textpos));  // NOLINT
    glVertexAttribPointer(faceAttrib, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, face));  // NOLINT

    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(texAttrib);
    glEnableVertexAttribArray(faceAttrib);

    std::string path = get_asset_path(AssetKind::Texture, "minecraft.png");
    int x, y, n;
    unsigned char* data = stbi_load(path.c_str(), &x, &y, &n, 0);

    const int tiles = 16;
    int subWidth = x / tiles;
    int subHeight = y / tiles;


    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_SRGB8_ALPHA8, subWidth, subHeight, tiles*tiles, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, x);
    for(int i = 0; i< tiles ;i++)
        for(int j = 0; j< tiles ;j++)
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, tiles*j + i, subWidth, subHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE, data + j*x*n + i*subWidth*n);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    stbi_image_free(data);

    GLint textidUnif = shader->getUniformLocation("textid");
    glUniform1i(textidUnif, 3);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);
}

void renderOneBlock(const Camera& camera, const Shader& shader, int x, int y,
                    int z, GLuint VAO) {
    glEnableVertexAttribArray(0);
    glBindVertexArray(VAO);
    auto with_shader = shader.use();

    const auto scale = math::scale<float>(0.5);             // NOLINT
    const auto transl = math::translation<float>(x, y, z);  // NOLINT
    auto model_trans = transl * scale;
    auto world_trans = math::translation<float>(0, 0, 0);
    auto proj = camera.getCameraMatrix();

    GLint modelUnif = shader.getUniformLocation("model");
    GLint worldUnif = shader.getUniformLocation("world");
    GLint projUnif = shader.getUniformLocation("proj");
    glUniformMatrix4fv(modelUnif, 1, GL_TRUE, model_trans.ptr());
    glUniformMatrix4fv(worldUnif, 1, GL_TRUE, world_trans.ptr());
    glUniformMatrix4fv(projUnif, 1, GL_TRUE, proj.ptr());

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glDisableVertexAttribArray(0);
}

void WorldRenderer::render() {
    camera->position = {4, 5, 4};
    camera->lookAt({0, 0, 0});

    camera->persp = math::projection<float>(1.333, M_PI / 2, 1, 30);
    for (auto&& chunk : world.data) {
        // TODO write an iterator for chunk
        for (int x = 0; x < chunkWidth; x++) {
            for (int z = 0; z < chunkWidth; z++) {
                for (int y = 0; y < chunkHeight; y++) {
                    auto block = chunk.getBlock(x, y, z);

                    if (block.type == BlockType::Empty) continue;

                    renderOneBlock(*camera, *shader,
                                   chunkWidth * chunk.getId().x + x, y,
                                   chunkWidth * chunk.getId().z + z, VAO);
                }
            }
        }
    }
}
