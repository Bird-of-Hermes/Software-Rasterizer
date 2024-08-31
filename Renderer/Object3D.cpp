#include "Object3D.hpp"

std::filesystem::path ResolvePath(const std::filesystem::path& path, const std::filesystem::path& directory) noexcept 
{
    using namespace std;

    if (filesystem::exists(path)) 
    {
        return path;
    }

    filesystem::path path1 = directory / path;
    if (filesystem::exists(path1))
    {
        return path1;
    }

    path1 = directory / path.filename();
    if (filesystem::exists(path1))
    {
        return path1;
    }

    path1 = directory / "textures" / path.filename();
    if (filesystem::exists(path1))
    {
        return path1;
    }

    path1 = directory.parent_path() / "textures" / path.filename();
    if (filesystem::exists(path1))
    {
        return path1;
    }

    return path;
}