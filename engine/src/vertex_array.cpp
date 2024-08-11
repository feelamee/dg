#include <engine/bind_guard.hpp>
#include <engine/error.hpp>
#include <engine/mesh.hpp>
#include <engine/vertex_array.hpp>

#include <glad/glad.h>

#include <cassert>

namespace dg
{

vertex_array::error::error(std::string const& msg)
    : std::runtime_error(msg)
{
}

vertex_array::error::error(char const* msg)
    : std::runtime_error(msg)
{
}

vertex_array::vertex_array(context const& /*ctx*/)
{
    GL_CHECK(glGenVertexArrays(1, &handle));
    if (handle == 0)
    {
        throw error("error occurs creating vertex_array");
    }
}

vertex_array::vertex_array(vertex_array&& other)
    : handle(other.handle)
{
}

vertex_array&
vertex_array::operator=(vertex_array other)
{
    using std::swap;

    swap(handle, other.handle);

    return *this;
}

vertex_array::~vertex_array() { GL_CHECK(glDeleteVertexArrays(1, &handle)); }

void
vertex_array::load(data_t type, mesh const& m)
{
    bind_guard _{ *this };

    // clang-format off
    GLenum const draw_type = type == data_t::immutable ? GL_STATIC_DRAW
                           : type == data_t::dynamic ? GL_DYNAMIC_DRAW
                           : type == data_t::stream  ? GL_STREAM_DRAW
                           : 0;
    // clang-format on
    assert(draw_type != 0);

    GLuint vbo{ 0 };
    GL_CHECK(glGenBuffers(1, &vbo));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, m.vertices_bytelen(), m.vertices.data(), draw_type));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr));

    GLuint vbe{ 0 };
    GL_CHECK(glGenBuffers(1, &vbe));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbe));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices_bytelen(), m.vertex_indices.data(), draw_type));

    GL_CHECK(glEnableVertexAttribArray(0));

    GL_CHECK(glBindVertexArray(0));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

std::any
vertex_array::bind()
{
    GLint cur{ 0 };
    GL_CHECK(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &cur));
    GL_CHECK(glBindVertexArray(handle));

    return static_cast<handle_t>(cur);
}

void
vertex_array::unbind(std::any data)
{
    GL_CHECK(glBindVertexArray(0));
    GL_CHECK(glBindVertexArray(std::any_cast<handle_t>(data)));
}

} // namespace dg
