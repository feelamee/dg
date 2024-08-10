#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <cstdint>
#include <memory>
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
    /*
     * @throws `context_error`, `std::bad_alloc`
     */
    context(char const* const window_title, glm::u32vec2 window_size);
    context(context&&);
    context(const context&) = delete;

    context& operator=(context);
    context& operator=(context&&) = delete;
    context& operator=(context const&) = delete;

    ~context();

    void clear_window(glm::vec4 color = { 0, 0, 0, 1 });
    void swap_window() const;

private:
    static context const* ctx;

    using flag_t = uint64_t;
    void init(flag_t flags);

    struct internal_data;
    struct internal_data_deleter
    {
        void operator()(internal_data*);
    };

    std::unique_ptr<internal_data, internal_data_deleter> data;
};

} // namespace dg
