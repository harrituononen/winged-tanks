namespace utilities
{

inline bool PoolObject::is_active() const
{
    return m_active;
}

inline void PoolObject::activate()
{
    m_active = true;
}

inline void PoolObject::deactivate()
{
    m_active = false;
}

} // namespace utilities
