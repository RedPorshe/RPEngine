#include <gtest/gtest.h>
#include "Math/MathTypes.h"
#include "Math/CE_MathHelpers.h"

using namespace CEMath;

// ============================================================================
// Quaternion Tests
// ============================================================================

TEST(QuaternionTest, Identity)
{
    Quaternion q;
    EXPECT_FLOAT_EQ(q.x, 0.0f);
    EXPECT_FLOAT_EQ(q.y, 0.0f);
    EXPECT_FLOAT_EQ(q.z, 0.0f);
    EXPECT_FLOAT_EQ(q.w, 1.0f);

    Quaternion identity = Quaternion::Identity();
    EXPECT_FLOAT_EQ(identity.x, 0.0f);
    EXPECT_FLOAT_EQ(identity.y, 0.0f);
    EXPECT_FLOAT_EQ(identity.z, 0.0f);
    EXPECT_FLOAT_EQ(identity.w, 1.0f);
}

TEST(QuaternionTest, FromAxisAngle)
{
    Vector3D axis(0.0f, 0.0f, 1.0f);
    float angle = HALF_PI;
    Quaternion q(axis, angle);

    EXPECT_FLOAT_EQ(q.x, 0.0f);
    EXPECT_FLOAT_EQ(q.y, 0.0f);
    EXPECT_FLOAT_EQ(q.z, Sin(HALF_PI * 0.5f));
    EXPECT_FLOAT_EQ(q.w, Cos(HALF_PI * 0.5f));
}

TEST(QuaternionTest, Multiplication)
{
    Quaternion q1(0.0f, 0.0f, 0.0f, 1.0f);
    Quaternion q2(0.0f, 0.0f, 0.0f, 1.0f);
    Quaternion q3 = q1 * q2;

    EXPECT_FLOAT_EQ(q3.w, 1.0f);
}

TEST(QuaternionTest, RotateVector)
{
    Quaternion q(Vector3D::UnitZ(), HALF_PI);
    Vector3D v(1.0f, 0.0f, 0.0f);
    Vector3D rotated = q * v;

    EXPECT_NEAR(rotated.x, 0.0f, EPSILON);
    EXPECT_NEAR(rotated.y, 1.0f, EPSILON);
    EXPECT_NEAR(rotated.z, 0.0f, EPSILON);
}

TEST(QuaternionTest, Normalize)
{
    Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
    float lenBefore = q.Length();
    q.Normalize();
    float lenAfter = q.Length();

    EXPECT_GT(lenBefore, 1.0f);
    EXPECT_NEAR(lenAfter, 1.0f, EPSILON);
}

TEST(QuaternionTest, Slerp)
{
    Quaternion q1(Vector3D::UnitX(), 0.0f);
    Quaternion q2(Vector3D::UnitX(), HALF_PI);
    Quaternion interpolated = Quaternion::Slerp(q1, q2, 0.5f);

    float halfAngle = HALF_PI * 0.25f;
    EXPECT_NEAR(interpolated.x, Sin(halfAngle), EPSILON);
    EXPECT_NEAR(interpolated.w, Cos(halfAngle), EPSILON);
}

TEST(QuaternionTest, ComparisonOperators)
{
    Quaternion q1(0.0f, 0.0f, 0.0f, 1.0f);
    Quaternion q2(0.0f, 0.0f, 0.0f, 1.0f);
    Quaternion q3(0.0f, 0.0f, 0.0f, 0.0f);

    EXPECT_EQ(q1, q2);
    EXPECT_NE(q1, q3);
}
