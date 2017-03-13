#include "QAgentSimulation.h"
#include "Algorithm.h"

#include <freeglut.h>

QAgentSimulation::QAgentSimulation(QWidget *parent): QGLWidget(parent)
{
    //Algorithm::build_city_grid_map();
    //Algorithm::build_city_damage_map();
    //Algorithm::find_the_main_road_by_data_road_information();
    //Algorithm::find_the_main_road_by_agent_movement();
    //Algorithm::build_city_road_network();
    //Algorithm::filter_main_road_by_road_weight_and_heat();
    //Algorithm::connect_main_road_network();
    //Algorithm::map_city_grid_and_road();
    //Algorithm::calculate_road_statistic_information();
    //Algorithm::filter_main_road_by_road_weight();
    //Algorithm::map_road_end_point_to_city_map_with_clearance();

    m_show_agent = false;
    m_show_road = true;
    m_show_main_road = false;
    m_show_selection_road = false;

    m_animation_play = false;
    m_animation_speed = 256;

    m_city.set_camera(&m_camera);
    m_city.load_data();
    m_river.set_camera(&m_camera);
    m_river.load_data();
    m_shelter.set_camera(&m_camera);
    m_shelter.load_data();
    m_agent.set_camera(&m_camera);
    m_agent.load_data();
    m_road.set_camera(&m_camera);
    m_road.load_data();
    m_main_road.set_camera(&m_camera);
    m_main_road.load_data();
    m_selection_road.set_camera(&m_camera);
    m_selection_road.load_data();
    m_camera.load_default_status();
    m_camera.set_perspective(true);
}

QAgentSimulation::~QAgentSimulation() {}

void QAgentSimulation::initializeGL()
{
    makeCurrent();

    // init glew
    GLenum l_glew_state = glewInit();
    if (l_glew_state != GLEW_OK) QMessageBox::warning(this, tr("GLEW"), tr("³õÊ¼»¯´íÎó£¡"));

    glClearColor(1.0, 1.0, 1.0, 1.0);

    m_city.link_program();
    m_city.setup_vertex_array();
    m_city.bind_buffer_data();

    m_river.link_program();
    m_river.setup_vertex_array();
    m_river.bind_buffer_data();

    m_shelter.link_program();
    m_shelter.setup_vertex_array();
    m_shelter.bind_buffer_data();

    m_agent.link_program();
    m_agent.setup_vertex_array();
    m_agent.bind_buffer_data();

    m_road.link_program();
    m_road.setup_vertex_array();
    m_road.bind_buffer_data();

    m_main_road.link_program();
    m_main_road.setup_vertex_array();
    m_main_road.bind_buffer_data();

    m_selection_road.link_program();
    m_selection_road.setup_vertex_array();
    m_selection_road.bind_buffer_data();
}

void QAgentSimulation::paintGL()
{
    makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_city.render();
    m_river.render();
    m_shelter.render();

    if (m_show_agent) m_agent.render();
    if (m_show_road) m_road.render();
    if (m_show_main_road) m_main_road.render();
    if (m_show_selection_road) m_selection_road.render();
}

void QAgentSimulation::resizeGL(int width, int height)
{
    makeCurrent();
    m_camera.set_viewport(0, 0, width, height);
    m_width = width;
    m_height = height;
}

void QAgentSimulation::wheelEvent(QWheelEvent * eve)
{
    float numDegrees = eve->delta() / 8;
    float factor = (numDegrees + 180.0f)/180.0f;
    m_camera.scale(factor);
    update();
}

void QAgentSimulation::mouseMoveEvent(QMouseEvent * eve)
{
    int l_x = eve->x();
    int l_y = eve->y();
    switch (eve->buttons())
    {
    case Qt::LeftButton:
        {
            m_camera.rotate(m_mouse_x, m_mouse_y, l_x, l_y);
        }
        break;
    case Qt::MidButton:
        {
            m_camera.translate(m_mouse_x, m_mouse_y, l_x, l_y);
        }
        break;
    case Qt::RightButton:
    case Qt::NoButton:
    default:
        break;
    }
    m_mouse_x = l_x;
    m_mouse_y = l_y;
    update();
}

void QAgentSimulation::mousePressEvent(QMouseEvent * eve)
{
    m_mouse_x = eve->x();
    m_mouse_y = eve->y();
    switch (eve->buttons())
    {
    case Qt::LeftButton:
        {
            if (m_show_selection_road)
            {
                m_selection_road.road_select(m_mouse_x, m_mouse_y);
                update();
            }
        }
        break;
    case Qt::MidButton:
    case Qt::RightButton:
    case Qt::NoButton:
    default:
        break;
    }
}

void QAgentSimulation::mouseReleaseEvent(QMouseEvent * eve)
{
    m_mouse_x = eve->x();
    m_mouse_y = eve->y();
}

bool QAgentSimulation::is_show_agent() const
{
    return m_show_agent;
}

void QAgentSimulation::set_show_agent(bool flag)
{
    m_show_agent = flag;
    if (!m_show_agent)
    {
        m_agent.bind_buffer_data();
    }
    update();
}

bool QAgentSimulation::is_show_road() const
{
    return m_show_road;
}

void QAgentSimulation::set_show_road(bool flag)
{
    m_show_road = flag;
    update();
}

bool QAgentSimulation::is_show_main_road() const
{
    return m_show_main_road;
}

void QAgentSimulation::set_show_main_road(bool flag)
{
    m_show_main_road = flag;
    update();
}

bool QAgentSimulation::is_show_selection_road() const
{
    return m_show_selection_road;
}

void QAgentSimulation::set_show_selection_road(bool flag)
{
    m_show_selection_road = flag;
    if (!m_show_selection_road)
    {
        m_selection_road.reset_selection_buffer();
    }
    update();
}

void QAgentSimulation::animation_play(QTimer * timer)
{
    m_animation_play = !m_animation_play;
    if (m_animation_play)
    {
        timer->start(m_animation_speed);
    }
    else
    {
        timer->stop();
    }
}

void QAgentSimulation::animation_next(QTimer * timer)
{
    if (m_animation_play)
    {
        if (m_animation_speed > 4) m_animation_speed >>= 1;
        timer->start(m_animation_speed);
    }
    else
    {
        m_agent.bind_next();
    }
    update();
}

void QAgentSimulation::animation_previous(QTimer * timer)
{
    if (m_animation_play)
    {
        if (m_animation_speed < 4096) m_animation_speed <<= 1;
        timer->start(m_animation_speed);
    }
    else
    {
        m_agent.bind_prevois();
    }
    update();
}

bool QAgentSimulation::animation_timeout(QTimer * timer)
{
    if (!m_agent.bind_next())
    {
        timer->stop();
        m_agent.bind_buffer_data();
        update();
        return true;
    }
    update();
    return false;
}




