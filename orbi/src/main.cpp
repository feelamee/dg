#include <engine/bind_guard.hpp>
#include <engine/context.hpp>
#include <engine/error.hpp>
#include <engine/mesh.hpp>
#include <engine/mesh_loader.hpp>
#include <engine/shader_program.hpp>
#include <engine/vertex_array.hpp>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_time.h>

#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#include <cstdlib>
#include <filesystem>
#include <string_view>

constexpr std::string_view vertex_shader_src = R"(
#version 320 es

layout (location = 0) in vec3 position;

layout (location  = 1) uniform vec4 color;

layout (location = 2) uniform mat4 projection;
layout (location = 3) uniform mat4 view;
layout (location = 4) uniform mat4 model;


out vec4 vertex_color;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vertex_color = color;
}
)";

constexpr std::string_view fragment_shader_src = R"(
#version 320 es
precision mediump float;

in vec4 vertex_color;

out vec4 color;

void main()
{
    color = vertex_color;
}
)";

int
main(int argc, char** argv)
{
    using namespace dg;

    glm::vec2 win_size{ 960, 590 };
    context ctx("window", win_size);
    ctx.enable(context::capability::depth_test);

    shader_program program(ctx);
    program.attach_from_src(shader_program::shader_t::fragment, fragment_shader_src);
    program.attach_from_src(shader_program::shader_t::vertex, vertex_shader_src);
    assert(program.link());

    auto const mesh = load(model_t::obj, std::filesystem::path(argv[0]).parent_path() /
                                             context::resources_path() / "torus.obj");
    assert(mesh.has_value());

    vertex_array vao(ctx);
    vao.load(vertex_array::data_t::immutable, mesh.value());

    while (true)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_EVENT_QUIT)
            {
                return EXIT_SUCCESS;
            } else if (ev.type == SDL_EVENT_WINDOW_RESIZED)
            {
                auto const size = ctx.window_size();
                glViewport(0, 0, size.x, size.y);
            }
        }

        ctx.clear_window({ 0.2, 0.5, 1, 1 });

        {
            bind_guard _{ program };

            // SDL_Time ticks{};
            // assert(0 == SDL_GetCurrentTime(&ticks));
            // float fticks = ticks % 360'000'000'000 / 1E8;
            program.uniform(1, glm::vec4{ 1.0f, 0.5f, 0.31f, 1.0f });

            {
                auto const size = ctx.window_size();
                glm::mat4 const proj =
                    glm::perspective(glm::radians(45.0f), (float)size.x / (float)size.y, 0.1f, 100.0f);
                glm::mat4 const view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
                glm::mat4 const model = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
                                                                   glm::vec3(1.0f, 0.0f, 0.0f)),
                                                       glm::vec3(0.0f, 0.0f, 0.0f));
                program.uniform(2, proj);
                program.uniform(3, view);
                program.uniform(4, model);
            }

            {
                bind_guard _{ vao };

                GL_CHECK(glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, nullptr));
            }
        }

        ctx.swap_window();
    }

    return EXIT_SUCCESS;
}
