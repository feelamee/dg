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
        if (0 == SDL_ReadIO(io, buf.data(), filesize))
        {
            LOG_DEBUG("error occurs reading SDL_IOStream: %s", SDL_GetError());
        }

        file << buf;

        if (0 != SDL_CloseIO(io))
        {
            LOG_DEBUG("error occurs closing SDL_IOStream: %s", SDL_GetError());
        }
    }

    std::vector<mesh::coord_type> normals;

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
                normals.push_back(x);
                normals.push_back(y);
                normals.push_back(z);
            }
        } else if (prefix == "f")
        {
            if (res.normals.empty())
            {
                res.normals.resize(res.vertices.size());
            }

            std::string tok;
            while (iss >> tok)
            {
                int v{}, vt{}, vn{};

                uint32_t const stride{ 3 };

                int const n = std::sscanf(tok.c_str(), "%d/%d/%d", &v, &vt, &vn);
                if (n != 3) break;

                res.vertex_indices.push_back(v - 1);
                res.normals.at((v - 1) * stride) = normals[vn - 1];
                res.normals.at((v - 1) * stride + 1) = normals[vn];
                res.normals.at((v - 1) * stride + 2) = normals[vn + 1];
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
