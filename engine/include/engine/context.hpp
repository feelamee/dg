#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <stdexcept>

namespace dg
{

struct context
{
public:
    struct error : public std::runtime_error
    {
        explicit error(std::string const&);
        error(char const*);
    };

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

    enum class buffer
    {
        none = 0,

        depth = 1 << 0,
        color = 1 << 1
    };
    friend buffer operator|(buffer, buffer);
    friend buffer operator&(buffer, buffer);

    void clear_window(glm::vec4 color = { 0, 0, 0, 1 }, buffer mask = buffer::color | buffer::depth);
    void swap_window();
    [[nodiscard]] glm::u32vec2 window_size() const;

    static std::filesystem::path resources_path();

    enum class capability
    {
        depth_test
    };
    void enable(capability);

    void window_relative_mouse_mode(bool enable);
    void window_mouse_position(glm::vec2 pos);

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
