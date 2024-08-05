#include <engine/context.hpp>
#include <engine/util.hpp>
#include <engine/window.hpp>

#include <SDL2/SDL_video.h>

#include <format>

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

void
window::internal_window_deleter::operator()(internal_window_t* w)
{
    if (nullptr != w)
    {
        SDL_DestroyWindow(static_cast<SDL_Window*>(w));
    }
}

window::window(context const& /*cxt*/, char const* const title, glm::i32vec2 pos, glm::u32vec2 size,
               window::flag flags)
{
    int internal_flags{ SDL_WINDOW_OPENGL };

    if ((flags & window::flag::shown) != window::flag::none)
    {
        internal_flags |= SDL_WINDOW_OPENGL;
    }

    SDL_Window* sdl_window = SDL_CreateWindow(title, pos.x, pos.y, static_cast<int>(size.x),
                                              static_cast<int>(size.y), internal_flags);
    if (nullptr == sdl_window)
    {
        throw window_error(std::format("internal window init failed with: {}", SDL_GetError()));
    }

    internal_window.reset(sdl_window);
}

window::window(window&& /*w*/) { unimplemented(); }

window&
window::operator=(window w)
{
    using std::swap;

    swap(internal_window, w.internal_window);

    return *this;
}

} // namespace dg
