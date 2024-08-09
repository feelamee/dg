#include <engine/context.hpp>
#include <engine/window.hpp>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_log.h>

#include <glad/glad.h>

#include <array>
#include <cstdlib>
#include <iostream>
#include <string_view>

#define GL_CHECK(expr)                                                                             \
    {                                                                                              \
        (expr);                                                                                    \
        orbi::gl_check(__FILE__, __LINE__, #expr);                                                 \
    }

#define LOG_DEBUG(...)                                                                             \
    {                                                                                              \
        /* fprintf(stderr, "[DEBUG: %s: %d]:\n    ", __FILE__,                                     \
        __LINE__); fprintf(stderr, __VA_ARGS__); */                                                \
        log_debug(__LINE__, __FILE__, __VA_ARGS__);                                                \
    }

namespace orbi
{
void
log_debug(int line, const char* fn, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, fmt, args);
    va_end(args);
}

int
gl_check(std::string const& file, unsigned int line, std::string_view const expression)
{
    GLenum error_code = glGetError();
    if (GL_NO_ERROR == error_code)
    {
        return EXIT_SUCCESS;
    }

    do
    {
        std::string error = "Unknown error";
        std::string description = "No description";

        // Decode the error code
        switch (error_code)
        {
        case GL_INVALID_ENUM: {
            error = "GL_INVALID_ENUM";
            description = "An unacceptable value has been specified for an "
                          "enumerated argument.";
            break;
        }

        case GL_INVALID_VALUE: {
            error = "GL_INVALID_VALUE";
            description = "A numeric argument is out of range.";
            break;
        }

        case GL_INVALID_OPERATION: {
            error = "GL_INVALID_OPERATION";
            description = "The specified operation is not allowed in the "
                          "current state.";
            break;
        }

        case GL_OUT_OF_MEMORY: {
            error = "GL_OUT_OF_MEMORY";
            description = "There is not enough memory left to execute the command.";
            break;
        }

        case GL_INVALID_FRAMEBUFFER_OPERATION: {
            error = "GL_INVALID_FRAMEBUFFER_OPERATION";
            description = "The object bound to FRAMEBUFFER_BINDING is not "
                          "\"framebuffer complete\".";
            break;
        }
        }

        LOG_DEBUG("[ %s : %d ]:"
                  "\n    An internal OpenGL call failed"
                  "\n    Expression:\n        %s"
                  "\n    Error description:\n        %s\n",
                  file.c_str(), line, expression.data(), description.c_str());

        error_code = glGetError();
    } while (false); // GL_NO_ERROR != error_code);

    return EXIT_FAILURE;
}

enum error_t : int
{
    NO_ERROR = 0,

    ERROR_COMPILE_SHADER = -1,
};

struct make_shader_result
{
    GLuint id;

    int error;
};

make_shader_result
make_shader(GLenum type, std::string_view const src)
{
    make_shader_result res{};
    res.id = glCreateShader(type);
    if (res.id == 0)
    {
        res.error = glGetError();
        return res;
    }

    char const* const cstr = src.data();
    int const size{ static_cast<int>(src.size()) };
    GL_CHECK(glShaderSource(res.id, 1, &cstr, &size));

    GL_CHECK(glCompileShader(res.id));

    GLint is_success{ GL_TRUE };
    GL_CHECK(glGetShaderiv(res.id, GL_COMPILE_STATUS, &is_success));
    if (is_success != GL_TRUE)
    {
        GLsizei log_len{ 0 };
        std::array<GLchar, 1024> log{ '\0' };
        GL_CHECK(glGetShaderInfoLog(res.id, 1024, &log_len, log.data()));
        std::cerr << __PRETTY_FUNCTION__ << ":\n";
        std::cerr << log.data() << std::endl;
        res.error = error_t::ERROR_COMPILE_SHADER;
        return res;
    }

    return res;
}

constexpr std::string_view vertex_shader_src = R"(
#version 320 es

layout (location = 0) in vec3 position;

out vec4 vertex_color;

void main()
{
    gl_Position = vec4(position, 1.0f);
    vertex_color = vec4(.8f, .0f, .0f, 1.0f);
}
)";

constexpr std::string_view fragment_shader_src = R"(
#version 320 es
precision mediump float;

in vec4 vertex_color;

out vec4 color;

void main()
{
    color = vertex_color;
}
)";

} // namespace orbi

int
main()
{
    using namespace dg;
    context ctx(context::flag::everything);
    window win(ctx, "window", { 960, 590 }, window::flag::resizeable);

    auto const [vertex_shader, errc1] = orbi::make_shader(GL_VERTEX_SHADER, orbi::vertex_shader_src);
    assert(errc1 == orbi::error_t::NO_ERROR);
    auto const [fragment_shader, errc2] = orbi::make_shader(GL_FRAGMENT_SHADER, orbi::fragment_shader_src);
    assert(errc2 == orbi::error_t::NO_ERROR);

    GLuint const program_id = glCreateProgram();
    assert(program_id != 0);

    GL_CHECK(glAttachShader(program_id, vertex_shader));
    GL_CHECK(glAttachShader(program_id, fragment_shader));

    GL_CHECK(glLinkProgram(program_id));
    {
        GLint is_success{ false };
        GL_CHECK(glGetProgramiv(program_id, GL_LINK_STATUS, &is_success));
        assert(is_success);
    }

    GL_CHECK(glDeleteShader(vertex_shader));
    GL_CHECK(glDeleteShader(fragment_shader));

    // clang-format off
    std::array<GLfloat, 12> vertices {
        0.0f,  0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };

    std::array<GLuint, 6> indices {
        0, 1, 2,
        0, 2, 3,
    };
    // clang-format on

    GLuint vao{ 0 };
    GL_CHECK(glGenVertexArrays(1, &vao));
    GL_CHECK(glBindVertexArray(vao));

    GLuint vbo{ 0 };
    GL_CHECK(glGenBuffers(1, &vbo));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0));

    GLuint vbe{ 0 };
    GL_CHECK(glGenBuffers(1, &vbe));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbe));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW));

    GL_CHECK(glEnableVertexAttribArray(0));

    GL_CHECK(glBindVertexArray(0));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    while (true)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_EVENT_QUIT)
            {
                return EXIT_SUCCESS;
            }
        }

        win.clear_with(0.2, 0.5, 1, 1);

        GL_CHECK(glUseProgram(program_id));
        GL_CHECK(glBindVertexArray(vao));

        GL_CHECK(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr));

        GL_CHECK(glBindVertexArray(0));
        GL_CHECK(glUseProgram(0));

        win.swap();
    }

    GL_CHECK(glDeleteVertexArrays(1, &vao));
    GL_CHECK(glDeleteBuffers(1, &vbo));
    GL_CHECK(glDeleteBuffers(1, &vbe));

    return EXIT_SUCCESS;
}
