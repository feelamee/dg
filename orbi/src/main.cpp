#include <engine/context.hpp>
#include <engine/window.hpp>

#include <cstdlib>

int
main()
{
    using namespace dg;
    context ctx(context::flag::everything);
    window win(ctx, "window", { 480, 295 }, { 960, 590 }, window::flag::shown);
    return EXIT_SUCCESS;
}
