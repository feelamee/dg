#include <engine/bind_guard.hpp>
#include <engine/context.hpp>
#include <engine/error.hpp>
#include <engine/shader_program.hpp>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_time.h>

#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#include <array>
#include <cstdlib>
#include <string_view>

constexpr std::string_view vertex_shader_src = R"(
#version 320 es

layout (location = 0) in vec3 position;

layout (location  = 1) uniform vec3 color;

layout (location = 2) uniform mat4 projection;
layout (location = 3) uniform mat4 view;
layout (location = 4) uniform mat4 model;


out vec4 vertex_color;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vertex_color = vec4(color, 1.0f);
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
main()
{
    using namespace dg;

    glm::vec2 win_size{ 960, 590 };
    context ctx("window", win_size);

    shader_program program(ctx);
    program.attach_from_src(shader_program::shader_t::fragment, fragment_shader_src);
    program.attach_from_src(shader_program::shader_t::vertex, vertex_shader_src);
    assert(program.link());

    // clang-format off
    std::array<GLfloat, 24> vertices {
       -0.5f, -0.5f,  0.0f, // 0 lbf
       -0.5f,  0.5f,  0.0f, // 1 luf
        0.5f,  0.5f,  0.0f, // 2 ruf
        0.5f, -0.5f,  0.0f, // 3 rbf

       -0.5f, -0.5f, -1.0f, // 4 lbb
       -0.5f,  0.5f, -1.0f, // 5 lub
        0.5f,  0.5f, -1.0f, // 6 rub
        0.5f, -0.5f, -1.0f, // 7 rbb
    };

    std::array<GLuint, 30> indices {
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7,

        1, 2, 5,
        2, 5, 6,

        0, 3, 7,
        0, 7, 4,

        
    };
    // clang-format on

    GLuint vao{ 0 };
    GL_CHECK(glGenVertexArrays(1, &vao));
    GL_CHECK(glBindVertexArray(vao));

    GLuint vbo{ 0 };
    GL_CHECK(glGenBuffers(1, &vbo));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr));

    GLuint vbe{ 0 };
    GL_CHECK(glGenBuffers(1, &vbe));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbe));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW));

    GL_CHECK(glEnableVertexAttribArray(0));

    GL_CHECK(glBindVertexArray(0));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    while (true)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_EVENT_QUIT)
            {
                goto cleanup;
            } else if (ev.type == SDL_EVENT_WINDOW_RESIZED)
            {
                auto const size = ctx.window_size();
                glViewport(0, 0, size.x, size.y);
            }
        }

        ctx.clear_window({ 0.2, 0.5, 1, 1 });

        {
            bind_guard _{ program };

            SDL_Time ticks{};
            assert(0 == SDL_GetCurrentTime(&ticks));
            float fticks = ticks % 360'000'000'000 / 1E8;
            program.uniform(1, { glm::sin(glm::radians(fticks)) / 2 + 0.5, 0.0f, 0.8f });

            {
                auto const size = ctx.window_size();
                glm::mat4 const proj =
                    glm::perspective(glm::radians(45.0f), (float)size.x / (float)size.y, 0.1f, 100.0f);
                glm::mat4 const view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
                glm::mat4 const model = glm::translate(glm::rotate(glm::mat4(1.0f), glm::radians(fticks),
                                                                   glm::vec3(1.0f, 1.0f, 1.0f)),
                                                       glm::vec3(0.0f, 0.0f, 0.5f));
                program.uniform(2, proj);
                program.uniform(3, view);
                program.uniform(4, model);
            }

            GL_CHECK(glBindVertexArray(vao));

            GL_CHECK(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr));

            GL_CHECK(glBindVertexArray(0));
        }

        ctx.swap_window();
    }

cleanup:
    GL_CHECK(glDeleteVertexArrays(1, &vao));
    GL_CHECK(glDeleteBuffers(1, &vbo));
    GL_CHECK(glDeleteBuffers(1, &vbe));

    return EXIT_SUCCESS;
}
