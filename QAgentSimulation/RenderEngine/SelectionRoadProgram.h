#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>

class UniformCamera;

class SelectionRoadProgram
{
public:
    SelectionRoadProgram();
    ~SelectionRoadProgram();

    UniformCamera * get_camera();
    void set_camera(UniformCamera * camera);

    bool link_program();
    void setup_vertex_array();
    void release_vertex_array();
    void bind_buffer_data();
    void render();

    void load_data();
    void road_select(const int x, const int y);
    void update_selection_buffer();
    void reset_selection_buffer();

private:

    UniformCamera * m_camera;

    GLuint m_point_buffer;
    GLuint m_index_buffer;
    GLuint m_road_selection_buffer;
    GLuint m_program_handle;
    GLuint m_vertex_array_object;

    std::vector<int> m_index;
    std::vector<float> m_point;
    std::vector<char> m_selection;
    std::vector<std::vector<float>> m_grid_road_map;
};