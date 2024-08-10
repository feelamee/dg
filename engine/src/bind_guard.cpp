#include <engine/bind_guard.hpp>
#include <engine/bindable.hpp>

namespace dg
{

bind_guard::bind_guard(bindable& o)
    : obj{ o }
{
    data = obj.bind();
}

bind_guard::~bind_guard() { obj.unbind(data); }

} // namespace dg
