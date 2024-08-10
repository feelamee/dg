#include <engine/context.hpp>

#include <SDL3/SDL_events.h>

#include <cstdlib>

int
main()
{
    using namespace dg;

    context ctx("window", { 590, 960 });

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

        ctx.clear_window({ 0.2, 0.5, 1, 1 });
        ctx.swap_window();
    }

    return EXIT_SUCCESS;
}
