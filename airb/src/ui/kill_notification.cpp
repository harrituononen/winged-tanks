#include "kill_notification.hpp"

#include "../logic/game.hpp"
#include "../ui/font.hpp"
#include "../ui/font_renderer.hpp"

namespace
{
    int const MAX_NOTIFICATION_AMOUNT = 5;
}

namespace ui {

KillNotification::KillNotification(Font const& font,
                                   math::Vector2f const& pos,
                                   math::Vector4f const& color)
    : m_font     (font)
    , m_position (pos)
    , m_color    (color)
{
    m_position[Y] -= m_font.get_size();
}

void KillNotification::add_notification(std::string const& notif)
{
    if (m_notifications.size() >= MAX_NOTIFICATION_AMOUNT)
        m_notifications.erase(m_notifications.begin());

    m_notifications.push_back(
        std::pair<std::string, math::Vector4f>(
            notif, math::Vector4f({ 1, 1, 1, 1 })
        )
    );
}

void KillNotification::render_text(FontRenderer& ren)
{
    int font_size = m_font.get_size();
    math::Vector2f position(m_position);

    size_t size = m_notifications.size();
    for (int i = static_cast<int>(size - 1); i >= 0; --i)
    {
        position[X] = logic::Game::TARGET_DIMENSIONS[X]
                    - m_font.get_text_width(m_notifications[i].first)
                    - 20.0f;

        ren.render_text(
            m_font,
            m_notifications[i].first,
            position,
            m_notifications[i].second
        );

        position[Y] -= font_size * 1.5f;
    }
}

void KillNotification::update(std::chrono::milliseconds const& dt)
{
    remove_old_notifications();

    size_t size = m_notifications.size();

    for (size_t i = 0; i < size; ++i)
        m_notifications[i].second[A] -= 0.0005f * dt.count();
}

void KillNotification::remove_old_notifications()
{
    if (m_notifications.size() == 0)
        return;

    if (m_notifications[0].second[A] <= 0.0f)
        m_notifications.erase(m_notifications.begin());
}

} // namespace ui
