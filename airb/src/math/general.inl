namespace math {

template<typename T>
inline T clamp(T const val, T const min, T const max)
{
    return val < min ? min : (val > max ? max : val);
}

template<typename T>
inline T signum(T const n)
{
    return static_cast<T>((n > static_cast<T>(0)) - (n < static_cast<T>(0)));
}

} // namespace math
