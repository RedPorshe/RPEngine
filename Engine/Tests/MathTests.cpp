#include <gtest/gtest.h>
#include "Math/MathTypes.h"
#include "Math/CE_MathHelpers.h"

using namespace CEMath;

// ============================================================================
// Math Helpers Tests
// ============================================================================

TEST(MathHelpersTest, IsZero)
{
    EXPECT_TRUE(IsZero(0.0f));
    EXPECT_TRUE(IsZero(0.0000001f));
    EXPECT_FALSE(IsZero(0.1f));
}

TEST(MathHelpersTest, IsEqual)
{
    EXPECT_TRUE(IsEqual(1.0f, 1.0f));
    EXPECT_TRUE(IsEqual(1.0f, 1.0000001f));
    EXPECT_FALSE(IsEqual(1.0f, 1.1f));
}

TEST(MathHelpersTest, Clamp)
{
    EXPECT_FLOAT_EQ(Clamp(5.0f, 0.0f, 10.0f), 5.0f);
    EXPECT_FLOAT_EQ(Clamp(-5.0f, 0.0f, 10.0f), 0.0f);
    EXPECT_FLOAT_EQ(Clamp(15.0f, 0.0f, 10.0f), 10.0f);
}

TEST(MathHelpersTest, Lerp)
{
    EXPECT_FLOAT_EQ(Lerp(0.0f, 10.0f, 0.5f), 5.0f);
    EXPECT_FLOAT_EQ(Lerp(0.0f, 10.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ(Lerp(0.0f, 10.0f, 1.0f), 10.0f);
}

TEST(MathHelpersTest, DegreesToRadians)
{
    EXPECT_NEAR(DegreesToRadians(0.0f), 0.0f, EPSILON);
    EXPECT_NEAR(DegreesToRadians(90.0f), HALF_PI, EPSILON);
    EXPECT_NEAR(DegreesToRadians(180.0f), PI, EPSILON);
    EXPECT_NEAR(DegreesToRadians(360.0f), TWO_PI, EPSILON);
}

TEST(MathHelpersTest, RadiansToDegrees)
{
    EXPECT_NEAR(RadiansToDegrees(0.0f), 0.0f, EPSILON);
    EXPECT_NEAR(RadiansToDegrees(HALF_PI), 90.0f, EPSILON);
    EXPECT_NEAR(RadiansToDegrees(PI), 180.0f, EPSILON);
    EXPECT_NEAR(RadiansToDegrees(TWO_PI), 360.0f, EPSILON);
}

TEST(MathHelpersTest, WrapAngle)
{
    EXPECT_NEAR(WrapAngle(0.0f), 0.0f, EPSILON);
    EXPECT_NEAR(WrapAngle(PI), -PI, EPSILON);  // PI wraps to -PI
    EXPECT_NEAR(WrapAngle(PI + 0.1f), -PI + 0.1f, EPSILON);
    EXPECT_NEAR(WrapAngle(-PI - 0.1f), PI - 0.1f, EPSILON);
    EXPECT_NEAR(WrapAngle(TWO_PI), 0.0f, EPSILON);  // 2PI wraps to 0
    EXPECT_NEAR(WrapAngle(-0.1f), -0.1f, EPSILON);
    EXPECT_NEAR(WrapAngle(3.0f), 3.0f, EPSILON);
    EXPECT_NEAR(WrapAngle(-3.0f), -3.0f, EPSILON);
}

TEST(MathHelpersTest, Sign)
{
    EXPECT_EQ(Sign(5.0f), 1);
    EXPECT_EQ(Sign(-5.0f), -1);
    EXPECT_EQ(Sign(0.0f), 0);
}

TEST(MathHelpersTest, SafeDivide)
{
    EXPECT_FLOAT_EQ(SafeDivide(10.0f, 2.0f), 5.0f);
    EXPECT_FLOAT_EQ(SafeDivide(10.0f, 0.0f), 0.0f);
    EXPECT_FLOAT_EQ(SafeDivide(10.0f, 0.0f, 999.0f), 999.0f);
}

TEST(MathHelpersTest, IsPowerOfTwo)
{
    EXPECT_TRUE(IsPowerOfTwo(1));
    EXPECT_TRUE(IsPowerOfTwo(2));
    EXPECT_TRUE(IsPowerOfTwo(4));
    EXPECT_TRUE(IsPowerOfTwo(8));
    EXPECT_TRUE(IsPowerOfTwo(16));

    EXPECT_FALSE(IsPowerOfTwo(0));
    EXPECT_FALSE(IsPowerOfTwo(3));
    EXPECT_FALSE(IsPowerOfTwo(5));
    EXPECT_FALSE(IsPowerOfTwo(6));
    EXPECT_FALSE(IsPowerOfTwo(7));
}

TEST(MathHelpersTest, NextPowerOfTwo)
{
    EXPECT_EQ(NextPowerOfTwo(1), 1);
    EXPECT_EQ(NextPowerOfTwo(2), 2);
    EXPECT_EQ(NextPowerOfTwo(3), 4);
    EXPECT_EQ(NextPowerOfTwo(5), 8);
    EXPECT_EQ(NextPowerOfTwo(12), 16);
    EXPECT_EQ(NextPowerOfTwo(0), 1);
    EXPECT_EQ(NextPowerOfTwo(-5), 1);
}

// ============================================================================
// Constants Tests
// ============================================================================

TEST(ConstantsTest, PIValues)
{
    EXPECT_NEAR(PI, 3.14159265358979323846f, 1e-6f);
    EXPECT_NEAR(TWO_PI, 6.28318530717958647692f, 1e-6f);
    EXPECT_NEAR(HALF_PI, 1.57079632679489661923f, 1e-6f);
    EXPECT_NEAR(QUARTER_PI, 0.785398163397448309616f, 1e-6f);
}

TEST(ConstantsTest, ConversionFactors)
{
    EXPECT_NEAR(DEG_TO_RAD, PI / 180.0f, 1e-6f);
    EXPECT_NEAR(RAD_TO_DEG, 180.0f / PI, 1e-6f);
}

TEST(ConstantsTest, Epsilon)
{
    EXPECT_GT(EPSILON, 0.0f);
    EXPECT_LE(EPSILON, 1e-6f);  // Используем <= вместо <
    // Или просто проверяем точное значение
    EXPECT_FLOAT_EQ(EPSILON, 1e-6f);
}