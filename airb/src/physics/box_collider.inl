namespace physics
{
    inline std::vector<math::Vector2f> BoxCollider::get_vertexes()
    {
        return m_vertexes;
    }
    inline math::Vector2f BoxCollider::get_center()
    {
        return m_center;
    }

    inline std::vector<math::Vector2f> BoxCollider::get_axes()
    {
        return m_axes;
    }
    /*inline RigidBodyWithCollider BoxCollider::get_rb()
    {
        return m_rb;
    }
    inline void BoxCollider::set_rb(RigidBodyWithCollider const& rb)
    {
        m_rb = rb;
    }*/
} // namespace physics