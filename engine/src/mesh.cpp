#include <engine/mesh.hpp>

namespace dg
{

size_t
mesh::vertices_bytelen() const
{
    return vertices.size() * sizeof(coord_type);
}

size_t
mesh::vertex_indices_bytelen() const
{
    return vertex_indices.size() * sizeof(index_type);
}

size_t
mesh::normals_bytelen() const
{
    return normals.size() * sizeof(coord_type);
}

size_t
mesh::normal_indices_bytelen() const
{
    return normal_indices.size() * sizeof(index_type);
}

} // namespace dg
