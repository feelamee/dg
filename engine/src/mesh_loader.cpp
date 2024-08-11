#include <engine/error.hpp>
#include <engine/mesh.hpp>
#include <engine/mesh_loader.hpp>
#include <engine/util.hpp>

#include <SDL3/SDL_iostream.h>

#include <filesystem>
#include <optional>
#include <sstream>

namespace dg
{

namespace
{

std::optional<mesh>
load_obj(std::filesystem::path const& filename)
{
    mesh res;

    // TODO: extract loading file into memory to separate function
    std::stringstream file;
    {
        SDL_IOStream* const io = SDL_IOFromFile(filename.c_str(), "r");
        if (nullptr == io)
        {
            LOG_DEBUG("error occured openning file: %s", SDL_GetError());
            return std::nullopt;
        }

        SDL_SeekIO(io, 0, SDL_IO_SEEK_END);
        size_t const filesize{ static_cast<size_t>(SDL_TellIO(io)) };
        SDL_SeekIO(io, 0, SDL_IO_SEEK_SET);

        std::string buf;
        buf.resize(filesize);
        SDL_ReadIO(io, buf.data(), filesize);

        file << buf;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            float x{}, y{}, z{};
            if (iss >> x >> y >> z)
            {
                res.vertices.push_back(x);
                res.vertices.push_back(y);
                res.vertices.push_back(z);
            }
        } else if (prefix == "f")
        {
            uint32_t index1{}, index2{}, index3{};
            if (iss >> index1 >> index2 >> index3)
            {
                res.indices.push_back(index1 - 1);
                res.indices.push_back(index2 - 1);
                res.indices.push_back(index3 - 1);
            }
        }
    }

    return res;
}

} // namespace

std::optional<mesh>
load(model_t type, std::filesystem::path const& filename)
{
    switch (type)
    {
    case model_t::obj: return load_obj(filename);
    }

    unreachable();
}

} // namespace dg
