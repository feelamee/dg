#include <engine/engine.hpp>
#include <engine/util.hpp>

#include <SDL2/SDL.h>

#include <format>

namespace dg
{

engine::flag
operator|(engine::flag l, engine::flag r)
{
    return static_cast<engine::flag>(to_underlying(l) | to_underlying(r));
}

engine::flag&
operator|=(engine::flag& l, engine::flag r)
{
    return l = l | r;
}

engine::flag
operator&(engine::flag l, engine::flag r)
{
    return static_cast<engine::flag>(to_underlying(l) & to_underlying(r));
}

void
engine::init(flag flags)
{
    static auto const throw_on_error = [](int errc)
    {
        if (0 != errc)
        {
            throw engine_error(std::format("internal initialization failed with: {}", SDL_GetError()));
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

engine::engine(flag flags) { init(flags); }

engine::engine(engine const& /*e*/)
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

engine&
engine::operator=(engine e)
{
    using std::swap;

    swap(*this, e);
    return *this;
}

engine::~engine()
{
    unsigned const flags = SDL_WasInit(0);
    SDL_QuitSubSystem(flags);

    if (SDL_WasInit(0) == 0)
    {
        SDL_Quit();
    }
}

engine_error::engine_error(std::string const& msg)
    : std::runtime_error(msg)
{
}

engine_error::engine_error(char const* msg)
    : std::runtime_error(msg)
{
}

} // namespace dg
