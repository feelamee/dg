#include <engine/context.hpp>
#include <engine/util.hpp>
#include <engine/window.hpp>

#include <SDL2/SDL_assert.h>
#include <SDL2/SDL_video.h>

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
            SDL_GL_DeleteContext(data->gl_context);
        }

        delete data;
    }
}

window::window(context const& /*cxt*/, char const* const title, glm::i32vec2 pos, glm::u32vec2 size,
               window::flag flags)
{
    int internal_flags{ SDL_WINDOW_OPENGL };

    if ((flags & window::flag::shown) != window::flag::none)
    {
        internal_flags |= SDL_WINDOW_SHOWN;
    }

    SDL_Window* sdl_window = SDL_CreateWindow(title, pos.x, pos.y, static_cast<int>(size.x),
                                              static_cast<int>(size.y), internal_flags);
    if (nullptr == sdl_window)
    {
        throw window_error(std::format("internal window init failed with: {}", SDL_GetError()));
    }

    {
        GLint desired_major_version{ 3 };
        GLint desired_minor_version{ 2 };
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

    int errc = gladLoadGLES2Loader(SDL_GL_GetProcAddress);
    SDL_assert_release(0 != errc);

    auto p = new internal_data{ .sdl_window = sdl_window, .gl_context = gl_context };
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

} // namespace dg
