#include <engine/bind_guard.hpp>
#include <engine/context.hpp>
#include <engine/error.hpp>
#include <engine/mesh.hpp>
#include <engine/mesh_loader.hpp>
#include <engine/shader_program.hpp>
#include <engine/vertex_array.hpp>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_time.h>

#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#include "glm/matrix.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string_view>

constexpr std::string_view vertex_shader_src = R"(
#version 320 es

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 in_normal;

layout (location = 2) uniform mat4 projection;
layout (location = 3) uniform mat4 view;
layout (location = 4) uniform mat4 model;

out vec3 normal;
out vec3 fragment_position;

void main()
{
    vec4 pos = vec4(position, 1.0f);
    gl_Position = projection * view * model * pos;
    fragment_position = vec3(model * pos);
    normal = in_normal;
}
)";

constexpr std::string_view fragment_shader_src = R"(
#version 320 es
precision mediump float;

in vec3 normal;
in vec3 fragment_position;

out vec4 color;

layout (location = 5) uniform vec4 vertex_color;
layout (location = 6) uniform vec3 light_color;
layout (location = 7) uniform vec3 light_position;
layout (location = 8) uniform float ambient_strength;
layout (location = 9) uniform mat3 normal_mat;
layout (location = 10) uniform vec3 camera_position;
layout (location = 11) uniform float specular_strength;

void main()
{
    vec3 ambient = ambient_strength * light_color;

    vec3 norm = normalize(normal_mat * normal);
    vec3 light_direction = normalize(light_position - fragment_position);
    float diff = max(dot(norm, light_direction), 0.0f);
    vec3 diffuse = diff * light_color;

    vec3 view_direction = normalize(camera_position - fragment_position);
    vec3 reflect_direction = reflect(-light_direction, norm);
    float spec = pow(max(dot(view_direction, reflect_direction), 0.0f), 32.0f);
    vec3 specular = specular_strength * spec * light_color;

    color = vec4(ambient + diffuse + specular, 1.0f) * vertex_color;
}
)";

constexpr std::string_view light_source_vertex_shader_src = R"(
#version 320 es

layout (location = 0) in vec3 position;

layout (location = 2) uniform mat4 projection;
layout (location = 3) uniform mat4 view;
layout (location = 4) uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
}
)";

constexpr std::string_view light_source_fragment_shader_src = R"(
#version 320 es
precision mediump float;

out vec4 color;

layout (location = 5) uniform vec4 vertex_color;

void main()
{
    color = vertex_color;
}
)";

