#include <gtest/gtest.h>
#include "Math/MathTypes.h"
#include "Math/CE_MathHelpers.h"

using namespace CEMath;

// ============================================================================
// Vector4D Tests
// ============================================================================

TEST(Vector4DTest, ConstructionAndAccess)
{
    Vector4D v1;
    EXPECT_FLOAT_EQ(v1.x, 0.0f);
    EXPECT_FLOAT_EQ(v1.y, 0.0f);
    EXPECT_FLOAT_EQ(v1.z, 0.0f);
    EXPECT_FLOAT_EQ(v1.w, 0.0f);

    Vector4D v2(1.0f, 2.0f, 3.0f, 4.0f);
    EXPECT_FLOAT_EQ(v2.x, 1.0f);
    EXPECT_FLOAT_EQ(v2.y, 2.0f);
    EXPECT_FLOAT_EQ(v2.z, 3.0f);
    EXPECT_FLOAT_EQ(v2.w, 4.0f);
}

TEST(Vector4DTest, ToCartesian)
{
    Vector4D homogeneous(2.0f, 4.0f, 6.0f, 2.0f);
    Vector3D cartesian = homogeneous.ToCartesian();

    EXPECT_FLOAT_EQ(cartesian.x, 1.0f);
    EXPECT_FLOAT_EQ(cartesian.y, 2.0f);
    EXPECT_FLOAT_EQ(cartesian.z, 3.0f);
}

TEST(Vector4DTest, DotAndLength)
{
    Vector4D a(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4D b(5.0f, 6.0f, 7.0f, 8.0f);

    EXPECT_FLOAT_EQ(a.Dot(b), 1.0f * 5.0f + 2.0f * 6.0f + 3.0f * 7.0f + 4.0f * 8.0f);
    EXPECT_FLOAT_EQ(a.LengthSquared(), 1.0f + 4.0f + 9.0f + 16.0f);
}