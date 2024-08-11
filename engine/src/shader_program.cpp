#include <engine/bind_guard.hpp>
#include <engine/context.hpp>
#include <engine/error.hpp>
#include <engine/shader_program.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

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
    // clang-format off
    GLenum shader_type = type == shader_t::fragment ? GL_FRAGMENT_SHADER
                       : type == shader_t::vertex ? GL_VERTEX_SHADER
                       : 0;
    // clang-format on
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

std::any
shader_program::bind()
{
    GLint cur{ 0 };
    GL_CHECK(glGetIntegerv(GL_CURRENT_PROGRAM, &cur));
    GL_CHECK(glUseProgram(handle));

    return static_cast<handle_t>(cur);
}

void
shader_program::unbind(std::any data)
{
    GL_CHECK(glUseProgram(0));
    GL_CHECK(glUseProgram(std::any_cast<handle_t>(data)));
}

void
shader_program::uniform(uniform_location id, glm::vec3 const& vec)
{
    bind_guard _{ *this };

    GL_CHECK(glUniform3f(id, vec.x, vec.y, vec.z));
}

void
shader_program::uniform(uniform_location id, glm::vec4 const& vec)
{
    bind_guard _{ *this };

    GL_CHECK(glUniform4f(id, vec.x, vec.y, vec.z, vec.w));
}

void
shader_program::uniform(uniform_location id, glm::mat4 const& mat)
{
    bind_guard _{ *this };

    GL_CHECK(glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(mat)));
}

void
shader_program::uniform(uniform_location id, float v)
{
    bind_guard _{ *this };

    GL_CHECK(glUniform1f(id, v));
}

} // namespace dg