int
main(int /*argc*/, char** argv)
{
    using namespace dg;

    glm::vec2 win_size{ 960, 590 };
    context ctx("window", win_size);
    ctx.enable(context::capability::depth_test);

    shader_program program(ctx);
    program.attach_from_src(shader_program::shader_t::fragment, fragment_shader_src);
    program.attach_from_src(shader_program::shader_t::vertex, vertex_shader_src);
    assert(program.link());

    shader_program light_source_program(ctx);
    light_source_program.attach_from_src(shader_program::shader_t::fragment, light_source_fragment_shader_src);
    light_source_program.attach_from_src(shader_program::shader_t::vertex, light_source_vertex_shader_src);
    assert(light_source_program.link());

    using fspath = std::filesystem::path;
    fspath const resdir{ fspath(argv[0]).parent_path() / context::resources_path() };

    auto const cube_mesh = load(model_t::obj, resdir / "cube.obj");
    assert(cube_mesh.has_value());

    vertex_array cube_vao(ctx);
    cube_vao.load(0, vertex_array::data_t::immutable, cube_mesh.value().vertices);
    cube_vao.load(1, vertex_array::data_t::immutable, cube_mesh.value().normals);
    cube_vao.load_indices(vertex_array::data_t::immutable, cube_mesh.value().indices);

    auto const suzanne_mesh = load(model_t::gltf, resdir / "suzanne.glb");
    assert(suzanne_mesh.has_value());

    vertex_array suzanne_vao(ctx);
    suzanne_vao.load(0, vertex_array::data_t::immutable, suzanne_mesh.value().vertices);
    suzanne_vao.load(1, vertex_array::data_t::immutable, suzanne_mesh.value().normals);
    suzanne_vao.load_indices(vertex_array::data_t::immutable, suzanne_mesh.value().indices);

    auto const torus_mesh = load(model_t::gltf, resdir / "torus.glb");
    assert(torus_mesh.has_value());

    vertex_array torus_vao(ctx);
    torus_vao.load(0, vertex_array::data_t::immutable, torus_mesh.value().vertices);
    torus_vao.load(1, vertex_array::data_t::immutable, torus_mesh.value().normals);
    torus_vao.load_indices(vertex_array::data_t::immutable, torus_mesh.value().indices);

    auto const plane_mesh = load(model_t::obj, resdir / "plane.obj");
    assert(plane_mesh.has_value());

    vertex_array plane_vao(ctx);
    plane_vao.load(0, vertex_array::data_t::immutable, plane_mesh.value().vertices);
    plane_vao.load(1, vertex_array::data_t::immutable, plane_mesh.value().normals);
    plane_vao.load_indices(vertex_array::data_t::immutable, plane_mesh.value().indices);

    struct camera
    {
        glm::vec3 position{};
        glm::vec3 direction{};

        glm::vec3 right{};
        glm::vec3 up{};
    };

    float const speed = 0.1f;
    glm::vec3 const init_camera_position{ 0.0f, 0.0f, 10.0f };
    glm::vec3 const init_camera_direction{ 0.0f, 0.0f, -1.0f };

    camera cam{ .position = init_camera_position,
                .direction = init_camera_direction,
                .up = glm::vec3{ 0.0f, 1.0f, 0.0f } };
    cam.right = glm::normalize(glm::cross(cam.up, cam.direction));

    bool is_w{ false };
    bool is_s{ false };
    bool is_a{ false };
    bool is_d{ false };
    bool is_lshift{ false };
    bool is_lctrl{ false };

    float delta_time{ 0 };
    float last_ticks{ 0 };

    float pitch{ 0 };
    float yaw{ -90 };

    struct light
    {
        glm::vec3 position{};
        glm::vec3 color{};
        float ambient_strength{};
        float specular_strength{};
    };
    light light_source{ .color = { 1.0f, 1.0f, 1.0f }, .ambient_strength = 0.2f, .specular_strength = 0.9f };
    bool is_light_source_rotating_around{ true };

    while (true)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
            case SDL_EVENT_WINDOW_RESIZED:
            {
                auto const size = ctx.window_size();
                glViewport(0, 0, size.x, size.y);
                break;
            }

            case SDL_EVENT_KEY_DOWN:
            {
                switch (ev.key.key)
                {
                case SDLK_Q:
                    return EXIT_SUCCESS;
                case SDLK_W:
                    is_w = true;
                    break;
                case SDLK_S:
                    is_s = true;
                    break;
                case SDLK_A:
                    is_a = true;
                    break;
                case SDLK_D:
                    is_d = true;
                    break;
                case SDLK_LEFT:
                    light_source.position.x -= 0.1;
                    break;
                case SDLK_RIGHT:
                    light_source.position.x += 0.1;
                    break;
                case SDLK_UP:
                    light_source.position.y += 0.1;
                    break;
                case SDLK_DOWN:
                    light_source.position.y -= 0.1;
                    break;
                case SDLK_Z:
                    light_source.position.z += 0.1;
                    break;
                case SDLK_X:
                    light_source.position.z -= 0.1;
                    break;
                case SDLK_EQUALS:
                    cam.position = init_camera_position;
                    cam.direction = init_camera_direction;
                    break;
                case SDLK_MINUS:
                    is_light_source_rotating_around = !is_light_source_rotating_around;
                    break;
                }
                is_lshift = ev.key.mod & SDL_KMOD_LSHIFT;
                bool const new_is_lctrl = ev.key.mod & SDL_KMOD_LCTRL;
                if (is_lctrl != new_is_lctrl)
                {
                    ctx.window_mouse_position(glm::vec2(ctx.window_size()) / 2.0f);
                    is_lctrl = new_is_lctrl;
                }
                break;
            }

            case SDL_EVENT_KEY_UP:
            {
                switch (ev.key.key)
                {
                case SDLK_W:
                    is_w = false;
                    break;
                case SDLK_S:
                    is_s = false;
                    break;
                case SDLK_A:
                    is_a = false;
                    break;
                case SDLK_D:
                    is_d = false;
                    break;
                }
                is_lshift = ev.key.mod & SDL_KMOD_LSHIFT;
                bool const new_is_lctrl = ev.key.mod & SDL_KMOD_LCTRL;
                if (is_lctrl != new_is_lctrl)
                {
                    ctx.window_mouse_position(glm::vec2(ctx.window_size()) / 2.0f);
                    is_lctrl = new_is_lctrl;
                }
                break;
            }

            case SDL_EVENT_MOUSE_MOTION:
            {
                if (is_lctrl)
                {
                    ctx.window_relative_mouse_mode(false);
                    assert(0 == SDL_ShowCursor());
                } else
                {
                    ctx.window_relative_mouse_mode(true);
                    assert(0 == SDL_HideCursor());
                    pitch -= ev.motion.yrel * 0.05f;
                    yaw += ev.motion.xrel * 0.05f;

                    pitch = std::clamp(pitch, -89.0f, 89.0f);

                    cam.direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
                    cam.direction.y = sin(glm::radians(pitch));
                    cam.direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

                    cam.direction = glm::normalize(cam.direction);
                }
                break;
            }
            }
        }

        {
            float const real_speed = speed * (is_lshift ? 4 : 1);
            if (is_w) cam.position += delta_time * real_speed * cam.direction;
            if (is_s) cam.position -= delta_time * real_speed * cam.direction;
            if (is_a)
                cam.position -= glm::normalize(glm::cross(cam.direction, cam.up)) * delta_time * real_speed;
            if (is_d)
                cam.position += glm::normalize(glm::cross(cam.direction, cam.up)) * delta_time * real_speed;
        }

        ctx.clear_window({ 0.2, 0.5, 1, 1 });

        glm::mat4 proj{ 1.0f };
        glm::mat4 view{ 1.0f };
        {
            auto const size = ctx.window_size();
            float const ratio = static_cast<float>(size.x) / static_cast<float>(size.y);

            proj = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.0f);
            view = glm::lookAt(cam.position, cam.position + cam.direction, cam.up);

            program.uniform(7, light_source.position);
        }

        {
            bind_guard _{ program };

            program.uniform(2, proj);
            program.uniform(3, view);
            program.uniform(6, light_source.color);
            program.uniform(8, light_source.ambient_strength);
            program.uniform(10, cam.position);
            program.uniform(11, light_source.specular_strength);

            {
                glm::mat4 model{ 1.0f };
                model = glm::translate(model, glm::vec3{ 0, -1.0f, 0.5f });
                model = glm::scale(model, glm::vec3{ 1, 2, 3 });

                program.uniform(4, model);
                program.uniform(5, glm::vec4{ 1.0f, 0.5f, 0.31f, 1.0f });

                glm::mat3 normal_mat = glm::transpose(glm::inverse(model));
                program.uniform(9, normal_mat);

                bind_guard _{ torus_vao };

                GL_CHECK(glDrawElements(GL_TRIANGLES, torus_mesh->indices.size(), GL_UNSIGNED_INT, nullptr));
            }

            {
                glm::mat4 model{ 1.0f };

                program.uniform(4, model);
                program.uniform(5, glm::vec4{ 1.0f, 0.5f, 0.31f, 1.0f });

                glm::mat3 normal_mat = glm::transpose(glm::inverse(model));
                program.uniform(9, normal_mat);

                bind_guard _{ suzanne_vao };

                GL_CHECK(glDrawElements(GL_TRIANGLES, suzanne_mesh->indices.size(), GL_UNSIGNED_INT, nullptr));
            }

            {
                glm::mat4 model{ 1.0f };
                model = glm::translate(model, glm::vec3{ 0.0f, -3.0f, 0.0f });
                model = glm::scale(model, glm::vec3{ 5 });

                program.uniform(4, model);
                program.uniform(5, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

                glm::mat3 normal_mat = glm::transpose(glm::inverse(model));
                program.uniform(9, normal_mat);

                bind_guard _{ plane_vao };

                GL_CHECK(glDrawElements(GL_TRIANGLES, plane_mesh->indices.size(), GL_UNSIGNED_INT, nullptr));
            }
        }

        {
            bind_guard _1{ light_source_program };

            light_source_program.uniform(2, proj);
            light_source_program.uniform(3, view);

            glm::mat4 model{ 1.0f };
            model = glm::translate(model, light_source.position);
            model = glm::scale(model, glm::vec3{ 0.05f });

            light_source_program.uniform(4, model);
            light_source_program.uniform(5, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

            bind_guard _2{ cube_vao };

            GL_CHECK(glDrawElements(GL_TRIANGLES, cube_mesh->indices.size(), GL_UNSIGNED_INT, nullptr));
        }

        ctx.swap_window();

        {
            SDL_Time ticks{};
            assert(0 == SDL_GetCurrentTime(&ticks));
            float fticks = ticks % 360'000'000'000 / 1E8;

            delta_time = fticks - last_ticks;
            last_ticks = fticks;
        }

        if (is_light_source_rotating_around)
        {
            light_source.position.x = sin(last_ticks * 0.1) * 4.0f;
            light_source.position.y = sin(last_ticks / 2.0f * 0.1) * 2.0f;
            light_source.position.z = cos(last_ticks * 0.1) * 4.0f;
        }
    }

    return EXIT_SUCCESS;
}
