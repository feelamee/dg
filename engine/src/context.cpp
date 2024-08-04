#include <engine/context.hpp>
#include <engine/util.hpp>

#include <SDL2/SDL.h>

#include "SDL_video.h"

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

context::flag
operator|(context::flag l, context::flag r)
{
    return static_cast<context::flag>(to_underlying(l) | to_underlying(r));
}

context::flag&
operator|=(context::flag& l, context::flag r)
{
    return l = l | r;
}

context::flag
operator&(context::flag l, context::flag r)
{
    return static_cast<context::flag>(to_underlying(l) & to_underlying(r));
}

void
context::init(flag flags)
{
    static auto const throw_on_error = [](int errc)
    {
        if (0 != errc)
        {
            throw context_error(std::format("internal initialization failed with: {}", SDL_GetError()));
        }
    };

    if ((flags & flag::video) != flag::none)
    {
        int errc = SDL_InitSubSystem(SDL_INIT_VIDEO);
        throw_on_error(errc);
    }

    if ((flags & flag::event) != flag::none)
    {
        int errc = SDL_InitSubSystem(SDL_INIT_EVENTS);
        throw_on_error(errc);
    }
}

context::context(flag flags) { init(flags); }

context::context(context const& /*e*/)
{
    unsigned const internal_flags = SDL_WasInit(0);
    flag flags{ flag::none };

    if ((internal_flags & SDL_INIT_VIDEO) != 0)
    {
        flags |= flag::video;
    }

    if ((internal_flags & SDL_INIT_EVENTS) != 0)
    {
        flags |= flag::event;
    }

    init(flags);
}

context&
context::operator=(context /*e*/) // NOLINT(performance-unnecessary-value-param)
{
    using std::swap;

    return *this;
}

context::~context()
{
    unsigned const flags = SDL_WasInit(0);
    SDL_QuitSubSystem(flags);

    if (SDL_WasInit(0) == 0)
    {
        SDL_Quit();
    }
}

window
context::make_window(char const* const title, glm::i32vec2 pos, glm::u32vec2 size, window::flag flags) const
{
    int internal_flags{ 0 };

    if ((flags & window::flag::shown) != window::flag::none)
    {
        internal_flags |= SDL_WINDOW_OPENGL;
    }
    if ((flags & window::flag::opengl) != window::flag::none)
    {
        internal_flags |= SDL_WINDOW_SHOWN;
    }

    SDL_Window* internal_window = SDL_CreateWindow(title, pos.x, pos.y, static_cast<int>(size.x),
                                                   static_cast<int>(size.y), internal_flags);
    if (nullptr == internal_window)
    {
        throw context_error(std::format("internal window init failed with: {}", SDL_GetError()));
    }

    window w;
    w.internal_window.reset(internal_window);

    return w;
}

} // namespace dg
