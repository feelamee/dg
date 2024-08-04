#include <engine/util.hpp>

#include <SDL2/SDL_log.h>

namespace dg
{

void
unimplemented(char const* const msg)
{
    if (msg)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_ASSERT, "%s", msg);
    } else
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_ASSERT, "not implemented yet :(");
    }

    std::abort();
}

} // namespace dg
