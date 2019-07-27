#ifndef MATH_GENERAL_HPP
#define MATH_GENRAL_HPP

namespace math {

extern float const PI;
extern float const DOUBLE_PI;

template<typename T> inline T clamp  (T const val, T const min, T const max);
template<typename T> inline T signum (T const n);

} // namespace math

#include "general.inl"

#endif // MATH_GENERAL_HPP
