#ifndef WORLD_H_
#define WORLD_H_

#include <array>
#include <cassert>

#include "chunk_array.h"

template <typename T, int N, int M, int L>
using array3d = std::array<std::array<std::array<T, L>, M>, N>;

const int chunkWidth = 8;
const int chunkHeight = 256;

struct chunk_identifier {
    int x;
    int z;
};

enum class BlockType { Empty, Grass };

struct BlockData {
    BlockType type = BlockType::Empty;
};

struct Chunk {
    // TODO: makes sure copy ctor and move ctor deals with deleting old context
   public:
    Chunk(int x, int z) : id{x, z} {}

    [[nodiscard]] chunk_identifier getId() const { return id; }
    [[nodiscard]] BlockData getBlock(int x, int y, int z) const {
        assert(x < chunkWidth && y < chunkHeight && z < chunkWidth);
        if (y > std::abs(id.x) + std::abs(id.z)) return {BlockType::Empty};
        return {BlockType::Grass};
    }

   private:
    chunk_identifier id;
};


struct World {
    chunk_array<Chunk> data;

    typename chunk_array<Chunk>::iterator begin(){
        return data.begin();
    }
    typename chunk_array<Chunk>::iterator end(){
        return data.end();
    }
};

#endif  // !WORLD_H_
