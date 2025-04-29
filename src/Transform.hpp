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
    TransformChain(const Transform& transform) noexcept;
    TransformChain(TransformChain&&) noexcept = default;
    TransformChain(const TransformChain&) noexcept = default;
    ~TransformChain() = default;

    TransformChain& operator=(TransformChain&&) noexcept = default;
    TransformChain& operator=(const TransformChain&) noexcept = default;

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
        return *this;
    }

    /// Sets the rotation.
    Transform& with_rotation(glm::quat rotation) noexcept
    {
        this->m_rotation = rotation;
        return *this;
    }

    /// Starts a transform chain.
    TransformChain transform() const noexcept
    {
        return TransformChain { *this };
    }

    /// Returns the transformation matrix.
    operator glm::mat4() const noexcept
    {
        glm::mat4 matrix = glm::translate(glm::identity<glm::mat4>(), this->m_translation);
        matrix *= glm::mat4_cast(this->m_rotation);
        matrix = glm::scale(matrix, glm::vec3 { this->m_scale });
        return matrix;
    }

private:
    glm::vec3 m_translation;
    float m_scale;
    glm::quat m_rotation;
};

inline TransformChain::TransformChain(const Transform& transform) noexcept
    : m_matrix { static_cast<glm::mat4>(transform) }
{
}

inline TransformChain& TransformChain::with_parent(const Transform& parent) noexcept
{
    this->m_matrix = static_cast<glm::mat4>(parent) * this->m_matrix;
    return *this;
};
