namespace entities {

inline bool Plane::is_reloading() const
{
    return m_weapons[m_current_weapon]->is_reloading();
}

inline physics::RigidBodyWithCollider& Plane::get_rigid_body()
{
    return m_rigid_body;
}

inline graphics::Sprite& Plane::get_weapon_sprite()
{
    return m_weapons[m_current_weapon]->get_sprite();
}

inline graphics::Sprite& Plane::get_sprite()
{
    return m_sprite;
}

inline Weapon& Plane::get_weapon()
{
    return *m_weapons[m_current_weapon];
}

inline int Plane::get_current_weapon() const
{
    return m_current_weapon;
}

inline int Plane::get_ammo() const
{
    return m_weapons[m_current_weapon]->get_rounds();
}

inline ConsumableResource<int>& Plane::get_health()
{
    return m_health;
}

inline void Plane::set_game_state(logic::GameplayState* game_state)
{
    m_game_state = game_state;
}

inline void Plane::reload_weapon()
{
    m_weapons[m_current_weapon]->start_reloading();
}

inline void Plane::set_weapon_rotation(float const rot)
{
    m_weapons[m_current_weapon]->set_rotation(rot);
}

inline void Plane::take_damage(int const damage)
{
    m_health.value -= damage;
    if (m_health.value < 0)
        m_health.value = 0;
}

inline void Plane::switch_weapon()
{
    m_weapons[m_current_weapon]->stop_reloading();
    m_current_weapon = (m_current_weapon + 1) % 2;
}

} // namespace entities
