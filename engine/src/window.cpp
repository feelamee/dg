#include <engine/context.hpp>
#include <engine/util.hpp>
#include <engine/window.hpp>

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_video.h>

#include <glad/glad.h>

#include <format>
#include <memory>

namespace dg
{

window_error::window_error(std::string const& msg)
    : std::runtime_error(msg)
{
}

window_error::window_error(char const* msg)
    : std::runtime_error(msg)
{
}

window::flag
operator|(window::flag l, window::flag r)
{
    return static_cast<window::flag>(to_underlying(l) | to_underlying(r));
}

window::flag&
operator|=(window::flag& l, window::flag r)
{
    return l = l | r;
}

window::flag
operator&(window::flag l, window::flag r)
{
    return static_cast<window::flag>(to_underlying(l) & to_underlying(r));
}

struct window::internal_data
{
    SDL_Window* sdl_window = nullptr;
    SDL_GLContext gl_context = nullptr;
};

void
window::internal_data_deleter::operator()(internal_data* data)
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

        delete data;
    }
}

window::window(context const& /*cxt*/, char const* const title, glm::u32vec2 size, window::flag flags)
{
    int internal_flags{ SDL_WINDOW_OPENGL };

    SDL_Window* sdl_window =
        SDL_CreateWindow(title, static_cast<int>(size.x), static_cast<int>(size.y), internal_flags);
    if (nullptr == sdl_window)
    {
        throw window_error(std::format("internal window init failed with: {}", SDL_GetError()));
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
        throw window_error(std::format("internal create OpenGL context failed with: {}", SDL_GetError()));
    }

    const auto load_gl_fn =
        +[](char const* fn) { return reinterpret_cast<void*>(SDL_GL_GetProcAddress(fn)); };
    int errc = gladLoadGLES2Loader(load_gl_fn);
    SDL_assert_release(0 != errc);

    {
        int x{ 0 }, y{ 0 }, w{ 0 }, h{ 0 };
        SDL_GetWindowPosition(sdl_window, &x, &y);
        SDL_GetWindowSize(sdl_window, &w, &h);
        glViewport(x, y, w, h);
    }

    auto* const p = new internal_data{ .sdl_window = sdl_window, .gl_context = gl_context };
    data.reset(p);
}

window::window(window&& /*w*/) { unimplemented(); }

window&
window::operator=(window w)
{
    using std::swap;

    swap(data, w.data);

    return *this;
}

void
window::swap() const
{
    SDL_GL_SwapWindow(data->sdl_window);
}

void
window::clear_with(float r, float g, float b, float a)
{
    r = std::clamp(r, 0.f, 1.f);
    g = std::clamp(g, 0.f, 1.f);
    b = std::clamp(b, 0.f, 1.f);
    a = std::clamp(a, 0.f, 1.f);

    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

} // namespace dg
