#pragma once

#include <cstdint>
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
    enum class flag : unsigned
    {
        none = 0,

        video = 1 << 0,
        event = 1 << 1,

        everything = video | event,
    };
    friend flag operator|(flag, flag);
    friend flag& operator|=(flag&, flag);
    friend flag operator&(flag, flag);

    /*
     * @throws `std::engine_error`
     */
    context(flag flags = flag::none);
    context(context&&) = default;
    context(context const&);

    context& operator=(context);
    context& operator=(context&&) = delete;
    context& operator=(context const&) = delete;

    ~context();

private:
    void init(flag);

    std::uint32_t init_flags;
};

} // namespace dg
