#include <type_traits>

namespace dg
{

template <class Enum>
constexpr std::underlying_type_t<Enum>
to_underlying(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

[[noreturn]] void unimplemented(char const* const msg = nullptr);

[[noreturn]] inline void
unreachable()
{
    // Uses compiler specific extensions if possible.
    // Even if no extension is used, undefined behavior is still raised by
    // an empty function body and the noreturn attribute.
#if defined(_MSC_VER) && !defined(__clang__) // MSVC
    __assume(false);
#else // GCC, Clang
    __builtin_unreachable();
#endif
}

} // namespace dg
