
#define GLM_ENABLE_EXPERIMENTAL
#include <gtest/gtest.h>
#include "Math/MathGLMConverters.h"

using namespace CEMath;

// ============================================================================
// GLM Converters Tests
// ============================================================================

// ============================================================================
// Vector2D Tests
// ============================================================================

TEST(GLMConvertersTest, Vector2DToGLM)
{
    Vector2D v(1.5f, 2.5f);
    glm::vec2 glmVec = ToGLM(v);

    EXPECT_FLOAT_EQ(glmVec.x, 1.5f);
    EXPECT_FLOAT_EQ(glmVec.y, 2.5f);
}

TEST(GLMConvertersTest, Vector2DFromGLM)
{
    glm::vec2 glmVec(3.5f, 4.5f);
    Vector2D v = FromGLM(glmVec);

    EXPECT_FLOAT_EQ(v.x, 3.5f);
    EXPECT_FLOAT_EQ(v.y, 4.5f);
}

TEST(GLMConvertersTest, Vector2DRoundTrip)
{
    Vector2D original(1.0f, 2.0f);
    Vector2D converted = FromGLM(ToGLM(original));

    EXPECT_EQ(original, converted);
}

TEST(GLMConvertersTest, Vector2DZeroVector)
{
    Vector2D original(0.0f, 0.0f);
    Vector2D converted = FromGLM(ToGLM(original));

    EXPECT_EQ(original, converted);
}

// ============================================================================
// Vector3D Tests
// ============================================================================

TEST(GLMConvertersTest, Vector3DToGLM)
{
    Vector3D v(1.5f, 2.5f, 3.5f);
    glm::vec3 glmVec = ToGLM(v);

    EXPECT_FLOAT_EQ(glmVec.x, 1.5f);
    EXPECT_FLOAT_EQ(glmVec.y, 2.5f);
    EXPECT_FLOAT_EQ(glmVec.z, 3.5f);
}

TEST(GLMConvertersTest, Vector3DFromGLM)
{
    glm::vec3 glmVec(3.5f, 4.5f, 5.5f);
    Vector3D v = FromGLM(glmVec);

    EXPECT_FLOAT_EQ(v.x, 3.5f);
    EXPECT_FLOAT_EQ(v.y, 4.5f);
    EXPECT_FLOAT_EQ(v.z, 5.5f);
}

TEST(GLMConvertersTest, Vector3DRoundTrip)
{
    Vector3D original(1.0f, 2.0f, 3.0f);
    Vector3D converted = FromGLM(ToGLM(original));

    EXPECT_EQ(original, converted);
}

TEST(GLMConvertersTest, Vector3DZeroVector)
{
    Vector3D original(0.0f, 0.0f, 0.0f);
    Vector3D converted = FromGLM(ToGLM(original));

    EXPECT_EQ(original, converted);
}

TEST(GLMConvertersTest, Vector3DNegativeValues)
{
    Vector3D original(-1.0f, -2.0f, -3.0f);
    Vector3D converted = FromGLM(ToGLM(original));

    EXPECT_EQ(original, converted);
}

// ============================================================================
// Vector4D Tests
// ============================================================================

TEST(GLMConvertersTest, Vector4DToGLM)
{
    Vector4D v(1.5f, 2.5f, 3.5f, 4.5f);
    glm::vec4 glmVec = ToGLM(v);

    EXPECT_FLOAT_EQ(glmVec.x, 1.5f);
    EXPECT_FLOAT_EQ(glmVec.y, 2.5f);
    EXPECT_FLOAT_EQ(glmVec.z, 3.5f);
    EXPECT_FLOAT_EQ(glmVec.w, 4.5f);
}

TEST(GLMConvertersTest, Vector4DFromGLM)
{
    glm::vec4 glmVec(3.5f, 4.5f, 5.5f, 6.5f);
    Vector4D v = FromGLM(glmVec);

    EXPECT_FLOAT_EQ(v.x, 3.5f);
    EXPECT_FLOAT_EQ(v.y, 4.5f);
    EXPECT_FLOAT_EQ(v.z, 5.5f);
    EXPECT_FLOAT_EQ(v.w, 6.5f);
}

