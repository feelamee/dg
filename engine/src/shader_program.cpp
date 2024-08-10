#include <engine/context.hpp>
#include <engine/error.hpp>
#include <engine/shader_program.hpp>

#include <glad/glad.h>

#include <GLES2/gl2.h>
#include <format>

namespace dg
{

shader_program::error::error(std::string const& msg)
    : std::runtime_error(msg)
{
}

shader_program::error::error(char const* msg)
    : std::runtime_error(msg)
{
}

shader_program::shader_program(context const& /* ctx */)
    : handle(glCreateProgram())
{
    if (handle == 0)
    {
        throw error("error occurs creating shader_program");
    }
}

shader_program::shader_program(shader_program&& other)
    : handle(other.handle)
{
}

shader_program&
shader_program::operator=(shader_program other)
{
    using std::swap;

    swap(handle, other.handle);

    return *this;
}

shader_program::~shader_program() { GL_CHECK(glDeleteProgram(handle)); }

void
shader_program::attach_from_src(shader_t type, std::string_view src)
{
    GLenum shader_type = type == shader_t::fragment ? GL_FRAGMENT_SHADER
                         : type == shader_t::vertex ? GL_VERTEX_SHADER
                                                    : 0;
    assert(shader_type != 0);

    GLuint id = glCreateShader(shader_type);
    if (handle == 0)
    {
        throw error(std::format("error occurs creating shader: {}", glGetError()));
    }

    char const* const cstr = src.data();
    int const size{ static_cast<int>(src.size()) };
    GL_CHECK(glShaderSource(id, 1, &cstr, &size));

    GL_CHECK(glCompileShader(id));

    GLint is_success{ GL_FALSE };
    GL_CHECK(glGetShaderiv(id, GL_COMPILE_STATUS, &is_success));
    if (is_success != GL_TRUE)
    {
        GLsizei log_len{ 0 };
        std::array<GLchar, 1024> log{ '\0' };
        GL_CHECK(glGetShaderInfoLog(id, 1024, &log_len, log.data()));

        GL_CHECK(glDeleteShader(id));

        throw error(std::format("compile shader failed with: {}", log.data()));
    }

    GL_CHECK(glAttachShader(handle, id));

    GL_CHECK(glDeleteShader(id));
}

bool
shader_program::link()
{
    GL_CHECK(glLinkProgram(handle));

    GLint is_success{ false };
    GL_CHECK(glGetProgramiv(handle, GL_LINK_STATUS, &is_success));

    return is_success;
}

void
shader_program::use(bool is_use)
{
    if (is_use)
    {
        GL_CHECK(glUseProgram(handle));
    } else
    {
        GL_CHECK(glUseProgram(0));
    }
}

} // namespace dg
