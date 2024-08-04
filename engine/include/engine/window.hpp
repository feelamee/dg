#pragma once

#include <memory>

namespace dg
{

struct context;

struct window
{
public:
    enum class flag
    {
        none,

        opengl,
        shown,
    };
    friend flag operator|(flag, flag);
    friend flag& operator|=(flag&, flag);
    friend flag operator&(flag, flag);

    window(window const&);
    window(window&&);

    window& operator=(window);
    window& operator=(window&&) = delete;
    window& operator=(window const&) = delete;

    ~window() = default;

private:
    window() = default;

    friend context;

    using internal_window_t = void;
    struct internal_window_deleter
    {
        void operator()(internal_window_t*);
    };

    std::unique_ptr<internal_window_t, internal_window_deleter> internal_window;
};

} // namespace dg
