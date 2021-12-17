#ifndef UTILS_H_
#define UTILS_H_

#include <filesystem>
#include <map>

#include "../asset.h"

namespace fs = std::filesystem;

static const std::map<AssetKind, fs::path> asset_path = {
    {AssetKind::AK_Shader, "shader"},
    {}
};

static const fs::path base_path {"./asset"};

inline static fs::path get_asset_path(AssetKind kind,
                                            const std::string& asset_name) {
    return base_path / asset_path.at(kind) / asset_name;
}

#endif  // !UTILS_H_
