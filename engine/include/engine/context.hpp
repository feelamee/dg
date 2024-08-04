#pragma once

#include <engine/window.hpp>

#include <glm/vec2.hpp>

#include <stdexcept>

namespace dg
{

struct context_error : public std::runtime_error
{
    explicit context_error(std::string const&);
    context_error(char const*);
};

struct context
{
public:
    enum class flag
    {
        none = 0,

        video,
        event
    };
    friend flag operator|(flag, flag);
    friend flag& operator|=(flag&, flag);
    friend flag operator&(flag, flag);

    /*
     * @throws `std::engine_error`
     */
    context(flag);
    context(context&&) = default;
    context(context const&);

    context& operator=(context);
    context& operator=(context&&) = delete;
    context& operator=(context const&) = delete;

    ~context();

    window make_window(char const* const title, glm::i32vec2, glm::u32vec2, window::flag) const;

private:
    void init(flag);
};

} // namespace dg
