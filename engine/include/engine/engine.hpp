#pragma once

#include <stdexcept>

namespace dg
{

struct engine
{
public:
    enum class flag
    {
        none = 0,

        video,
        event
    };
    friend engine::flag operator|(engine::flag, engine::flag);
    friend engine::flag& operator|=(engine::flag&, engine::flag);
    friend engine::flag operator&(engine::flag, engine::flag);

    /*
     * @throws `std::engine_error`
     */
    engine(flag);
    engine(engine&&) = default;
    engine(engine const&);

    engine& operator=(engine);
    engine& operator=(engine&&) = delete;
    engine& operator=(engine const&) = delete;

    ~engine();

private:
    void init(flag);
};

struct engine_error : public std::runtime_error
{
    explicit engine_error(std::string const&);
    engine_error(char const*);
};

} // namespace dg
