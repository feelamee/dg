#include <engine/mesh.hpp>
#include <engine/mesh_loader.hpp>
#include <engine/util.hpp>

#include <filesystem>
#include <fstream>
#include <optional>

namespace dg
{

namespace
{

std::optional<mesh>
load_obj(std::filesystem::path const& filename)
{
    mesh res;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        return std::nullopt;
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
