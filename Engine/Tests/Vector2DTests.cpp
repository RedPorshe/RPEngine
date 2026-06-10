#include <gtest/gtest.h>
#include "Math/MathTypes.h"
#include "Math/CE_MathHelpers.h"

using namespace CEMath;

// ============================================================================
// Vector2D Tests
// ============================================================================

TEST(Vector2DTest, ConstructionAndAccess)
{
    Vector2D v1;
    EXPECT_FLOAT_EQ(v1.x, 0.0f);
    EXPECT_FLOAT_EQ(v1.y, 0.0f);

    Vector2D v2(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v2.x, 3.0f);
    EXPECT_FLOAT_EQ(v2.y, 4.0f);

    Vector2D v3(5.0f);
    EXPECT_FLOAT_EQ(v3.x, 5.0f);
    EXPECT_FLOAT_EQ(v3.y, 5.0f);
}

TEST(Vector2DTest, ArithmeticOperations)
{
    Vector2D a(2.0f, 3.0f);
    Vector2D b(4.0f, 5.0f);

    Vector2D sum = a + b;
    EXPECT_FLOAT_EQ(sum.x, 6.0f);
    EXPECT_FLOAT_EQ(sum.y, 8.0f);

    Vector2D diff = a - b;
    EXPECT_FLOAT_EQ(diff.x, -2.0f);
    EXPECT_FLOAT_EQ(diff.y, -2.0f);

    Vector2D scaled = a * 2.0f;
    EXPECT_FLOAT_EQ(scaled.x, 4.0f);
    EXPECT_FLOAT_EQ(scaled.y, 6.0f);
}

TEST(Vector2DTest, LengthAndNormalize)
{
    Vector2D v(3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v.Length(), 5.0f);
    EXPECT_FLOAT_EQ(v.LengthSquared(), 25.0f);

    Vector2D normalized = v.Normalized();
    EXPECT_FLOAT_EQ(normalized.x, 0.6f);
    EXPECT_FLOAT_EQ(normalized.y, 0.8f);
    EXPECT_FLOAT_EQ(normalized.Length(), 1.0f);
}

TEST(Vector2DTest, DotAndCross)
{
    Vector2D a(2.0f, 3.0f);
    Vector2D b(4.0f, 5.0f);

    EXPECT_FLOAT_EQ(a.Dot(b), 2.0f * 4.0f + 3.0f * 5.0f);
    EXPECT_FLOAT_EQ(a.Cross(b), 2.0f * 5.0f - 3.0f * 4.0f);
}

TEST(Vector2DTest, Lerp)
{
    Vector2D a(0.0f, 0.0f);
    Vector2D b(10.0f, 10.0f);

    Vector2D mid = Vector2D::Lerp(a, b, 0.5f);
    EXPECT_FLOAT_EQ(mid.x, 5.0f);
    EXPECT_FLOAT_EQ(mid.y, 5.0f);
}