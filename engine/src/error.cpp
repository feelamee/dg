#include <engine/error.hpp>

#include <SDL3/SDL_log.h>

#include <glad/glad.h>

#include <cstdlib>

namespace dg
{

void
log_error(int /*line*/, char const* /*fn*/, char const* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, fmt, args);
    va_end(args);
}

void
gl_check(std::string_view file, unsigned int line, std::string_view expression)
{
    GLenum errc = glGetError();

    while (GL_NO_ERROR != errc)
    {
        std::string_view error = "Unknown error";
        std::string_view description = "No description";

        switch (errc)
        {
        case GL_INVALID_ENUM: {
            error = "GL_INVALID_ENUM";
            description = "An unacceptable value has been specified for an "
                          "enumerated argument.";
            break;
        }

        case GL_INVALID_VALUE: {
            error = "GL_INVALID_VALUE";
            description = "A numeric argument is out of range.";
            break;
        }

        case GL_INVALID_OPERATION: {
            error = "GL_INVALID_OPERATION";
            description = "The specified operation is not allowed in the "
                          "current state.";
            break;
        }

        case GL_OUT_OF_MEMORY: {
            error = "GL_OUT_OF_MEMORY";
            description = "There is not enough memory left to execute the command.";
            break;
        }

        case GL_INVALID_FRAMEBUFFER_OPERATION: {
            error = "GL_INVALID_FRAMEBUFFER_OPERATION";
            description = "The object bound to FRAMEBUFFER_BINDING is not "
                          "\"framebuffer complete\".";
            break;
        }
        }

        LOG_DEBUG("[ %s : %d ]:"
                  "\n    An internal OpenGL call failed"
                  "\n    Expression:\n        %s"
                  "\n    Error description:\n        %s\n",
                  file.data(), line, expression.data(), description.data());

        errc = glGetError();
    }
}

} // namespace dg
