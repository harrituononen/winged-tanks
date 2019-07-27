namespace graphics {

inline void Shader::use() const
{
    glUseProgram(m_program);
}

} // namespace graphics
