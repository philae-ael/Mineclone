#ifndef CHUNK_H_
#define CHUNK_H_

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "../data_structure/chunk_array.h"
#include "../utils/logging.h"
#include "../utils/mat.h"
#include "../utils/profiler.h"
#include "../utils/range_iterator.h"
#include "mesh.h"

template <typename T, int N, int M, int L>
using array3d = std::array<std::array<std::array<T, L>, M>, N>;

const int chunkWidth = 8;
const int chunkHeight = 256;

struct chunk_identifier {
    int x;
    int z;
};

using block_chunk_coord = math::vec<std::size_t, 3>;

enum class BlockType { Empty, Grass };

struct BlockData {
    BlockType type = BlockType::Empty;
};

using chunk_range_it =
    range_it<std::size_t, chunkWidth, chunkHeight, chunkWidth>;
struct Chunk {
    // TODO: makes sure copy ctor and move ctor deals with deleting old context
   public:
    Chunk(chunk_identifier id) : id{id} {}

    [[nodiscard]] chunk_identifier getId() const { return id; }
    [[nodiscard]] BlockData getBlock(block_chunk_coord coords) const {
        auto [x, y, z] = coords.data;

        assert(x < chunkWidth && y < chunkHeight && z < chunkWidth);
        if (y > 2 + (unsigned int)std::abs(id.x) + (unsigned int)std::abs(id.z))
            return {BlockType::Empty};
        return {BlockType::Grass};
    }

    [[nodiscard]] BlockTexture getBlockTexture(block_chunk_coord coords,
                                               FaceKind kind) const {
        switch (getBlock(coords).type) {
            case BlockType::Grass:
                switch (kind) {
                    case FaceKind::Front:
                    case FaceKind::Back:
                    case FaceKind::Left:
                    case FaceKind::Right:
                        if (coords[1] < chunkHeight - 1 &&
                            getBlock(coords + block_chunk_coord{0, 1, 0})
                                    .type != BlockType::Empty)
                            return BlockTexture::GrassBottom;
                        return BlockTexture::GrassSide;
                    case FaceKind::Top: {
                        if (coords[1] < chunkHeight - 1 &&
                            getBlock(coords + block_chunk_coord{0, 1, 0})
                                    .type != BlockType::Empty)
                            return BlockTexture::GrassBottom;
                        return BlockTexture::GrassTop;
                    }
                    case FaceKind::Bottom:
                    default:
                        return BlockTexture::GrassBottom;
                }
            case BlockType::Empty:
            default:
                Logger::get() << "Should not be here...";
                return BlockTexture::GrassBottom;
        }
    }

    void setBlock(int x, int y, int z, BlockData data) {
        (void)this, (void)x, (void)y, (void)z, (void)data;
        throw std::runtime_error{"not implemented yet"};
    }

   private:
    chunk_identifier id;
};

class ChunkSimplifyerProxy {
   public:
    ChunkSimplifyerProxy(chunk_identifier id) : chunk(id) { simplify(); }
    ChunkSimplifyerProxy(ChunkSimplifyerProxy&&) noexcept = default;
    ChunkSimplifyerProxy& operator=(ChunkSimplifyerProxy&& other) = default;

    void setBlock(int x, int y, int z, const BlockData& data) {
        chunk.setBlock(x, y, z, data);
    }

    [[nodiscard]] BlockData getBlock(block_chunk_coord coords) const {
        return chunk.getBlock(coords);
    }

    [[nodiscard]] chunk_identifier getId() const { return chunk.getId(); }

    void simplify() {
        PROFILE_SCOPED();
        auto conditionalAddBlockFace = [&](block_chunk_coord coords,
                                           block_chunk_coord neighbour,
                                           FaceKind kind) {
            if (neighbour != coords &&  // chunk borders
                chunk.getBlock(neighbour).type != BlockType::Empty)
                return false;

            mesh.addBlockFace(coords, kind,
                              chunk.getBlockTexture(coords, kind));
            return true;
        };

        for (auto [x, y, z] : chunk_range_it{}) {
            block_chunk_coord coords{x, y, z};
            bool visible = false;
            if (chunk.getBlock(coords).type == BlockType::Empty) continue;
            auto neighbours = get_neighbours(coords);
            visible |=
                conditionalAddBlockFace(coords, neighbours[0], FaceKind::Right);
            visible |=
                conditionalAddBlockFace(coords, neighbours[1], FaceKind::Top);
            visible |=
                conditionalAddBlockFace(coords, neighbours[2], FaceKind::Back);
            visible |=
                conditionalAddBlockFace(coords, neighbours[3], FaceKind::Left);
            visible |= conditionalAddBlockFace(coords, neighbours[4],
                                               FaceKind::Bottom);
            visible |=
                conditionalAddBlockFace(coords, neighbours[5], FaceKind::Front);

            if (visible) log << "Block " << coords << " is visible";
        }
    }

    Chunk chunk;

    using mesh_type = MeshChunk;
    mesh_type mesh;

   private:
    // returns all 6 neighbours of coords in chunk. If neighbours is out
    // of bounds, will return coords
    // Right
    // Top
    // Back
    // Left
    // Bottom
    // Front
    static std::array<block_chunk_coord, 6> get_neighbours(
        block_chunk_coord coords) {
        std::array neighbours{coords, coords, coords, coords, coords, coords};

        if (coords[0] + 1 < chunkWidth) neighbours[0][0] += 1;
        if (coords[1] + 1 < chunkHeight) neighbours[1][1] += 1;
        if (coords[2] + 1 < chunkWidth) neighbours[2][2] += 1;
        if (coords[0] > 0) neighbours[3][0] -= 1;
        if (coords[1] > 0) neighbours[4][1] -= 1;
        if (coords[2] > 0) neighbours[5][2] -= 1;

        return neighbours;
    }

    Logger log{Logger::get({"ChunkSimplifyer"})};
};

#endif  // !CHUNK_H_
