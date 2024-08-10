#pragma once

#include <cstdint>
#include <stdexcept>

namespace dg
{

struct context;

struct shader_program
{
public:
    struct error : public std::runtime_error
    {
        explicit error(std::string const&);
        error(char const*);
    };

    shader_program(context const& ctx);

    shader_program(shader_program const&) = delete;
    shader_program(shader_program&&);

    shader_program& operator=(shader_program);
    shader_program& operator=(shader_program const&) = delete;
    shader_program& operator=(shader_program&&) = delete;

    ~shader_program();

    enum class shader_t
    {
        vertex,
        fragment
    };

    void attach_from_src(shader_t type, std::string_view src);
    bool link();
    void use(bool is_use = true);

private:
    uint64_t handle{ 0 };
};

} // namespace dg
