#include <gtest/gtest.h>
#include "../Source/Math/CE_MathConstants.h"
#include "../Source/Math/MathTypes.h"

// ============================================================================
// Constants Tests
// ============================================================================
using namespace CEMath;

TEST(ConstantsTest, PIValues)
{
    EXPECT_NEAR(PI, 3.14159265358979323846f, 1e-6f);
    EXPECT_NEAR(TWO_PI, 6.28318530717958647692f, 1e-6f);
    EXPECT_NEAR(HALF_PI, 1.57079632679489661923f, 1e-6f);
    EXPECT_NEAR(QUARTER_PI, 0.785398163397448309616f, 1e-6f);

    // Проверка взаимосвязей
    EXPECT_NEAR(TWO_PI, PI * 2.0f, 1e-6f);
    EXPECT_NEAR(HALF_PI, PI / 2.0f, 1e-6f);
    EXPECT_NEAR(QUARTER_PI, PI / 4.0f, 1e-6f);
}

TEST(ConstantsTest, AdditionalConstants)
{
#ifdef __cpp_lib_math_constants
    // Тесты для дополнительных констант (только C++20)
    EXPECT_NEAR(INV_PI, 0.31830988618379067153776752674503f, 1e-6f);
    EXPECT_NEAR(INV_SQRTPI, 0.56418958354775628694807945156077f, 1e-6f);
    EXPECT_NEAR(INV_PI, 1.0f / PI, 1e-6f);
    EXPECT_NEAR(INV_SQRTPI, 1.0f / std::sqrt(PI), 1e-6f);
#endif
}

TEST(ConstantsTest, EConstants)
{
#ifdef __cpp_lib_math_constants
    EXPECT_NEAR(E, 2.7182818284590452353602874713527f, 1e-6f);
    EXPECT_NEAR(LOG2E, 1.4426950408889634073599246810019f, 1e-6f);
    EXPECT_NEAR(LOG10E, 0.43429448190325182765112891891661f, 1e-6f);
#else
    EXPECT_NEAR(E, 2.71828182845904523536f, 1e-6f);
    EXPECT_NEAR(LOG2E, 1.44269504088896340736f, 1e-6f);
    EXPECT_NEAR(LOG10E, 0.43429448190325182765f, 1e-6f);
#endif

    // Взаимосвязи с ранее проверенными константами
    EXPECT_NEAR(LOG2E, 1.0f / LN2, 1e-5f);    // log2(e) = 1 / ln(2)
    EXPECT_NEAR(LOG10E, 1.0f / LN10, 1e-5f);  // log10(e) = 1 / ln(10)

    // Проверка через exp и log
    EXPECT_NEAR(std::exp(1.0f), E, 1e-6f);
    EXPECT_NEAR(std::log(E), 1.0f, 1e-6f);
}

TEST(ConstantsTest, LogarithmConstants)
{
#ifdef __cpp_lib_math_constants
    EXPECT_NEAR(LN2, 0.69314718055994530941723212145818f, 1e-6f);
    EXPECT_NEAR(LN10, 2.3025850929940456840179914546844f, 1e-6f);
#else
    EXPECT_NEAR(LN2, 0.69314718055994530942f, 1e-6f);
    EXPECT_NEAR(LN10, 2.30258509299404568402f, 1e-6f);
#endif

    // Проверка взаимосвязей
    EXPECT_NEAR(std::logf(2.0f), LN2, 1e-6f);
    EXPECT_NEAR(std::logf(10.0f), LN10, 1e-6f);
    EXPECT_NEAR(std::expf(LN2), 2.0f, 1e-6f);
    EXPECT_NEAR(std::expf(LN10), 10.0f, 1e-5f);
}

TEST(ConstantsTest, SquareRootConstants)
{
#ifdef __cpp_lib_math_constants
    EXPECT_NEAR(SQRT2, 1.4142135623730950488016887242097f, 1e-6f);
    EXPECT_NEAR(SQRT3, 1.7320508075688772935274463415059f, 1e-6f);
    EXPECT_NEAR(INV_SQRT3, 0.57735026918962576450914878050196f, 1e-6f);
#else
    EXPECT_NEAR(SQRT2, 1.41421356237309504880f, 1e-6f);
    EXPECT_NEAR(SQRT3, 1.73205080756887729353f, 1e-6f);
    EXPECT_NEAR(INV_SQRT3, 0.57735026918962576451f, 1e-6f);
#endif

    // Проверка взаимосвязей
    EXPECT_NEAR(std::sqrt(2.0f), SQRT2, 1e-6f);
    EXPECT_NEAR(std::sqrt(3.0f), SQRT3, 1e-6f);
    EXPECT_NEAR(INV_SQRT3, 1.0f / SQRT3, 1e-6f);
    EXPECT_NEAR(SQRT2 * SQRT2, 2.0f, 1e-6f);
    EXPECT_NEAR(SQRT3 * SQRT3, 3.0f, 1e-6f);
}

