#pragma once

#include <filesystem>
#include <optional>

namespace dg
{

enum class model_t
{
    obj,
    gltf,
};

struct mesh;

std::optional<mesh> load(model_t type, std::filesystem::path const& filename);

} // namespace dg
