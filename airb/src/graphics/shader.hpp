#ifndef GRAPHICS_SHADER_HPP
#define GRAPHICS_SHADER_HPP

#include <map>
#include <string>

#include <glew.h>

#include "../math/matrix.hpp"

namespace graphics {

class Shader final
{
private:
    std::string const m_path;

    GLuint m_vertex_shader;
    GLuint m_fragment_shader;
    GLuint m_program;

    std::map<std::string, GLint> m_vector_uniforms_3fv;
    std::map<std::string, GLint> m_vector_uniforms_4fv;
    std::map<std::string, GLint> m_matrix_uniforms_3fv;
    std::map<std::string, GLint> m_matrix_uniforms_4fv;

public:
    explicit  Shader(std::string const& path);
             ~Shader();

    Shader            (Shader const&) = delete;
    Shader& operator= (Shader const&) = delete;

    inline void use() const;

    void set_vector_uniform_3fv(std::string const& u_name, math::Vector3f const& vec);
    void set_vector_uniform_4fv(std::string const& u_name, math::Vector4f const& vec);
    void set_matrix_uniform_3fv(std::string const& u_name, math::Matrix3f const& mat);
    void set_matrix_uniform_4fv(std::string const& u_name, math::Matrix4f const& mat);
};

} // namespace graphics

#include "shader.inl"

#endif // GRAPHICS_SHADER_HPP
