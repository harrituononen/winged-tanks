namespace graphics {

inline void Window::swap_buffers() const
{
    glfwSwapBuffers(m_glfw_window);
}

inline void Window::set_title(std::string const& title)
{
    glfwSetWindowTitle(m_glfw_window, (m_title = title).c_str());
}

inline void Window::set_cursor_visible(bool const c)
{
    glfwSetInputMode(m_glfw_window, GLFW_CURSOR, c ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

inline std::string Window::get_title() const
{
    return m_title;
}

inline math::Vector2i const& Window::get_dimensions() const
{
    return m_dimensions;
}

inline bool Window::is_open() const
{
    return !glfwWindowShouldClose(m_glfw_window);
}

} // namespace graphics
