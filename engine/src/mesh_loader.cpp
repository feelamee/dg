#include <engine/error.hpp>
#include <engine/mesh.hpp>
#include <engine/mesh_loader.hpp>
#include <engine/util.hpp>

#include <SDL3/SDL_iostream.h>

#include "engine/vertex_array.hpp"

#include <filesystem>
#include <optional>
#include <sstream>
#include <string>

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

    // TODO: rewrite
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
        } else if (prefix == "vn")
        {
            float x{}, y{}, z{};
            if (iss >> x >> y >> z)
            {
                res.normals.push_back(x);
                res.normals.push_back(y);
                res.normals.push_back(z);
            }
        } else if (prefix == "f")
        {
            std::string tok;
            while (iss >> tok)
            {
                int v{}, vt{}, vn{};

                int const n = std::sscanf(tok.c_str(), "%d/%d/%d", &v, &vt, &vn);
                if (n != 3) break;

                res.vertex_indices.push_back(v - 1);
                res.normal_indices.push_back(vn - 1);
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
    case model_t::obj:
        return load_obj(filename);
    }

    unreachable();
}

} // namespace dg
