namespace physics
{

    inline math::Vector2f Collision::get_mtv()
    {
        return m_mtv;
    }
    inline int Collision::get_collider() 
    {
        return m_collider;
    }

    inline void Collision::set_mtv(math::Vector2f mtv)
    {
        m_mtv = mtv;
    }

    inline void Collision::set_collider(int collider)
    {
        m_collider = collider;
    }

} // namespace physics