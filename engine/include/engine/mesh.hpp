#pragma once

#include <cstdint>
#include <vector>

namespace dg
{

struct mesh
{
public:
    [[nodiscard]] size_t vertices_bytelen() const;
    [[nodiscard]] size_t indices_bytelen() const;

    using coord_type = float;
    using index_type = uint32_t;

    std::vector<coord_type> vertices;
    std::vector<index_type> vertex_indices;
    std::vector<coord_type> normals;
    std::vector<index_type> normal_indices;
};

} // namespace dg
