#include <engine/context.hpp>
#include <engine/util.hpp>

#include <SDL2/SDL.h>

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

} // namespace dg
