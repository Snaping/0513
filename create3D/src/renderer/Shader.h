#pragma once

#include "Types.h"

namespace Create3D {

class Shader
{
public:
    Shader();
    ~Shader();

    bool LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    bool LoadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    void Use() const;
    void Unbind() const;

    void SetInt(const std::string& name, i32 value) const;
    void SetFloat(const std::string& name, f32 value) const;
    void SetVec2(const std::string& name, const Vec2& value) const;
    void SetVec3(const std::string& name, const Vec3& value) const;
    void SetVec4(const std::string& name, const Vec4& value) const;
    void SetMat4(const std::string& name, const Mat4& value) const;
    void SetColor(const std::string& name, const Color& value) const;

    u32 GetID() const { return m_ProgramID; }

private:
    u32 m_ProgramID = 0;

    u32 CompileShader(u32 type, const std::string& source);
    std::string ReadFile(const std::string& path);
    void CheckLinkErrors();
};

} 
