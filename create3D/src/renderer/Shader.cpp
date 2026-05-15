#include "Shader.h"

#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Create3D {

Shader::Shader() = default;

Shader::~Shader()
{
    if (m_ProgramID)
    {
        glDeleteProgram(m_ProgramID);
    }
}

std::string Shader::ReadFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        return "";
    
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

u32 Shader::CompileShader(u32 type, const std::string& source)
{
    u32 shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    i32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void Shader::CheckLinkErrors()
{
    i32 success;
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        glGetProgramInfoLog(m_ProgramID, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader linking error: " << infoLog << std::endl;
    }
}

bool Shader::LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vsSource = ReadFile(vertexPath);
    std::string fsSource = ReadFile(fragmentPath);
    
    if (vsSource.empty() || fsSource.empty())
        return false;

    return LoadFromSource(vsSource, fsSource);
}

bool Shader::LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource)
{
    u32 vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
    u32 fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    if (vs == 0 || fs == 0)
        return false;

    m_ProgramID = glCreateProgram();
    glAttachShader(m_ProgramID, vs);
    glAttachShader(m_ProgramID, fs);
    glLinkProgram(m_ProgramID);

    CheckLinkErrors();

    glDeleteShader(vs);
    glDeleteShader(fs);

    return true;
}

void Shader::Use() const
{
    glUseProgram(m_ProgramID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetInt(const std::string& name, i32 value) const
{
    glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, f32 value) const
{
    glUniform1f(glGetUniformLocation(m_ProgramID, name.c_str()), value);
}

void Shader::SetVec2(const std::string& name, const Vec2& value) const
{
    glUniform2fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const Vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const Vec4& value) const
{
    glUniform4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const Mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetColor(const std::string& name, const Color& value) const
{
    glUniform4f(glGetUniformLocation(m_ProgramID, name.c_str()), value.r, value.g, value.b, value.a);
}

} 
