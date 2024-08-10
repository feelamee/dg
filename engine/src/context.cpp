#include <engine/context.hpp>
#include <engine/error.hpp>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

#include <glad/glad.h>

#include <format>

namespace dg
{

context_error::context_error(std::string const& msg)
    : std::runtime_error(msg)
{
}

context_error::context_error(char const* msg)
    : std::runtime_error(msg)
{
}

struct context::internal_data
{
    SDL_Window* sdl_window = nullptr;
    SDL_GLContext gl_context = nullptr;
};

void
context::internal_data_deleter::operator()(internal_data* data)
{
    if (data)
    {
        if (data->sdl_window)
        {
            SDL_DestroyWindow(static_cast<SDL_Window*>(data->sdl_window));
        }
        if (data->gl_context)
        {
            SDL_GL_DestroyContext(data->gl_context);
        }

        SDL_Quit();

        delete data;
    }
}

context const* context::ctx{ nullptr };

context::context(char const* const title, glm::u32vec2 size)
{
    if (ctx != nullptr)
    {
        throw context_error("context already created");
    }

    flag_t flags{ SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE };
    static_assert(std::is_same_v<decltype(flags), SDL_WindowFlags>);

    SDL_Window* sdl_window =
        SDL_CreateWindow(title, static_cast<int>(size.x), static_cast<int>(size.y), flags);
    if (nullptr == sdl_window)
    {
        throw context_error(std::format("internal window init failed with: {}", SDL_GetError()));
    }

    {
        GLint const desired_major_version{ 3 };
        GLint const desired_minor_version{ 2 };
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, desired_major_version);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, desired_minor_version);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

        GLint real_major_version{ 0 };
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &real_major_version);
        GLint real_minor_version{ 0 };
        SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &real_minor_version);

        SDL_assert(real_major_version >= desired_major_version && real_minor_version >= desired_minor_version);
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(sdl_window);
    if (nullptr == gl_context)
    {
        throw context_error(std::format("internal create OpenGL context failed with: {}", SDL_GetError()));
    }

    const auto load_gl_fn =
        +[](char const* fn) { return reinterpret_cast<void*>(SDL_GL_GetProcAddress(fn)); };
    int errc = gladLoadGLES2Loader(load_gl_fn);
    SDL_assert_release(0 != errc);

    {
        int w{ 0 }, h{ 0 };
        if (0 != SDL_GetWindowSize(sdl_window, &w, &h))
        {
            throw context_error(std::format("getting window size failed with: {}", SDL_GetError()));
        }
        GL_CHECK(glViewport(0, 0, w, h));
    }

    auto* const p = new internal_data{ .sdl_window = sdl_window, .gl_context = gl_context };
    data.reset(p);

    ctx = this;
}

context::context(context&& ctx)
    : data(std::move(ctx.data))
{
}

context&
context::operator=(context ctx)
{
    using std::swap;

    std::swap(data, ctx.data);

    return *this;
}

context::~context() { ctx = nullptr; }

void
context::swap_window()
{
    SDL_GL_SwapWindow(data->sdl_window);
}

void
context::clear_window(glm::vec4 color)
{
    GL_CHECK(glClearColor(color.r, color.g, color.b, color.a));
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
}

glm::u32vec2
context::window_size() const
{
    int w{ 0 }, h{ 0 };
    if (0 != SDL_GetWindowSize(data->sdl_window, &w, &h))
    {
        throw context_error(std::format("error getting window size: {}", SDL_GetError()));
    }

    return { w, h };
}

} // namespace dg
