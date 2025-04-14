#pragma once
#include <stb_image.h>

#include <cstring>
#include <filesystem>
#include <memory>
#include <stdexcept>

#include "Resources.hpp"

/// Wrapper over an image in memory.
class Image {
public:
    /// Creates a new empty image.
    ///
    /// @param width Width in pixel.
    /// @param height Height in pixel.
    /// @param channels Number of channels.
    Image(int width, int height, int channels)
        : m_data { nullptr }
        , m_width { width }
        , m_height { height }
        , m_channels { channels }
    {
        if (width <= 0) {
            throw std::runtime_error { "Invalid image width." };
        }
        if (height <= 0) {
            throw std::runtime_error { "Invalid image height." };
        }
        if (channels < 1 || channels > 4) {
            throw std::runtime_error { "The image must have between 1 and 4 channels." };
        }

        std::size_t width_sz = static_cast<std::size_t>(width);
        std::size_t height_sz = static_cast<std::size_t>(height);
        std::size_t channels_sz = static_cast<std::size_t>(channels);
        auto elements = width_sz * height_sz * channels_sz;
        this->m_data = std::make_unique<unsigned char[]>(elements);
    }

    /// Loads an image from the resource directory.
    ///
    /// @param file_name Absolute path to the image file.
    Image(std::filesystem::path file_name)
    {
        auto file_path_str = file_name.string();
        auto data = stbi_load(file_path_str.c_str(), &this->m_width, &this->m_height, &this->m_channels, 0);
        if (data == nullptr) {
            throw std::runtime_error { "Could not load image at path: " + file_path_str };
        }

        try {
            std::size_t width = static_cast<std::size_t>(this->m_width);
            std::size_t height = static_cast<std::size_t>(this->m_height);
            std::size_t channels = static_cast<std::size_t>(this->m_channels);
            auto elements = width * height * channels;

            this->m_data = std::make_unique<unsigned char[]>(elements);
            std::memcpy(this->m_data.get(), data, elements);
            stbi_image_free(data);
        } catch (std::exception&) {
            stbi_image_free(data);
        }
    }

    /// Returns the data of the image.
    unsigned char* data() noexcept
    {
        return this->m_data.get();
    }

    /// Returns the data of the image.
    const unsigned char* data() const noexcept
    {
        return this->m_data.get();
    }

    /// Returns the width of the image.
    int width() const noexcept
    {
        return this->m_width;
    }

    /// Returns the height of the image.
    int height() const noexcept
    {
        return this->m_height;
    }

    /// Returns the number of channels contained in the image.
    int channels() const noexcept
    {
        return this->m_channels;
    }

private:
    std::unique_ptr<unsigned char[]> m_data;
    int m_width;
    int m_height;
    int m_channels;
};
