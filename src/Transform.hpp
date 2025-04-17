#pragma once

#ifdef _MSVC_VER
#pragma warning(push, 3)
#endif
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#ifdef _MSVC_VER
#pragma warning(pop)
#endif

#include <optional>

class Transform;

class TransformChain {
public:
    TransformChain() noexcept
        : m_matrix { glm::identity<glm::mat4>() }
    {
    }
    TransformChain(Transform& transform) noexcept;
    TransformChain(const Transform& transform) noexcept;
    TransformChain(TransformChain&&) noexcept = default;
    TransformChain(const TransformChain&) noexcept = default;
    ~TransformChain() = default;

    TransformChain& operator=(TransformChain&&) noexcept = default;
    TransformChain& operator=(const TransformChain&) noexcept = default;

    TransformChain& with_parent(Transform& parent) noexcept;
    TransformChain& with_parent(const Transform& parent) noexcept;

    operator glm::mat4() const noexcept { return this->m_matrix; }

private:
    glm::mat4 m_matrix;
};

class Transform {
public:
    Transform() noexcept
        : m_translation { 0.0f }
        , m_scale { 1.0f }
        , m_rotation { glm::identity<glm::quat>() }
        , m_post_transform { std::nullopt }
        , m_global_cache { std::nullopt }
        , m_local_cache { std::nullopt }
    {
    }

    Transform(Transform&&) noexcept = default;
    Transform(const Transform&) noexcept = default;
    ~Transform() = default;

    Transform& operator=(Transform&&) noexcept = default;
    Transform& operator=(const Transform&) noexcept = default;

    /// Returns the translation.
    glm::vec3 translation() const noexcept
    {
        return this->m_translation;
    }

    /// Sets the translation vector.
    Transform& with_translation(glm::vec3 translation) noexcept
    {
        this->m_translation = translation;
        this->m_global_cache = std::nullopt;
        this->m_local_cache = std::nullopt;
        return *this;
    }

    /// Returns the uniform scaling.
    float scale() const noexcept
    {
        return this->m_scale;
    }

    /// Sets the uniform scaling.
    Transform& with_scale(float scale) noexcept
    {
        this->m_scale = scale;
        this->m_global_cache = std::nullopt;
        this->m_local_cache = std::nullopt;
        return *this;
    }

    /// Returns the rotation with pitch in x, yaw in y, and roll in z, in radians.
    glm::vec3 euler_angles() const noexcept
    {
        return glm::eulerAngles(this->m_rotation);
    }

    /// Returns the rotation as a quaternion.
    glm::quat rotation() const noexcept
    {
        return this->m_rotation;
    }

    /// Sets the rotation from euler angles in radians.
    Transform& with_rotation_euler(glm::vec3 rotation) noexcept
    {
        this->m_rotation = glm::quat { rotation };
        this->m_global_cache = std::nullopt;
        this->m_local_cache = std::nullopt;
        return *this;
    }

    /// Sets the rotation.
    Transform& with_rotation(glm::quat rotation) noexcept
    {
        this->m_rotation = rotation;
        this->m_global_cache = std::nullopt;
        this->m_local_cache = std::nullopt;
        return *this;
    }

    /// Returns the optional post-transform matrix.
    std::optional<glm::mat4> post_transform_matrix() const noexcept
    {
        return this->m_post_transform;
    }

    /// Sets the post transform matrix.
    Transform& with_post_transform_matrix(std::optional<glm::mat4> matrix) noexcept
    {
        this->m_post_transform = matrix;
        this->m_local_cache = std::nullopt;
        return *this;
    }

    /// Returns the global transformation matrix.
    glm::mat4 global_matrix() const noexcept
    {
        if (this->m_global_cache.has_value()) {
            return *this->m_global_cache;
        } else {
            return this->compute_global_matrix();
        }
    }

    /// Returns the global transformation matrix.
    ///
    /// May cache the matrix for faster lookup.
    glm::mat4 global_matrix() noexcept
    {
        if (this->m_global_cache.has_value()) {
            return *this->m_global_cache;
        } else {
            auto matrix = this->compute_global_matrix();
            this->m_global_cache = matrix;
            return matrix;
        }
    }

    /// Returns the local transformation matrix.
    glm::mat4 local_matrix() const noexcept
    {
        if (this->m_local_cache.has_value()) {
            return *this->m_local_cache;
        } else {
            auto matrix = this->global_matrix();
            if (this->m_post_transform.has_value()) {
                return matrix * *this->m_post_transform;
            } else {
                return matrix;
            }
        }
    }

    /// Returns the local transformation matrix.
    ///
    /// May cache the matrix for faster lookup.
    glm::mat4 local_matrix() noexcept
    {
        if (this->m_local_cache.has_value()) {
            return *this->m_local_cache;
        } else {
            auto matrix = this->global_matrix();
            if (this->m_post_transform.has_value()) {
                matrix *= *this->m_post_transform;
            }
            this->m_local_cache = matrix;
            return matrix;
        }
    }

    /// Starts a transform chain.
    TransformChain transform() const noexcept
    {
        return TransformChain { *this };
    }

    /// Starts a transform chain.
    TransformChain transform() noexcept
    {
        return TransformChain { *this };
    }

private:
    glm::mat4 compute_global_matrix() const noexcept
    {
        glm::mat4 matrix = glm::translate(glm::identity<glm::mat4>(), this->m_translation);
        matrix *= glm::mat4_cast(this->m_rotation);
        matrix = glm::scale(matrix, glm::vec3 { this->m_scale });
        return matrix;
    }

    glm::vec3 m_translation;
    float m_scale;
    glm::quat m_rotation;
    std::optional<glm::mat4> m_post_transform;

    std::optional<glm::mat4> m_global_cache;
    std::optional<glm::mat4> m_local_cache;
};

inline TransformChain::TransformChain(Transform& transform) noexcept
    : m_matrix { transform.local_matrix() }
{
}

inline TransformChain::TransformChain(const Transform& transform) noexcept
    : m_matrix { transform.local_matrix() }
{
}

inline TransformChain& TransformChain::with_parent(Transform& parent) noexcept
{
    this->m_matrix = parent.global_matrix() * this->m_matrix;
    return *this;
};

inline TransformChain& TransformChain::with_parent(const Transform& parent) noexcept
{
    this->m_matrix = parent.global_matrix() * this->m_matrix;
    return *this;
};
