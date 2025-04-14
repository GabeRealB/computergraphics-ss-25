#pragma once

#include <filesystem>
#include <string>

/// Returns the absolute path to the applications resource directory.
inline const std::filesystem::path& get_resource_dir()
{
    static std::filesystem::path resource_dir = RESOURCE_PATH;
    return resource_dir;
}

/// Returns the absolute path to the resource with a given name.
///
/// @param resource_name Name of the resource relative to the resource directory.
inline std::filesystem::path to_resource_path(const std::string& resource_name)
{
    return get_resource_dir() / resource_name;
}
