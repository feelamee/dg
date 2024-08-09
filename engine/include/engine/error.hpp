#ifndef ERROR_HPP
#define ERROR_HPP

#include <string_view>

#ifndef NDEBUG
#define GL_CHECK(expr)                                                                             \
    {                                                                                              \
        (expr);                                                                                    \
        ::dg::gl_check(__FILE__, __LINE__, #expr);                                                 \
    }

#define LOG_DEBUG(...)                                                                             \
    {                                                                                              \
        ::dg::log_error(__LINE__, __FILE__, __VA_ARGS__);                                          \
    }

#else
#define GL_CHECK(expr) (expr);
#define LOG_DEBUG(...)
#endif

namespace dg
{

void log_error(int line, char const* fn, char const* fmt, ...);

///! `filename` and `expr` must be null-terminated
void gl_check(std::string_view filename, unsigned int line, std::string_view expr);

} // namespace dg

#endif // ERROR_HPP
