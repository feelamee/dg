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

    using vertex_type = float;
    using index_type = uint32_t;

    std::vector<vertex_type> vertices;
    std::vector<index_type> indices;
};

} // namespace dg
