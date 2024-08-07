#include <engine/context.hpp>
#include <engine/window.hpp>

#include <SDL3/SDL_events.h>

#include <cstdlib>

int
main()
{
    using namespace dg;
    context ctx(context::flag::everything);
    window win(ctx, "window", { 960, 590 });
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
        win.swap();
    }
    return EXIT_SUCCESS;
}
