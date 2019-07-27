#ifndef UI_KILL_NOTIFICATION_HPP
#define UI_KILL_NOTIFICATION_HPP

#include <chrono>
#include <string>
#include <utility>
#include <vector>

#include "../math/matrix.hpp"

namespace ui {

class Font;
class FontRenderer;

class KillNotification final
{
private:
    Font const&      m_font;
    math::Vector2f   m_position;
    math::Vector4f   m_color;

    std::vector<std::pair<std::string, math::Vector4f>> m_notifications;

public:
     KillNotification(Font const& font,
                      math::Vector2f const& pos,
                      math::Vector4f const& color);
    ~KillNotification() = default;

    KillNotification            (KillNotification const&) = delete;
    KillNotification& operator= (KillNotification const&) = delete;

    void add_notification (std::string const& notif);
    void render_text      (FontRenderer& ren);
    void update           (std::chrono::milliseconds const& dt);

private:
    void remove_old_notifications();
};

} // namespace ui

#endif // UI_KILL_NOTIFICATION_HPP
