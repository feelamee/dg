#pragma once

#include <any>

namespace dg
{

struct bindable;

struct bind_guard
{
public:
    bind_guard(bindable&);

    bind_guard(const bind_guard&) = delete;
    bind_guard(bind_guard&&) = delete;
    bind_guard& operator=(const bind_guard&) = delete;
    bind_guard& operator=(bind_guard&&) = delete;

    ~bind_guard();

private:
    bindable& obj;
    std::any data;
};

} // namespace dg
