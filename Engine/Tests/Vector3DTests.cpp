#include <gtest/gtest.h>
#include "Math/MathTypes.h"
#include "Math/CE_MathHelpers.h"

using namespace CEMath;

// ============================================================================
// Vector3D Tests
// ============================================================================

TEST(Vector3DTest, ConstructionAndAccess)
{
    Vector3D v1;
    EXPECT_FLOAT_EQ(v1.x, 0.0f);
    EXPECT_FLOAT_EQ(v1.y, 0.0f);
    EXPECT_FLOAT_EQ(v1.z, 0.0f);

    Vector3D v2(1.0f, 2.0f, 3.0f);
    EXPECT_FLOAT_EQ(v2.x, 1.0f);
    EXPECT_FLOAT_EQ(v2.y, 2.0f);
    EXPECT_FLOAT_EQ(v2.z, 3.0f);

    Vector3D v3(5.0f);
    EXPECT_FLOAT_EQ(v3.x, 5.0f);
    EXPECT_FLOAT_EQ(v3.y, 5.0f);
    EXPECT_FLOAT_EQ(v3.z, 5.0f);
}

TEST(Vector3DTest, ArithmeticOperations)
{
    Vector3D a(1.0f, 2.0f, 3.0f);
    Vector3D b(4.0f, 5.0f, 6.0f);

    Vector3D sum = a + b;
    EXPECT_FLOAT_EQ(sum.x, 5.0f);
    EXPECT_FLOAT_EQ(sum.y, 7.0f);
    EXPECT_FLOAT_EQ(sum.z, 9.0f);

    Vector3D scaled = a * 2.0f;
    EXPECT_FLOAT_EQ(scaled.x, 2.0f);
    EXPECT_FLOAT_EQ(scaled.y, 4.0f);
    EXPECT_FLOAT_EQ(scaled.z, 6.0f);
}

TEST(Vector3DTest, LengthAndNormalize)
{
    Vector3D v(3.0f, 4.0f, 0.0f);
    EXPECT_FLOAT_EQ(v.Length(), 5.0f);

    Vector3D v2(1.0f, 2.0f, 3.0f);
    Vector3D normalized = v2.Normalized();
    EXPECT_FLOAT_EQ(normalized.Length(), 1.0f);
}

TEST(Vector3DTest, DotAndCross)
{
    Vector3D a(1.0f, 2.0f, 3.0f);
    Vector3D b(4.0f, 5.0f, 6.0f);

    EXPECT_FLOAT_EQ(a.Dot(b), 1.0f * 4.0f + 2.0f * 5.0f + 3.0f * 6.0f);

    Vector3D cross = a.Cross(b);
    EXPECT_FLOAT_EQ(cross.x, 2.0f * 6.0f - 3.0f * 5.0f);
    EXPECT_FLOAT_EQ(cross.y, 3.0f * 4.0f - 1.0f * 6.0f);
    EXPECT_FLOAT_EQ(cross.z, 1.0f * 5.0f - 2.0f * 4.0f);
}

TEST(Vector3DTest, AngleBetweenVectors)
{
    Vector3D a(1.0f, 0.0f, 0.0f);
    Vector3D b(0.0f, 1.0f, 0.0f);

    float angle = Vector3D::Angle(a, b);
    EXPECT_FLOAT_EQ(angle, HALF_PI);
}

TEST(Vector3DTest, Lerp)
{
    Vector3D a(0.0f, 0.0f, 0.0f);
    Vector3D b(10.0f, 10.0f, 10.0f);

    Vector3D mid = Vector3D::Lerp(a, b, 0.5f);
    EXPECT_FLOAT_EQ(mid.x, 5.0f);
    EXPECT_FLOAT_EQ(mid.y, 5.0f);
    EXPECT_FLOAT_EQ(mid.z, 5.0f);
}

TEST(Vector3DTest, ComparisonOperators)
{
    Vector3D v1(1.0f, 2.0f, 3.0f);
    Vector3D v2(1.0f, 2.0f, 3.0f);
    Vector3D v3(1.0f, 2.0f, 4.0f);

    EXPECT_EQ(v1, v2);
    EXPECT_NE(v1, v3);
}
