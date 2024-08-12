#pragma once

#include <cstdint>
#include <vector>

namespace dg
{

struct mesh
{
public:
    using coord_type = float;
    using index_type = uint32_t;

    std::vector<coord_type> vertices;
    std::vector<index_type> indices;
    std::vector<coord_type> normals;
};

} // namespace dg
