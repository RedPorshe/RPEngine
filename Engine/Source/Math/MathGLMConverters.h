#pragma once
#include "Math/MathTypes.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/transform.hpp>

namespace CEMath
{

inline glm::vec2 ToGLM(const Vector2D& v)
{
    return glm::vec2(v.x, v.y);
}
inline glm::vec3 ToGLM(const Vector3D& v)
{
    return glm::vec3(v.x, v.y, v.z);
}
inline glm::vec4 ToGLM(const Vector4D& v)
{
    return glm::vec4(v.x, v.y, v.z, v.w);
}

inline glm::quat ToGLM(const Quaternion& q)
{
    return glm::quat(q.w, q.x, q.y, q.z);
}

inline glm::mat4 ToGLM(const Matrix4x4& m)
{

    return glm::make_mat4(m.m);
}

inline Vector2D FromGLM(const glm::vec2& v)
{
    return Vector2D(v.x, v.y);
}
inline Vector3D FromGLM(const glm::vec3& v)
{
    return Vector3D(v.x, v.y, v.z);
}
inline Vector4D FromGLM(const glm::vec4& v)
{
    return Vector4D(v.x, v.y, v.z, v.w);
}

inline Quaternion FromGLM(const glm::quat& q)
{
    return Quaternion(q.x, q.y, q.z, q.w);
}

inline Matrix4x4 FromGLM(const glm::mat4& m)
{

    const float* glmData = glm::value_ptr(m);
    return Matrix4x4(glmData);
}

inline glm::mat4 ToGLMMatrix(const FTransform& t)
{
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), ToGLM(t.Location));
    glm::mat4 rotation = glm::mat4_cast(ToGLM(t.Rotation));
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), ToGLM(t.Scale));

    return translation * rotation * scale;
}

inline FTransform FromGLMMatrix(const glm::mat4& m)
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(m, scale, rotation, translation, skew, perspective);

    return FTransform(FromGLM(translation), FromGLM(rotation), FromGLM(scale));
}

inline FTransform FromGLMMatrixSimple(const glm::mat4& m)
{
    FTransform result;

    result.Location = FromGLM(glm::vec3(m[3]));

    glm::vec3 scaleX = glm::vec3(m[0]);
    glm::vec3 scaleY = glm::vec3(m[1]);
    glm::vec3 scaleZ = glm::vec3(m[2]);

    result.Scale = Vector3D(glm::length(scaleX), glm::length(scaleY), glm::length(scaleZ));

    glm::mat3 rotationMat(1.0f);

    if (result.Scale.x > 0.0f)
        rotationMat[0] = scaleX / result.Scale.x;
    else
        rotationMat[0] = glm::vec3(1.0f, 0.0f, 0.0f);

    if (result.Scale.y > 0.0f)
        rotationMat[1] = scaleY / result.Scale.y;
    else
        rotationMat[1] = glm::vec3(0.0f, 1.0f, 0.0f);

    if (result.Scale.z > 0.0f)
        rotationMat[2] = scaleZ / result.Scale.z;
    else
        rotationMat[2] = glm::vec3(0.0f, 0.0f, 1.0f);

    result.Rotation = FromGLM(glm::quat_cast(rotationMat));

    return result;
}
}  // namespace CEMath