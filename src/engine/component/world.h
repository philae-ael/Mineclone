#ifndef WORLD_H_
#define WORLD_H_

#include <array>

#include "chunk_array.h"

template <typename T, int N, int M, int L>
using array3d = std::array<std::array<std::array<T, L>, M>, N>;

const int chunkWidth = 32;
const int chunkHeight = 256;

struct chunk_identifier {
    int x;
    int y;
};

enum class BlockType { Empty, Grass };

struct BlockData {
    BlockType type = BlockType::Empty;
};

struct Chunk {
    // TODO: makes sure copy ctor and move ctor deals with deleting old context
   public:
    Chunk(int x, int y) : id{x, y} {}

    [[nodiscard]] chunk_identifier getId() const { return id; }
    [[nodiscard]] BlockData getBlock(int /*x*/, int /*y*/, int z) const {
        if (z > id.x) return {BlockType::Empty};
        return {BlockType::Grass};
    }

   private:
    chunk_identifier id;
};


struct World {
    chunk_array<Chunk> data;
};

#endif  // !WORLD_H_
