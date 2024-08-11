#pragma once

#include <engine/bindable.hpp>

#include <any>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace dg
{

struct context;
struct mesh;

struct vertex_array : public bindable
{
public:
    struct error : public std::runtime_error
    {
        explicit error(std::string const&);
        error(char const*);
    };

    vertex_array(context const& ctx);

    vertex_array(vertex_array const&) = delete;
    vertex_array(vertex_array&&);

    vertex_array& operator=(vertex_array);
    vertex_array& operator=(vertex_array const&) = delete;
    vertex_array& operator=(vertex_array&&) = delete;

    ~vertex_array() override;

    enum class data_t
    {
        immutable,
        dynamic,
        stream
    };
    using location = uint32_t;
    using vertex_type = float;
    using index_type = uint32_t;
    void load(location loc, data_t type, std::vector<vertex_type> const& vertices,
              std::vector<index_type> const& indices);

    std::any bind() override;
    void unbind(std::any data) override;

private:
    using handle_t = uint32_t;
    handle_t handle{ 0 };
};

} // namespace dg
