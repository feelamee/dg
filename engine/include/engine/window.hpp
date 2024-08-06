#pragma once

#include <glm/vec2.hpp>

#include <memory>

namespace dg
{

struct context;

struct window_error : public std::runtime_error
{
    explicit window_error(std::string const&);
    window_error(char const*);
};

struct window
{
public:
    enum class flag : unsigned
    {
        none = 0,

        shown = 1 << 0,
    };
    friend flag operator|(flag, flag);
    friend flag& operator|=(flag&, flag);
    friend flag operator&(flag, flag);

    /*
     * @throws `window_error`, `std::bad_alloc`
     */
    window(context const& ctx, char const* const title, glm::i32vec2 pos, glm::u32vec2 size,
           window::flag flags = window::flag::none);

    window(window const&) = delete;
    window(window&&);

    window& operator=(window);
    window& operator=(window&&) = delete;
    window& operator=(window const&) = delete;

    ~window() = default;

    void clear_with(float r, float g, float b, float a);
    void swap() const;

private:
    struct internal_data;
    struct internal_data_deleter
    {
        void operator()(internal_data*);
    };

    std::unique_ptr<internal_data, internal_data_deleter> data;
};

} // namespace dg
