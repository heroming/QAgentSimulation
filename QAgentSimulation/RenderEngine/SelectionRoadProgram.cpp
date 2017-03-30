#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SelectionRoadProgram.h"

#ifndef NDEBUG
#include "RenderEngine/GLErrorChecker.h"
#endif

#include "Utilities/IO.h"
#include "RenderEngine/UniformCamera.h"
#include "RenderEngine/ShaderManager.h"

SelectionRoadProgram::SelectionRoadProgram()
{
    m_point_buffer = 0;
    m_index_buffer = 0;
    m_road_selection_buffer = 0;
    m_program_handle = 0;
    m_vertex_array_object = 0;
}

SelectionRoadProgram::~SelectionRoadProgram()
{
    release_vertex_array();
    if (m_program_handle != 0) glDeleteProgram(m_program_handle);
}

UniformCamera * SelectionRoadProgram::get_camera()
{
    return m_camera;
}

void SelectionRoadProgram::set_camera(UniformCamera * camera)
{
    m_camera = camera;
}

bool SelectionRoadProgram::link_program()
{
    // fetch shaders;
    GLuint l_vertex_shader_handle = ShaderManager::add_shader("./Shaders/SelectionRoadVS.glsl", GL_VERTEX_SHADER);
    GLuint l_fragment_shader_handle = ShaderManager::add_shader("./Shaders/SelectionRoadFS.glsl", GL_FRAGMENT_SHADER);

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

void SelectionRoadProgram::setup_vertex_array()
{
    if (m_vertex_array_object) release_vertex_array();

    // create vertex array object
    glGenVertexArrays(1, &m_vertex_array_object);
    // bind vertex array object
    glBindVertexArray(m_vertex_array_object);

    glGenBuffers(1, &m_point_buffer);
    glGenBuffers(1, &m_index_buffer);
    glGenBuffers(1, &m_road_selection_buffer);

    // vertex coordinates
    glBindVertexBuffer(0, m_point_buffer, 0, 3 * sizeof(float));
    glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
    glEnableVertexAttribArray(0);

    // triangle index
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);

    glBindVertexArray(0);
}

void SelectionRoadProgram::release_vertex_array()
{
    glDeleteVertexArrays(1, &m_vertex_array_object);
    m_vertex_array_object = 0;
}

void SelectionRoadProgram::render()
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

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_road_selection_buffer);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_point_buffer);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_index_buffer);

    glEnable(GL_DEPTH_TEST);
    glLineWidth(6.0);
    glDrawElements(GL_LINES, m_index.size(), GL_UNSIGNED_INT, 0);
    glLineWidth(1.0);
    glDisable(GL_DEPTH_TEST);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

    glUseProgram(0);

    glBindVertexArray(0);

#ifndef NDEBUG
    GLErrorChecker::check_current_gl_status(__FILE__, __LINE__, __FUNCSIG__);
#endif
}

void SelectionRoadProgram::bind_buffer_data()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_point_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_point.size() * sizeof(float), m_point.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_index_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_index.size() * sizeof(int), m_index.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_road_selection_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_selection.size() * sizeof(char), m_selection.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SelectionRoadProgram::update_selection_buffer()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_road_selection_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_selection.size() * sizeof(char), m_selection.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SelectionRoadProgram::reset_selection_buffer()
{
    memset(m_selection.data(), 0, m_selection.size() * sizeof(char));
    update_selection_buffer();
}

void SelectionRoadProgram::load_data()
{
    IO::load_line_data("./Data/road/road.dat", m_point, m_index);
    m_selection.resize(m_index.size() >> 1, 0);
}

void SelectionRoadProgram::road_select(const int x, const int y)
{
    if (m_grid_road_map.empty())
    {
        IO::load_city_grid_data("./Data/road/grid_road_map.dat", m_grid_road_map, true);
    }

    float wx = 0.0, wy = 0.0, wz = 0.0;
    //m_camera->screen_to_world_coordinate(x, y, wx, wy, wz);
    m_camera->screen_to_world_flat_coordinate(x, y, wx, wy, wz);

    const int MIN_X = 558;
    const int MIN_Y = 59180;

    static const int l_width = (int)m_grid_road_map.size();
    static const int l_height = (int)m_grid_road_map[0].size();

    int px = (int)wx - MIN_X, py = (int)wy - MIN_Y;

    const int box_size = 5;
    const float eps = -1e-6;
    int road = -1, min_dis = 100;
    for (int dx = -box_size; dx <= box_size; ++ dx)
    {
        for (int dy = -box_size; dy <= box_size; ++ dy)
        {
            int a = px + dx, b = py + dy;
            if (0 <= a && a < l_width && 0 <= b && b < l_height && m_grid_road_map[a][b] > eps)
            {
                int dis = std::abs(px - a) + std::abs(py - b);
                int l_road_index = (int)m_grid_road_map[a][b];
                if (dis < min_dis)
                {
                    min_dis = dis;
                    road = l_road_index;
                }
            }
        }
    }
    if (road != -1)
    {
        m_selection[road] ^= 1;

        if (m_selection[road] == 1)
        {
            static std::vector<float> weight;
            static std::vector<float> exits;
            if (weight.empty())
            {
                const std::string attribute_weight_path = "./Data/road/attribute_weight.dat";
                IO::load_road_attribute_data(attribute_weight_path, weight, true);
                const std::string attribute_exits_path = "./Data/road/attribute_exits.dat";
                IO::load_road_attribute_data(attribute_exits_path, exits, true);
            }
            int a = m_index[road << 1], b = m_index[road << 1 | 1];

            // test
            //{
                //printf("\n\n\n\n---------------------\n");
                //int gx = 0, gy = 0;
                //float fx = 0, fy = 0, fz = 0;
                //m_camera->screen_to_world_coordinate(x, y, fx, fy, fz);
                //printf("(%d, %d) ==> (%.0f, %.0f, %.0f)\n", x, y, fx, fy, fz);
                //m_camera->world_to_screen_coordinate(fx, fy, fz, gx, gy);
                //printf("(%.0f, %.0f, %.0f) ==> (%d %d)\n", fx, fy, fz, gx, gy);
                //m_camera->screen_to_world_flat_coordinate(x, y, fx, fy, fz);

                //printf("(%.0f, %.0f) --- (%.0f, %.0f)\n", m_point[a * 3], m_point[a * 3 + 1], m_point[b * 3], m_point[b * 3 + 1]);
            //}

            printf("[%d, %d] = (%.2f, %.2f) - (%.2f, %.2f)\n", a, b, m_point[a * 3], m_point[a * 3 + 1], m_point[b * 3], m_point[b * 3 + 1]);
            printf("Select Road : %d, weight = %.2lf, exits = (%d, %d)\n", road, weight[road], (int)exits[a], (int)exits[b]);

        }
        update_selection_buffer();
    }

}

