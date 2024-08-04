#include <type_traits>

namespace dg
{

template <class Enum>
constexpr std::underlying_type_t<Enum>
to_underlying(Enum e) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

} // namespace dg
