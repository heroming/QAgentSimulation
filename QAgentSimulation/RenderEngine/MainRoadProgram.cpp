#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MainRoadProgram.h"

#ifndef NDEBUG
#include "RenderEngine/GLErrorChecker.h"
#endif

#include "Utilities/IO.h"
#include "RenderEngine/UniformCamera.h"
#include "RenderEngine/ShaderManager.h"

MainRoadProgram::MainRoadProgram()
{
    m_point_buffer = 0;
    m_index_buffer = 0;
    m_program_handle = 0;
    m_vertex_array_object = 0;
}

MainRoadProgram::~MainRoadProgram()
{
    release_vertex_array();
    if (m_program_handle != 0) glDeleteProgram(m_program_handle);
}

UniformCamera * MainRoadProgram::get_camera()
{
    return m_camera;
}

void MainRoadProgram::set_camera(UniformCamera * camera)
{
    m_camera = camera;
}

bool MainRoadProgram::link_program()
{
    // fetch shaders;
    GLuint l_vertex_shader_handle = ShaderManager::add_shader("./Shaders/MainRoadVS.glsl", GL_VERTEX_SHADER);
    GLuint l_fragment_shader_handle = ShaderManager::add_shader("./Shaders/MainRoadFS.glsl", GL_FRAGMENT_SHADER);

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

void MainRoadProgram::setup_vertex_array()
{
    if (m_vertex_array_object) release_vertex_array();

    // create vertex array object
    glGenVertexArrays(1, &m_vertex_array_object);
    // bind vertex array object
    glBindVertexArray(m_vertex_array_object);

    glGenBuffers(1, &m_point_buffer);
    glGenBuffers(1, &m_index_buffer);

    // vertex coordinates
    glBindVertexBuffer(0, m_point_buffer, 0, 3 * sizeof(float));
    glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
    glEnableVertexAttribArray(0);

    // triangle index
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

    glBindVertexArray(0);
}

void MainRoadProgram::release_vertex_array()
{
    glDeleteVertexArrays(1, &m_vertex_array_object);
    m_vertex_array_object = 0;
}

void MainRoadProgram::render()
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
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_index_buffer);

    glEnable(GL_DEPTH_TEST);
    glLineWidth(3.0);
    glDrawElements(GL_LINES, m_index.size(), GL_UNSIGNED_INT, 0);
    glLineWidth(1.0);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(0);

    glBindVertexArray(0);

#ifndef NDEBUG
    GLErrorChecker::check_current_gl_status(__FILE__, __LINE__, __FUNCSIG__);
#endif
}

void MainRoadProgram::bind_buffer_data()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_point_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_point.size() * sizeof(float), m_point.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_index_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_index.size() * sizeof(int), m_index.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MainRoadProgram::load_data()
{
    m_point.clear();
    m_index.clear();
    IO::load_line_data("./Data/road/create_main_road.dat", m_point, m_index);
    //IO::load_line_data("./Data/road/main_road_shrink.dat", m_point, m_index);
    //IO::load_line_data("./Data/road/main_road_clearance_10.dat", m_point, m_index);
    //IO::load_line_data("./Data/road/main_road_clearance.dat", m_point, m_index);
    //IO::load_line_data("./Data/road/main_road.dat", m_point, m_index);
}

void MainRoadProgram::load_data(const std::string & l_path)
{
    m_point.clear();
    m_index.clear();
    if (! l_path.empty())
    {
        IO::load_line_data(l_path, m_point, m_index);
    }
}

void MainRoadProgram::update_date(const std::vector<int> & index, const std::vector<float> & point)
{
    int n = (int)index.size();
    int m = (int)m_index.size();
    while (n != m)
    {
        if (m < n)
        {
            m_index.push_back(index[m ++]);
        }
        else
        {
            -- m;
            m_index.pop_back();
        }
    }
    n = (int)point.size();
    m = (int)m_point.size();
    while (n != m)
    {
        if (m < n)
        {
            m_point.push_back(point[m ++]);
        }
        else
        {
            -- m;
            m_point.pop_back();
        }
    }
}
