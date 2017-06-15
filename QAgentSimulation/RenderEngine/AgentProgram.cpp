#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AgentProgram.h"

#ifndef NDEBUG
#include "RenderEngine/GLErrorChecker.h"
#endif

#include "Utilities/IO.h"
#include "RenderEngine/UniformCamera.h"
#include "RenderEngine/ShaderManager.h"

AgentProgram::AgentProgram()
{
    m_size = 0;
    m_timestamp = 0;
    m_simulation = 0;
    m_point_buffer = 0;
    m_program_handle = 0;
    m_vertex_array_object = 0;
}

AgentProgram::~AgentProgram()
{
    release_vertex_array();
    if (m_program_handle != 0) glDeleteProgram(m_program_handle);
}

UniformCamera * AgentProgram::get_camera()
{
    return m_camera;
}

void AgentProgram::set_camera(UniformCamera * camera)
{
    m_camera = camera;
}

bool AgentProgram::link_program()
{
    // fetch shaders;
    GLuint l_vertex_shader_handle = ShaderManager::add_shader("./Shaders/AgentVS.glsl", GL_VERTEX_SHADER);
    GLuint l_fragment_shader_handle = ShaderManager::add_shader("./Shaders/AgentFS.glsl", GL_FRAGMENT_SHADER);

    if (!l_vertex_shader_handle || !l_fragment_shader_handle) return false;

    // create program handle
    m_program_handle = glCreateProgram();

    // attach shaders
    glAttachShader(m_program_handle, l_vertex_shader_handle);
    glAttachShader(m_program_handle, l_fragment_shader_handle);
#ifndef NDEBUG
    GLErrorChecker::check_current_gl_status(__FILE__, __LINE__, __FUNCSIG__);
#endif

    // link program
    glLinkProgram(m_program_handle);
    GLint l_program_link_state;
    glGetProgramiv(m_program_handle, GL_LINK_STATUS, &l_program_link_state);

    if (!l_program_link_state)
    {
#ifndef NDEBUG
        GLsizei l_link_log_length;
        glGetProgramiv(m_program_handle, GL_INFO_LOG_LENGTH, &l_link_log_length);

        std::vector<GLchar> l_link_log_content(l_link_log_length);
        glGetProgramInfoLog(m_program_handle, l_link_log_length, &l_link_log_length, l_link_log_content.data());
        GLErrorChecker::print_error_message(__FILE__, __LINE__, __FUNCSIG__, l_link_log_content.data());
#endif
        glDeleteProgram(m_program_handle);
        return false;
    }
    return true;
}

void AgentProgram::setup_vertex_array()
{
    if (m_vertex_array_object) release_vertex_array();

    // create vertex array object
    glGenVertexArrays(1, &m_vertex_array_object);
    // bind vertex array object
    glBindVertexArray(m_vertex_array_object);

    glGenBuffers(1, &m_point_buffer);

    // vertex coordinates
    glBindVertexBuffer(0, m_point_buffer, 0, 3 * sizeof(float));
    glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void AgentProgram::release_vertex_array()
{
    glDeleteVertexArrays(1, &m_vertex_array_object);
    m_vertex_array_object = 0;
}

void AgentProgram::render()
{
    if (!m_camera || !m_vertex_array_object || !m_program_handle) return;

    // bind vertex array object
    glBindVertexArray(m_vertex_array_object);

    // use program
    glUseProgram(m_program_handle);

    GLuint l_modelview_matrix_location = glGetUniformLocation(m_program_handle, "modelview_matrix");
    glUniformMatrix4fv(l_modelview_matrix_location, 1, false, m_camera->get_model_view_matrix());
    GLuint l_pojection_matrix_location = glGetUniformLocation(m_program_handle, "projection_matrix");
    glUniformMatrix4fv(l_pojection_matrix_location, 1, false, m_camera->get_projection_matrix());

    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_point_buffer);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_DEPTH_TEST);
    if (m_timestamp >= 0 && m_timestamp < (int)m_point.size())
    {
        glDrawArrays(GL_POINTS, 0, m_point[m_timestamp].size() / 3);
    }
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_POINT_SPRITE);
    glDisable(GL_PROGRAM_POINT_SIZE);

    glUseProgram(0);

    glBindVertexArray(0);

#ifndef NDEBUG
    GLErrorChecker::check_current_gl_status(__FILE__, __LINE__, __FUNCSIG__);
#endif
}

void AgentProgram::bind_buffer_data(const int k)
{
    m_timestamp = k;
    glBindBuffer(GL_ARRAY_BUFFER, m_point_buffer);
    if (k != -1)
    {
        glBufferData(GL_ARRAY_BUFFER, m_point[k].size() * sizeof(float), m_point[k].data(), GL_DYNAMIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void AgentProgram::load_data()
{
    m_size = TIME_COUNT;
    m_point.clear();
    m_point.resize(TIME_COUNT);

    char ch[16];
    for (int k = 0; k < TIME_COUNT; ++ k)
    {
        //sprintf(ch, "%07d", k * INTERVAL);
        sprintf(ch, "%06d", k);
        const std::string agent_path = "./Data/agent/learn_10/learn_10_" + std::string(ch) + ".dat";
        IO::load_point_data(agent_path, m_point[k]);
    }

    //const std::string agent_path_out = "./Data/agent/agent_rand_uniform.dat";
    //IO::load_point_data(agent_path_out, m_point[0]);
}

bool AgentProgram::bind_next()
{
    if (m_timestamp + 1 < m_size)
    {
        bind_buffer_data(++ m_timestamp);
        return true;
    }
    return false;
}

bool AgentProgram::bind_prevois()
{
    if (m_timestamp > 0)
    {
        bind_buffer_data(-- m_timestamp);
        return true;
    }
    return false;
}

void AgentProgram::start_simulation(const bool start)
{
    if (start)
    {
        if (! m_simulation)
        {
            m_size = 0;
            m_point.clear();
        }
        bind_buffer_data(-1);
    }
    else
    {
        if (m_simulation)
        {
            load_data();
        }
        bind_buffer_data(0);
    }
    m_simulation = start;
}

void AgentProgram::update(const std::vector<float> & agent)
{
    ++ m_size;
    ++ m_timestamp;
    m_point.push_back(agent);
    bind_buffer_data(m_timestamp);
}

int AgentProgram::get_timestamp() const
{
    return m_timestamp;
}

