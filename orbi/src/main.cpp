#include <engine/context.hpp>
#include <engine/window.hpp>

#include <SDL2/SDL_events.h>

#include <cstdlib>

int
main()
{
    using namespace dg;
    context ctx(context::flag::everything);
    window win(ctx, "window", { 480, 295 }, { 960, 590 }, window::flag::shown);
    while (true)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT)
            {
                return EXIT_SUCCESS;
            }
        }

        win.clear_with(0.2, 0.5, 1, 1);
        win.swap();
    }
    return EXIT_SUCCESS;
}
