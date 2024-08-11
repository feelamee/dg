#include <engine/mesh.hpp>

namespace dg
{

size_t
mesh::vertices_bytelen() const
{
    return vertices.size() * sizeof(coord_type);
}

size_t
mesh::indices_bytelen() const
{
    return vertex_indices.size() * sizeof(index_type);
}

} // namespace dg
