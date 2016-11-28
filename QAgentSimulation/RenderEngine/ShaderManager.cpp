#include <vector>

#include "ShaderManager.h"

#ifndef NDEBUG
#include "RenderEngine/GLErrorChecker.h"
#endif

#include "Utilities/IO.h"

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

GLuint ShaderManager::add_shader(const std::string & path, GLenum type)
{
    // create shader handle
    GLuint l_shader_handle = glCreateShader(type);
#ifndef NDEBUG
    GLErrorChecker::check_current_gl_status(__FILE__, __LINE__, __FUNCSIG__);
#endif

    std::vector<char> l_shader_file_content;
    if (!IO::load_shader(path, l_shader_file_content))
    {
        glDeleteShader(l_shader_handle);
        return 0;
    }

    // attach shader source
    const GLchar * l_shader_file_content_ptr = l_shader_file_content.data();
    GLint l_shader_file_content_length = l_shader_file_content.size();
    glShaderSource(l_shader_handle, 1, &l_shader_file_content_ptr, &l_shader_file_content_length);
#ifndef NDEBUG
    GLErrorChecker::check_current_gl_status(__FILE__, __LINE__, __FUNCSIG__);
#endif

    // compile shader
    glCompileShader(l_shader_handle);
    GLint l_shader_compile_state;
    glGetShaderiv(l_shader_handle, GL_COMPILE_STATUS, &l_shader_compile_state);
    if (!l_shader_compile_state)
    {
#ifndef NDEBUG
        GLsizei l_shader_compile_state_log_length;
        glGetShaderiv(l_shader_handle, GL_INFO_LOG_LENGTH, &l_shader_compile_state_log_length);

        std::vector<GLchar> l_shader_compile_state_log_content(l_shader_compile_state_log_length);
        glGetShaderInfoLog(l_shader_handle, l_shader_compile_state_log_length, &l_shader_compile_state_log_length, l_shader_compile_state_log_content.data());
        GLErrorChecker::print_error_message(__FILE__, __LINE__, __FUNCSIG__, l_shader_compile_state_log_content.data());
#endif
        glDeleteShader(l_shader_handle);
        return 0;
    }

    return l_shader_handle;

}