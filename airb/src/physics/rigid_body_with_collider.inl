namespace physics
{

inline std::vector<BoxCollider>& RigidBodyWithCollider::get_collider()
{
    return m_collider;
}

} // namespace physics