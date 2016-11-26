#ifndef NDEBUG
#pragma once
// cpp
#include <iostream>
class GLErrorChecker
{
public:
    GLErrorChecker(void);
    ~GLErrorChecker(void);

    static inline void check_current_gl_status(const char* file_name, int file_line, const char* function_signature)
    {
        GLenum l_last_error = glGetError();
        while (l_last_error != GL_NO_ERROR)
        {
            std::cerr << file_name << "(" << file_line << ")" << function_signature << ":" << gluErrorString(l_last_error) << std::endl;
            l_last_error = glGetError();
        }
    }

    static inline void print_error_message(const char* file_name, int file_line, const char* function_signature, const char* message)
    {
        std::cerr << file_name << "(" << file_line << ")" << function_signature << ":" << message << std::endl;
    }
};
#endif

