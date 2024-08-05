#include <engine/util.hpp>
#include <engine/window.hpp>

#include <SDL2/SDL_video.h>

namespace dg
{

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

window::window(window&& /*w*/) { unimplemented(); }

window&
window::operator=(window w)
{
    using std::swap;

    swap(internal_window, w.internal_window);

    return *this;
}

} // namespace dg
