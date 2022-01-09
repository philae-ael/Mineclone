#ifndef ASSET_H_
#define ASSET_H_

#include <filesystem>
#include <fstream>
#include <map>

#include "../component/shader.h"
#include "../component/texture_atlas.h"

namespace fs = std::filesystem;

namespace asset_details {
inline std::string load_file(const fs::path& asset_path) {
    std::ifstream f(asset_path);
    if (!f.is_open()) {
        throw std::runtime_error("could not open " + asset_path.string());
    }

    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}
};  // namespace asset_details

enum class AssetKind { Shader, TextureAtlas };

static const std::map<AssetKind, fs::path> asset_path = {
    {AssetKind::Shader, "shader"}, {AssetKind::TextureAtlas, "texture"}};

static const fs::path base_path{"./asset"};

inline fs::path get_asset_path(AssetKind kind, const std::string& asset_name) {
    return base_path / asset_path.at(kind) / asset_name;
}

template <AssetKind kind>
struct get_asset_helper {};

template <AssetKind kind>
constexpr auto get_asset = &get_asset_helper<kind>::get_asset;

template <>
struct get_asset_helper<AssetKind::TextureAtlas> {
    static TextureAtlas get_asset(const std::string& asset_name,
                                  std::size_t horizontal_tiles,
                                  std::size_t vertical_tiles) {
        return {get_asset_path(AssetKind::TextureAtlas, asset_name),
                horizontal_tiles, vertical_tiles};
    }
};

template <>
struct get_asset_helper<AssetKind::Shader> {
    static Shader get_asset(const std::string& asset_name) {
        fs::path base_asset_path =
            get_asset_path(AssetKind::Shader, asset_name);

        fs::path vertex_shader_path = base_asset_path;
        vertex_shader_path += ".vert";

        fs::path fragment_shader_path = base_asset_path;
        fragment_shader_path += ".frag";

        return {asset_details::load_file(vertex_shader_path),
                asset_details::load_file(fragment_shader_path)};
    }
};

#endif  // !DEBUG
