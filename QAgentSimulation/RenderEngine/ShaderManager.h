#pragma once

#include <string>
#include <GL/glew.h>

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    static GLuint add_shader(const std::string & path, GLenum type);
};