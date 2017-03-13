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
    void bind_buffer_data(const int k = 0);
    void render();

    void load_data();
    bool bind_next();
    bool bind_prevois();

private:

    static const int INTERVAL = 50;
    static const int TIMESTEP = 15000;
    static const int TIME_COUNT = 300;
    

    UniformCamera * m_camera;

    GLuint m_point_buffer;
    GLuint m_program_handle;
    GLuint m_vertex_array_object;

    int m_timestep;
    std::vector<std::vector<float>> m_point;
};