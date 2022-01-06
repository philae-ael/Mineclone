#ifndef WORLD_H_
#define WORLD_H_

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "../utils/logging.h"
#include "../utils/mat.h"
#include "../utils/profiler.h"
#include "../utils/range_iterator.h"
#include "chunk_array.h"

template <typename T, int N, int M, int L>
using array3d = std::array<std::array<std::array<T, L>, M>, N>;

const int chunkWidth = 8;
const int chunkHeight = 256;

struct chunk_identifier {
    int x;
    int z;
};

using block_chunk_coord = std::array<std::size_t, 3>;

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
        auto [x, y, z] = coords;

        assert(x < chunkWidth && y < chunkHeight && z < chunkWidth);
        if (y > 2 + (unsigned int)std::abs(id.x) + (unsigned int)std::abs(id.z))
            return {BlockType::Empty};
        return {BlockType::Grass};
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

    void setBlock(int x, int y, int z, const BlockData& data) {
        chunk.setBlock(x, y, z, data);
    }

    [[nodiscard]] BlockData getBlock(block_chunk_coord coords) const {
        if (!details.on_boder[coords[0]][coords[1]][coords[2]])
            return {BlockType::Empty};

        return chunk.getBlock(coords);
    }

    [[nodiscard]] chunk_identifier getId() const { return chunk.getId(); }

    void simplify() {
        PROFILE_SCOPED();
        for (auto [x, y, z] : chunk_range_it{}) {
            block_chunk_coord coords{x, y, z};
            uint32_t group = details.get_group(coords);
            if (chunk.getBlock(coords).type == BlockType::Empty) continue;

            if (group == 0) group = details.attribute_new_group(coords);

            details.log << "Got group " << group << " " << coords;
            for (auto neighbour : details.get_neighbours(coords)) {
                if (neighbour == coords) continue;

                auto neighbour_group = details.get_group(neighbour);
                if (neighbour_group != 0 && neighbour_group != group) {
                    details.merge_groups(neighbour_group, group);
                    group = details.get_group(coords);
                } else if (chunk.getBlock(neighbour).type != BlockType::Empty) {
                    details.set_group(neighbour, group);
                }
            }
        }
        for (auto [x, y, z] : chunk_range_it{}) {
            block_chunk_coord coords{x, y, z};
            details.on_boder[x][y][z] = details.on_group_border(coords);
        }
    }

    // Note: this is const means that the chunk will not modified. That's all
    Chunk chunk;

   private:
    struct details_t {
        // 0 == no group
        array3d<uint32_t, chunkWidth, chunkHeight, chunkWidth> block_groups{};
        array3d<bool, chunkWidth, chunkHeight, chunkWidth> on_boder{};
        uint32_t get_group(block_chunk_coord c) const {
            return block_groups[c[0]][c[1]][c[2]];
        }
        void set_group(block_chunk_coord c, uint32_t group) {
            block_groups[c[0]][c[1]][c[2]] = group;
        }

        uint32_t attribute_new_group(block_chunk_coord coords) {
            set_group(coords, next_free_group);
            next_free_group += 1;
            return get_group(coords);
        }

        bool on_group_border(block_chunk_coord coords) const {
            for (auto neighbour : get_neighbours(coords)) {
                if (neighbour == coords) return true;  // Chunk border
                if (get_group(coords) != get_group(neighbour)) return true;
            }
            return false;
        }

        // returns all 6 neighbours of coords in chunk. If neighbours is out
        // of bounds, will return coords
        static std::array<block_chunk_coord, 6> get_neighbours(
            block_chunk_coord coords) {
            std::array neighbours{coords, coords, coords,
                                  coords, coords, coords};

            if (coords[0] + 1 < chunkWidth) neighbours[0][0] += 1;
            if (coords[1] + 1 < chunkHeight) neighbours[1][1] += 1;
            if (coords[2] + 1 < chunkWidth) neighbours[2][2] += 1;
            if (coords[0] > 0) neighbours[3][0] -= 1;
            if (coords[1] > 0) neighbours[4][1] -= 1;
            if (coords[2] > 0) neighbours[5][2] -= 1;

            return neighbours;
        }

        void merge_groups(uint32_t g1, uint32_t g2) {
            log << "Merging groups " << g1 << " and " << g2;
            for (auto [x, y, z] : chunk_range_it{}) {
                if (block_groups[x][y][z] == g2) block_groups[x][y][z] = g1;
            }
        }
        Logger log{Logger::get("ChunkSimplifyer")};

        uint32_t next_free_group = 1;
    };
    mutable details_t details{};
};

template <typename T>
T chunk_factory_generic(int x, int y) {
    return {chunk_identifier{x, y}};
}

struct World {
    using storage = chunk_array<ChunkSimplifyerProxy, chunk_factory_generic>;
    storage data;

    typename storage::iterator begin() { return data.begin(); }
    typename storage::iterator end() { return data.end(); }
};

#endif  // !WORLD_H_
