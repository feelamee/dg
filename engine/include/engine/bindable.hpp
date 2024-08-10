#pragma once

#include <any>

namespace dg
{

struct bindable
{
public:
    virtual ~bindable() = default;

    virtual std::any bind() = 0;
    virtual void unbind(std::any) = 0;
};

} // namespace dg
