#ifndef ASSET_H_
#define ASSET_H_

#include <filesystem>
#include <fstream>
#include <map>

namespace fs = std::filesystem;

enum class AssetKind { Shader, Texture };

static const std::map<AssetKind, fs::path> asset_path = {
    {AssetKind::Shader, "shader"}, {AssetKind::Texture, "texture"}};

static const fs::path base_path{"./asset"};

inline static fs::path get_asset_path(AssetKind kind,
                                      const std::string& asset_name) {
    return base_path / asset_path.at(kind) / asset_name;
}

inline static std::string get_asset(AssetKind kind, const std::string& asset_name) {
    fs::path asset_path = get_asset_path(kind, asset_name);

    std::ifstream f(asset_path);
    if (!f.is_open()) {
        throw std::runtime_error("could not open " + asset_path.string());
    }

    
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

#endif  // !DEBUG