TEST(ConstantsTest, GoldenRatio)
{
#ifdef __cpp_lib_math_constants
    EXPECT_NEAR(PHI, 1.6180339887498948482045868343656f, 1e-6f);
#else
    EXPECT_NEAR(PHI, 1.61803398874989484820f, 1e-6f);
#endif

    // Проверка свойств золотого сечения: φ² = φ + 1
    EXPECT_NEAR(PHI * PHI, PHI + 1.0f, 1e-6f);
    // Проверка: 1/φ = φ - 1
    EXPECT_NEAR(1.0f / PHI, PHI - 1.0f, 1e-6f);
}

TEST(ConstantsTest, EulerGamma)
{
#ifdef __cpp_lib_math_constants
    EXPECT_NEAR(EULER_GAMMA, 0.57721566490153286060651209008240f, 1e-6f);
#else
    EXPECT_NEAR(EULER_GAMMA, 0.57721566490153286061f, 1e-6f);
#endif

    // Константа должна быть положительной
    EXPECT_GT(EULER_GAMMA, 0.0f);
    EXPECT_LT(EULER_GAMMA, 1.0f);
}

TEST(ConstantsTest, ConversionFactors)
{
    EXPECT_NEAR(DEG_TO_RAD, PI / 180.0f, 1e-6f);
    EXPECT_NEAR(RAD_TO_DEG, 180.0f / PI, 1e-6f);

    // Проверка взаимных преобразований
    EXPECT_NEAR(toRadians(180.0f), PI, 1e-6f);
    EXPECT_NEAR(toRadians(90.0f), HALF_PI, 1e-6f);
    EXPECT_NEAR(toDegrees(PI), 180.0f, 1e-6f);
    EXPECT_NEAR(toDegrees(HALF_PI), 90.0f, 1e-6f);
    EXPECT_NEAR(toDegrees(toRadians(45.0f)), 45.0f, 1e-6f);
}

TEST(ConstantsTest, Epsilon)
{
    EXPECT_GT(EPSILON, 0.0f);
    EXPECT_LE(EPSILON, 1e-6f);
    EXPECT_FLOAT_EQ(EPSILON, 1e-6f);
    EXPECT_FLOAT_EQ(EPSILON_SQUARED, 1e-12f);
    EXPECT_LT(EPSILON_SQUARED, EPSILON);
}

TEST(ConstantsTest, FloatLimits)
{
    EXPECT_GT(FLOAT_MAX, 0.0f);
    EXPECT_GT(FLOAT_MIN, 0.0f);
    EXPECT_LT(FLOAT_MIN, FLOAT_MAX);
    EXPECT_GT(FLOAT_MAX, 1e38f);
    EXPECT_LT(FLOAT_MIN, 1e-37f);
}

TEST(ConstantsTest, TrigonometricIdentities)
{
    // Проверка тригонометрических тождеств с нашими константами
    EXPECT_NEAR(std::sin(PI), 0.0f, 1e-6f);
    EXPECT_NEAR(std::cos(PI), -1.0f, 1e-6f);
    EXPECT_NEAR(std::sin(HALF_PI), 1.0f, 1e-6f);
    EXPECT_NEAR(std::cos(HALF_PI), 0.0f, 1e-6f);
    EXPECT_NEAR(std::sin(QUARTER_PI), std::cos(QUARTER_PI), 1e-6f);
    EXPECT_NEAR(std::sin(QUARTER_PI), 0.7071067811865475f, 1e-6f);
}

TEST(ConstantsTest, Consistency)
{
    // Проверка что константы определены (не должны быть нулями)
    EXPECT_NE(PI, 0.0f);
    EXPECT_NE(TWO_PI, 0.0f);
    EXPECT_NE(HALF_PI, 0.0f);
    EXPECT_NE(QUARTER_PI, 0.0f);
    EXPECT_NE(DEG_TO_RAD, 0.0f);
    EXPECT_NE(RAD_TO_DEG, 0.0f);
}