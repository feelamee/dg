#pragma once

#include <engine/bindable.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <cstdint>
#include <stdexcept>

namespace dg
{

struct context;

struct shader_program : public bindable
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

    ~shader_program() override;

    enum class shader_t
    {
        vertex,
        fragment
    };

    void attach_from_src(shader_t type, std::string_view src);
    bool link();

    std::any bind() override;
    void unbind(std::any data) override;

    using uniform_location = uint32_t;
    void uniform(uniform_location id, glm::vec3 const& vec);
    void uniform(uniform_location id, glm::mat4 const& mat);

private:
    using handle_t = uint64_t;
    handle_t handle{ 0 };
};

} // namespace dg
