#ifndef CEMATH_CONSTANTS_H
#define CEMATH_CONSTANTS_H

#if __cplusplus >= 202002L

#include <numbers>

namespace CEMath
{
using namespace std::numbers;

// Основные математические константы
constexpr float PI = pi_v<float>;
constexpr float TWO_PI = 2.0f * pi_v<float>;
constexpr float HALF_PI = pi_v<float> / 2.0f;
constexpr float QUARTER_PI = pi_v<float> / 4.0f;

// Дополнительные константы
constexpr float INV_PI = inv_pi_v<float>;
constexpr float INV_SQRTPI = inv_sqrtpi_v<float>;

// Константы связанные с e
constexpr float E = e_v<float>;
constexpr float LOG2E = log2e_v<float>;
constexpr float LOG10E = log10e_v<float>;

// Константы связанные с логарифмами
constexpr float LN2 = ln2_v<float>;
constexpr float LN10 = ln10_v<float>;

// Квадратные корни
constexpr float SQRT2 = sqrt2_v<float>;
constexpr float SQRT3 = sqrt3_v<float>;
constexpr float INV_SQRT3 = inv_sqrt3_v<float>;

// Золотое сечение
constexpr float PHI = phi_v<float>;

// Постоянная Эйлера-Маскерони
constexpr float EULER_GAMMA = egamma_v<float>;

// Преобразования
constexpr float DEG_TO_RAD = pi_v<float> / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / pi_v<float>;

// Точность и границы
constexpr float EPSILON = 1e-6f;
constexpr float EPSILON_SQUARED = 1e-12f;

constexpr float FLOAT_MAX = 3.402823466e+38f;
constexpr float FLOAT_MIN = 1.175494351e-38f;

// Радианы в градусы/градусы в радианы
inline constexpr float toRadians(float degrees)
{
    return degrees * DEG_TO_RAD;
}

inline constexpr float toDegrees(float radians)
{
    return radians * RAD_TO_DEG;
}
}  // namespace CEMath

#else

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace CEMath
{
// Основные математические константы
constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 6.28318530717958647692f;
constexpr float HALF_PI = 1.57079632679489661923f;
constexpr float QUARTER_PI = 0.785398163397448309616f;

// Дополнительные константы
constexpr float INV_PI = 0.31830988618379067153776752674503f;
constexpr float INV_SQRTPI = 0.56418958354775628694807945156077f;

// Константы связанные с e
constexpr float E = 2.7182818284590452353602874713527f;
constexpr float LOG2E = 1.4426950408889634073599246810019f;
constexpr float LOG10E = 0.43429448190325182765112891891661f;

// Константы связанные с логарифмами
constexpr float LN2 = 0.69314718055994530941723212145818f;
constexpr float LN10 = 2.3025850929940456840179914546844f;

// Квадратные корни
constexpr float SQRT2 = 1.4142135623730950488016887242097f;
constexpr float SQRT3 = 1.7320508075688772935274463415059f;
constexpr float INV_SQRT3 = 0.57735026918962576450914878050196f;

// Золотое сечение
constexpr float PHI = 1.6180339887498948482045868343656f;

// Постоянная Эйлера-Маскерони
constexpr float EULER_GAMMA = 0.57721566490153286060651209008240f;

// Преобразования
constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;

// Точность и границы
constexpr float EPSILON = 1e-6f;
constexpr float EPSILON_SQUARED = 1e-12f;

constexpr float FLOAT_MAX = 3.402823466e+38f;
constexpr float FLOAT_MIN = 1.175494351e-38f;

// Радианы в градусы/градусы в радианы
inline constexpr float toRadians(float degrees)
{
    return degrees * DEG_TO_RAD;
}

inline constexpr float toDegrees(float radians)
{
    return radians * RAD_TO_DEG;
}
}  // namespace CEMath

#endif

#endif  // CEMATH_CONSTANTS_H