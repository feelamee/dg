#include <engine/error.hpp>
#include <engine/mesh.hpp>
#include <engine/mesh_loader.hpp>
#include <engine/util.hpp>

#include <SDL3/SDL_iostream.h>

#include "engine/vertex_array.hpp"

#include <filesystem>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <tiny_gltf.h>

namespace dg
{

namespace
{

std::vector<std::byte>
load_file(std::filesystem::path const& filename)
{
    SDL_IOStream* const io = SDL_IOFromFile(filename.c_str(), "r");
    if (nullptr == io)
    {
        LOG_DEBUG("error occured openning file: %s", SDL_GetError());
        return {};
    }

    SDL_SeekIO(io, 0, SDL_IO_SEEK_END);
    size_t const filesize{ static_cast<size_t>(SDL_TellIO(io)) };
    SDL_SeekIO(io, 0, SDL_IO_SEEK_SET);

    std::vector<std::byte> buf;
    buf.resize(filesize);
    if (0 == SDL_ReadIO(io, buf.data(), filesize))
    {
        LOG_DEBUG("error occurs reading SDL_IOStream: %s", SDL_GetError());
    }

    if (0 != SDL_CloseIO(io))
    {
        LOG_DEBUG("error occurs closing SDL_IOStream: %s", SDL_GetError());
    }

    return buf;
}

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

                res.indices.push_back(v - 1);
                res.normals.at((v - 1) * stride) = normals[vn - 1];
                res.normals.at((v - 1) * stride + 1) = normals[vn];
                res.normals.at((v - 1) * stride + 2) = normals[vn + 1];
            }
        }
    }

    return res;
}

std::optional<mesh>
load_gltf(std::filesystem::path const& filename)
{
    auto const data{ load_file(filename) };

    tinygltf::TinyGLTF gltf;
    tinygltf::Model model;
    std::string err;
    if (!gltf.LoadBinaryFromMemory(&model, &err, nullptr,
                                   reinterpret_cast<const unsigned char*>(data.data()), data.size()))
    {
        LOG_DEBUG("error occurs loading gltf from file %s:\n\t%s", filename.c_str(), err.c_str());
        return std::nullopt;
    }

    mesh res;

    const tinygltf::Mesh& gltf_mesh = model.meshes[0];

    const tinygltf::Primitive& primitive = gltf_mesh.primitives[0];
    const tinygltf::Accessor& position_accessor =
        model.accessors[primitive.attributes.at("POSITION")];
    const tinygltf::BufferView& position_buffer_view = model.bufferViews[position_accessor.bufferView];
    const tinygltf::Buffer& position_buffer = model.buffers[position_buffer_view.buffer];

    res.vertices.resize(position_accessor.count * 3);
    std::memcpy(res.vertices.data(),
                position_buffer.data.data() + position_buffer_view.byteOffset + position_accessor.byteOffset,
                position_accessor.count * sizeof(float) * 3);

    if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
    {
        const tinygltf::Accessor& normal_accessor =
            model.accessors[primitive.attributes.at("NORMAL")];
        const tinygltf::BufferView& normal_buffer_view = model.bufferViews[normal_accessor.bufferView];
        const tinygltf::Buffer& normal_buffer = model.buffers[normal_buffer_view.buffer];

        res.normals.resize(normal_accessor.count * 3);
        std::memcpy(res.normals.data(),
                    normal_buffer.data.data() + normal_buffer_view.byteOffset + normal_accessor.byteOffset,
                    normal_accessor.count * sizeof(float) * 3);
    }

    if (primitive.indices >= 0)
    {
        const tinygltf::Accessor& index_accessor = model.accessors[primitive.indices];
        const tinygltf::BufferView& index_buffer_view = model.bufferViews[index_accessor.bufferView];
        const tinygltf::Buffer& index_buffer = model.buffers[index_buffer_view.buffer];

        size_t index_size =
            (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) ? sizeof(uint32_t)
            : (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) ? sizeof(uint16_t)
                                                                                       : 0;

        if (index_size == 0)
        {
            LOG_DEBUG("Unsupported index component type");
            return std::nullopt;
        }

        size_t required_size = index_accessor.count * index_size;
        if (required_size > index_buffer.data.size() - index_buffer_view.byteOffset)
        {
            LOG_DEBUG("Index data exceeds buffer size")
            return std::nullopt;
        }

        if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
        {
            const auto* index_data = reinterpret_cast<const uint16_t*>(
                index_buffer.data.data() + index_buffer_view.byteOffset + index_accessor.byteOffset);

            res.indices.resize(index_accessor.count);
            for (size_t i = 0; i < index_accessor.count; ++i)
            {
                res.indices[i] = static_cast<uint32_t>(index_data[i]);
            }
        } else if (index_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
        {
            res.indices.resize(index_accessor.count);
            std::memcpy(res.indices.data(),
                        index_buffer.data.data() + index_buffer_view.byteOffset + index_accessor.byteOffset,
                        required_size);
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

    case model_t::gltf:
        return load_gltf(filename);
    }

    unreachable();
}

} // namespace dg
