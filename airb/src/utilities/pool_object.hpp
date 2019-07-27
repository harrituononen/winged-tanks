#ifndef UTILITIES_POOL_OBJECT_HPP
#define UTILITIES_POOL_OBJECT_HPP

namespace  utilities
{

class PoolObject
{
public:
             PoolObject ();
    virtual ~PoolObject () = default;

    inline bool is_active  () const;
    inline void activate   ();
    inline void deactivate ();

private:
    bool m_active;
};

} // namespace utilities

#include "pool_object.inl"

#endif // UTILITIES_POOL_OBJECT_HPP
