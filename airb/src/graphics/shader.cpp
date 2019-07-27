#include "shader.hpp"

#include <memory>
#include <stdexcept>

#include "render_tools.hpp"
#include "../utilities/debug.hpp"
#include "../utilities/file_io.hpp"

namespace graphics {

namespace
{
    enum class TargetType
    {
        SHADER,
        PROGRAM
    };
}

static GLuint create_shader   (std::string const& path, GLenum const sh_t);
static GLuint create_program  (GLuint const vs, GLuint const fs);
static void   validate_status (GLuint const tgt, TargetType const tgt_t, GLenum const stat_t);

Shader::Shader(std::string const& path)
    : m_path (path)

    , m_vertex_shader   (0)
    , m_fragment_shader (0)
    , m_program         (0)

    , m_vector_uniforms_3fv ()
    , m_vector_uniforms_4fv ()
    , m_matrix_uniforms_3fv ()
    , m_matrix_uniforms_4fv ()
{
    try
    {
        m_vertex_shader   = create_shader(path, GL_VERTEX_SHADER);
        m_fragment_shader = create_shader(path, GL_FRAGMENT_SHADER);
        m_program         = create_program(m_vertex_shader, m_fragment_shader);
    }
    catch (std::exception const&)
    {
        if (m_vertex_shader   != 0) glDeleteShader(m_vertex_shader);
        if (m_fragment_shader != 0) glDeleteShader(m_fragment_shader);
        if (m_program         != 0) glDeleteProgram(m_program);

        throw;
    }

    utilities::Debug::log("Shader " + m_path + " loaded.");
}

Shader::~Shader()
{
    glDeleteShader(m_vertex_shader);
    glDeleteShader(m_fragment_shader);
    glDeleteProgram(m_program);

    utilities::Debug::log("Shader " + m_path + " destroyed.");
}

static GLuint create_shader(std::string const& path, GLenum const sh_t)
{
    GLuint const sh = glCreateShader(sh_t);
    if (sh == 0)
        throw std::runtime_error("Shader creation failed.");

    std::string const ext = sh_t == GL_VERTEX_SHADER ? ".vs" : ".fs";
    std::string const tmp = utilities::FileIO::get_string_from_file(path + ext);
    GLchar const* sh_cstr = tmp.c_str();
    glShaderSource(sh, 1, &sh_cstr, nullptr);
    glCompileShader(sh);
    validate_status(sh, TargetType::SHADER, GL_COMPILE_STATUS);

    return sh;
}

static GLuint create_program(GLuint const vs, GLuint const fs)
{
    GLuint const prog = glCreateProgram();
    if (prog == 0)
        throw std::runtime_error("Program creation failed.");

    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    validate_status(prog, TargetType::PROGRAM, GL_LINK_STATUS);
    glValidateProgram(prog);
    validate_status(prog, TargetType::PROGRAM, GL_VALIDATE_STATUS);

    return prog;
}

static void validate_status(GLuint const tgt, TargetType const tgt_t, GLenum const stat_t)
{
    bool const sh = tgt_t == TargetType::SHADER;

    GLint stat;
    if (sh)
        glGetShaderiv(tgt, stat_t, &stat);
    else
        glGetProgramiv(tgt, stat_t, &stat);

    if (stat != GL_TRUE)
    {
        GLint len;
        if (sh)
            glGetShaderiv(tgt, GL_INFO_LOG_LENGTH, &len);
        else
            glGetProgramiv(tgt, GL_INFO_LOG_LENGTH, &len);

        try
        {
            std::unique_ptr<GLchar[]> log(new GLchar[len]);
            if (sh)
                glGetShaderInfoLog(tgt, len, &len, &log[0]);
            else
                glGetProgramInfoLog(tgt, len, &len, &log[0]);

            std::string const tgt_t_str  = sh ? "Shader " : "Program ";
            std::string const stat_t_str = stat_t == GL_COMPILE_STATUS ? "compilation "
                                         : stat_t == GL_LINK_STATUS    ? "linking "
                                         :                               "validation ";
            throw std::runtime_error(tgt_t_str + stat_t_str + "failed:\n" + std::string(&log[0]));
        }
        catch (std::bad_alloc const&)
        {
            throw std::runtime_error("Failed to allocate memory for error log.");
        }
    }
}

void Shader::set_vector_uniform_3fv(std::string const& u_name, math::Vector3f const& vec)
{
    use();
    auto const itr = m_vector_uniforms_3fv.find(u_name);
    if (itr == m_vector_uniforms_3fv.end())
        m_vector_uniforms_3fv[u_name] = glGetUniformLocation(m_program, u_name.c_str());
    glUniform3fv(m_vector_uniforms_3fv[u_name], 1, &vec[0]);
}

void Shader::set_vector_uniform_4fv(std::string const& u_name, math::Vector4f const& vec)
{
    use();
    auto const itr = m_vector_uniforms_4fv.find(u_name);
    if (itr == m_vector_uniforms_4fv.end())
        m_vector_uniforms_4fv[u_name] = glGetUniformLocation(m_program, u_name.c_str());
    glUniform4fv(m_vector_uniforms_4fv[u_name], 1, &vec[0]);;
}

void Shader::set_matrix_uniform_3fv(std::string const& u_name, math::Matrix3f const& mat)
{
    use();
    auto const itr = m_matrix_uniforms_3fv.find(u_name);
    if (itr == m_matrix_uniforms_3fv.end())
        m_matrix_uniforms_3fv[u_name] = glGetUniformLocation(m_program, u_name.c_str());
    glUniformMatrix3fv(m_matrix_uniforms_3fv[u_name], 1, GL_TRUE, &mat[0]);
}

void Shader::set_matrix_uniform_4fv(std::string const& u_name, math::Matrix4f const& mat)
{
    use();
    auto const itr = m_matrix_uniforms_4fv.find(u_name);
    if (itr == m_matrix_uniforms_4fv.end())
        m_matrix_uniforms_4fv[u_name] = glGetUniformLocation(m_program, u_name.c_str());
    glUniformMatrix4fv(m_matrix_uniforms_4fv[u_name], 1, GL_TRUE, &mat[0]);
}

} // namespace graphics