TEST(GLMConvertersTest, Vector4DRoundTrip)
{
    Vector4D original(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4D converted = FromGLM(ToGLM(original));

    EXPECT_EQ(original, converted);
}

TEST(GLMConvertersTest, Vector4DZeroVector)
{
    Vector4D original(0.0f, 0.0f, 0.0f, 0.0f);
    Vector4D converted = FromGLM(ToGLM(original));

    EXPECT_EQ(original, converted);
}

TEST(GLMConvertersTest, Vector4DHomogeneous)
{
    Vector4D original(1.0f, 2.0f, 3.0f, 1.0f);
    Vector4D converted = FromGLM(ToGLM(original));

    EXPECT_EQ(original, converted);
}

// ============================================================================
// Quaternion Tests
// ============================================================================

TEST(GLMConvertersTest, QuaternionToGLM)
{
    Quaternion q(0.1f, 0.2f, 0.3f, 0.4f);
    q.Normalize();

    glm::quat glmQuat = ToGLM(q);

    EXPECT_NEAR(glmQuat.w, q.w, EPSILON);
    EXPECT_NEAR(glmQuat.x, q.x, EPSILON);
    EXPECT_NEAR(glmQuat.y, q.y, EPSILON);
    EXPECT_NEAR(glmQuat.z, q.z, EPSILON);
}

TEST(GLMConvertersTest, QuaternionFromGLM)
{
    glm::quat glmQuat(0.5f, 0.5f, 0.5f, 0.5f);
    glmQuat = glm::normalize(glmQuat);

    Quaternion q = FromGLM(glmQuat);

    EXPECT_NEAR(glmQuat.w, q.w, EPSILON);
    EXPECT_NEAR(glmQuat.x, q.x, EPSILON);
    EXPECT_NEAR(glmQuat.y, q.y, EPSILON);
    EXPECT_NEAR(glmQuat.z, q.z, EPSILON);
}

TEST(GLMConvertersTest, QuaternionRoundTrip)
{
    Quaternion original(Vector3D::UnitZ(), HALF_PI);
    Quaternion converted = FromGLM(ToGLM(original));

    EXPECT_TRUE(original.IsEqual(converted, 1e-5f));
}

TEST(GLMConvertersTest, QuaternionIdentity)
{
    Quaternion original = Quaternion::Identity();
    Quaternion converted = FromGLM(ToGLM(original));

    EXPECT_TRUE(converted.IsEqual(Quaternion::Identity(), 1e-5f));
}

TEST(GLMConvertersTest, QuaternionAxisAngle)
{
    Vector3D axis = Vector3D(1.0f, 1.0f, 0.0f).Normalized();
    float angle = PI / 3.0f;

    Quaternion original(axis, angle);
    Quaternion converted = FromGLM(ToGLM(original));

    EXPECT_NEAR(original.x, converted.x, 1e-5f);
    EXPECT_NEAR(original.y, converted.y, 1e-5f);
    EXPECT_NEAR(original.z, converted.z, 1e-5f);
    EXPECT_NEAR(original.w, converted.w, 1e-5f);
}

// ============================================================================
// Matrix4x4 Tests
// ============================================================================

TEST(GLMConvertersTest, Matrix4x4ToGLM)
{
    Matrix4x4 m = Matrix4x4::Translation(Vector3D(1.0f, 2.0f, 3.0f));
    glm::mat4 glmMat = ToGLM(m);

    // В GLM column-major, translation в [3][0], [3][1], [3][2]
    EXPECT_FLOAT_EQ(glmMat[3][0], 1.0f);
    EXPECT_FLOAT_EQ(glmMat[3][1], 2.0f);
    EXPECT_FLOAT_EQ(glmMat[3][2], 3.0f);
    EXPECT_FLOAT_EQ(glmMat[3][3], 1.0f);
}

TEST(GLMConvertersTest, Matrix4x4FromGLM)
{
    glm::mat4 glmMat = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    Matrix4x4 m = FromGLM(glmMat);

    Vector3D translation = m.GetTranslation();
    EXPECT_FLOAT_EQ(translation.x, 1.0f);
    EXPECT_FLOAT_EQ(translation.y, 2.0f);
    EXPECT_FLOAT_EQ(translation.z, 3.0f);
}

TEST(GLMConvertersTest, Matrix4x4RoundTrip)
{
    Matrix4x4 original =
        Matrix4x4::Translation(Vector3D(1.0f, 2.0f, 3.0f)) * Matrix4x4::RotationZ(HALF_PI) * Matrix4x4::Scaling(Vector3D(2.0f, 2.0f, 2.0f));

    Matrix4x4 converted = FromGLM(ToGLM(original));

    for (int i = 0; i < 16; ++i)
    {
        EXPECT_NEAR(original.m[i], converted.m[i], 1e-5f);
    }
}

TEST(GLMConvertersTest, Matrix4x4Identity)
{
    Matrix4x4 original = Matrix4x4::IdentityMatrix();
    Matrix4x4 converted = FromGLM(ToGLM(original));

    EXPECT_TRUE(converted.IsIdentity());
}

TEST(GLMConvertersTest, Matrix4x4Translation)
{
    Matrix4x4 original = Matrix4x4::Translation(Vector3D(5.0f, 6.0f, 7.0f));
    Matrix4x4 converted = FromGLM(ToGLM(original));

    Vector3D trans1 = original.GetTranslation();
    Vector3D trans2 = converted.GetTranslation();

    EXPECT_FLOAT_EQ(trans1.x, trans2.x);
    EXPECT_FLOAT_EQ(trans1.y, trans2.y);
    EXPECT_FLOAT_EQ(trans1.z, trans2.z);
}

TEST(GLMConvertersTest, Matrix4x4Rotation)
{
    Matrix4x4 original = Matrix4x4::RotationX(HALF_PI) * Matrix4x4::RotationY(QUARTER_PI);
    Matrix4x4 converted = FromGLM(ToGLM(original));

    Vector3D point(1.0f, 0.0f, 0.0f);
    Vector3D rotated1 = original * point;
    Vector3D rotated2 = converted * point;

    EXPECT_NEAR(rotated1.x, rotated2.x, 1e-5f);
    EXPECT_NEAR(rotated1.y, rotated2.y, 1e-5f);
    EXPECT_NEAR(rotated1.z, rotated2.z, 1e-5f);
}

TEST(GLMConvertersTest, Matrix4x4Scaling)
{
    Matrix4x4 original = Matrix4x4::Scaling(Vector3D(2.0f, 3.0f, 4.0f));
    Matrix4x4 converted = FromGLM(ToGLM(original));

    Vector3D point(1.0f, 1.0f, 1.0f);
    Vector3D scaled1 = original * point;
    Vector3D scaled2 = converted * point;

    EXPECT_FLOAT_EQ(scaled1.x, scaled2.x);
    EXPECT_FLOAT_EQ(scaled1.y, scaled2.y);
    EXPECT_FLOAT_EQ(scaled1.z, scaled2.z);
}

// ============================================================================
// Mixed Type Tests
// ============================================================================

TEST(GLMConvertersTest, CombineVectorAndQuaternion)
{
    Quaternion q(Vector3D::UnitZ(), HALF_PI);
    Vector3D v(1.0f, 0.0f, 0.0f);

    // Вращаем вектор через Quaternion
    Vector3D rotated1 = q * v;

    // Конвертируем в GLM, вращаем, конвертируем обратно
    glm::quat glmQ = ToGLM(q);
    glm::vec3 glmV = ToGLM(v);
    glm::vec3 glmRotated = glmQ * glmV;
    Vector3D rotated2 = FromGLM(glmRotated);

    EXPECT_NEAR(rotated1.x, rotated2.x, EPSILON);
    EXPECT_NEAR(rotated1.y, rotated2.y, EPSILON);
    EXPECT_NEAR(rotated1.z, rotated2.z, EPSILON);
}

TEST(GLMConvertersTest, CombineMatrixAndVector)
{
    Matrix4x4 m = Matrix4x4::Translation(Vector3D(1.0f, 2.0f, 3.0f)) * Matrix4x4::RotationZ(HALF_PI);
    Vector3D v(1.0f, 0.0f, 0.0f);

    // Трансформируем вектор через нашу матрицу
    Vector3D transformed1 = m * v;

    // Конвертируем в GLM, трансформируем, конвертируем обратно
    glm::mat4 glmM = ToGLM(m);
    glm::vec3 glmV = ToGLM(v);
    glm::vec3 glmTransformed = glmM * glm::vec4(glmV, 1.0f);
    Vector3D transformed2 = FromGLM(glmTransformed);

    EXPECT_NEAR(transformed1.x, transformed2.x, EPSILON);
    EXPECT_NEAR(transformed1.y, transformed2.y, EPSILON);
    EXPECT_NEAR(transformed1.z, transformed2.z, EPSILON);
}