#include <gtest/gtest.h>
#include "Math/MathTypes.h"
#include "Math/CE_MathHelpers.h"

using namespace CEMath;

// ============================================================================
// Matrix4x4 Tests
// ============================================================================

TEST(Matrix4x4Test, Identity)
{
    Matrix4x4 m;
    EXPECT_TRUE(m.IsIdentity());
    EXPECT_FLOAT_EQ(m(0, 0), 1.0f);
    EXPECT_FLOAT_EQ(m(1, 1), 1.0f);
    EXPECT_FLOAT_EQ(m(2, 2), 1.0f);
    EXPECT_FLOAT_EQ(m(3, 3), 1.0f);
}

TEST(Matrix4x4Test, Translation)
{
    Vector3D translation(3.0f, 4.0f, 5.0f);
    Matrix4x4 m = Matrix4x4::Translation(translation);

    // Используем GetTranslation() вместо прямого доступа
    Vector3D t = m.GetTranslation();
    EXPECT_FLOAT_EQ(t.x, 3.0f);
    EXPECT_FLOAT_EQ(t.y, 4.0f);
    EXPECT_FLOAT_EQ(t.z, 5.0f);

    Vector3D point(1.0f, 2.0f, 3.0f);
    Vector3D transformed = m * point;
    EXPECT_FLOAT_EQ(transformed.x, 4.0f);
    EXPECT_FLOAT_EQ(transformed.y, 6.0f);
    EXPECT_FLOAT_EQ(transformed.z, 8.0f);
}

TEST(Matrix4x4Test, Scaling)
{
    Vector3D scale(2.0f, 3.0f, 4.0f);
    Matrix4x4 m = Matrix4x4::Scaling(scale);

    Vector3D point(1.0f, 1.0f, 1.0f);
    Vector3D transformed = m * point;
    EXPECT_FLOAT_EQ(transformed.x, 2.0f);
    EXPECT_FLOAT_EQ(transformed.y, 3.0f);
    EXPECT_FLOAT_EQ(transformed.z, 4.0f);
}

TEST(Matrix4x4Test, RotationX)
{
    Matrix4x4 m = Matrix4x4::RotationX(HALF_PI);
    Vector3D point(0.0f, 1.0f, 0.0f);
    Vector3D rotated = m * point;

    EXPECT_NEAR(rotated.x, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.y, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.z, 1.0f, EPSILON);
}

TEST(Matrix4x4Test, RotationY)
{
    Matrix4x4 m = Matrix4x4::RotationY(HALF_PI);
    Vector3D point(1.0f, 0.0f, 0.0f);
    Vector3D rotated = m * point;

    EXPECT_NEAR(rotated.x, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.y, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.z, -1.0f, EPSILON);
}

TEST(Matrix4x4Test, RotationZ)
{
    Matrix4x4 m = Matrix4x4::RotationZ(HALF_PI);
    Vector3D point(1.0f, 0.0f, 0.0f);
    Vector3D rotated = m * point;

    EXPECT_NEAR(rotated.x, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.y, 1.0f, EPSILON);
    EXPECT_NEAR(rotated.z, 0.0f, EPSILON);
}

TEST(Matrix4x4Test, MatrixMultiplication)
{
    Matrix4x4 m1 = Matrix4x4::Translation(Vector3D(1.0f, 2.0f, 3.0f));
    Matrix4x4 m2 = Matrix4x4::Scaling(Vector3D(2.0f, 2.0f, 2.0f));
    Matrix4x4 m3 = m1 * m2;

    Vector3D point(1.0f, 1.0f, 1.0f);
    Vector3D transformed = m3 * point;

    EXPECT_FLOAT_EQ(transformed.x, 3.0f);
    EXPECT_FLOAT_EQ(transformed.y, 4.0f);
    EXPECT_FLOAT_EQ(transformed.z, 5.0f);
}

TEST(Matrix4x4Test, Inverse)
{
    Matrix4x4 m = Matrix4x4::Translation(Vector3D(5.0f, 6.0f, 7.0f));
    Matrix4x4 inv = m.Inversed();

    Vector3D point(10.0f, 20.0f, 30.0f);
    Vector3D translated = m * point;
    Vector3D restored = inv * translated;

    EXPECT_NEAR(restored.x, point.x, EPSILON);
    EXPECT_NEAR(restored.y, point.y, EPSILON);
    EXPECT_NEAR(restored.z, point.z, EPSILON);
}

TEST(Matrix4x4Test, Determinant)
{
    Matrix4x4 m;
    EXPECT_FLOAT_EQ(m.Determinant(), 1.0f);

    Matrix4x4 m2 = Matrix4x4::Scaling(2.0f, 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(m2.Determinant(), 24.0f);
}

TEST(Matrix4x4Test, Transpose)
{
    Matrix4x4 m(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f);

    Matrix4x4 transposed = m.Transposed();

    EXPECT_FLOAT_EQ(transposed(0, 1), m(1, 0));
    EXPECT_FLOAT_EQ(transposed(1, 0), m(0, 1));
    EXPECT_FLOAT_EQ(transposed(2, 3), m(3, 2));
}

TEST(Matrix4x4Test, ComparisonOperators)
{
    Matrix4x4 m1;
    Matrix4x4 m2;
    Matrix4x4 m3 = Matrix4x4::Translation(Vector3D(1.0f, 0.0f, 0.0f));

    EXPECT_EQ(m1, m2);
    EXPECT_NE(m1, m3);
}
