#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>

class UniformCamera;

class AgentProgram
{
public:
    AgentProgram();
    ~AgentProgram();

    UniformCamera * get_camera();
    void set_camera(UniformCamera * camera);

    bool link_program();
    void setup_vertex_array();
    void release_vertex_array();
    void bind_buffer_data();
    void render();

    void load_agent();

private:

    UniformCamera * m_camera;

    GLuint m_point_buffer;
    GLuint m_program_handle;
    GLuint m_vertex_array_object;

    std::vector<float> m_point;
};