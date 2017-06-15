#include "QAgentSimulation.h"
#include "Algorithm.h"
#include "QDialogCreatRoad.h"

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
    //Algorithm::calculate_shelter_influence();
    //Algorithm::calculate_space_clearance();
    //Algorithm::calculate_damage_clearance();
    //Algorithm::calculate_shelter_clearance();
    //Algorithm::connect_main_road_and_detailed_road();

    //Algorithm::add_detailed_road();
    //Algorithm::pre_get_agent_timestep_data();
    //Algorithm::pre_calculate_shelter_infomation();
    //Algorithm::pre_calculate_road_distance_and_width();
    //Algorithm::pre_calculate_orignal_shelter_influence();
    //Algorithm::pre_calculate_my_shelter_influence();

    //Algorithm::get_agent_subset();
    //Algorithm::get_init_agent();
    //Algorithm::get_agent_information();

    m_show_agent = false;
    m_show_road = false;
    m_show_main_road = false;
    m_show_selection_road = false;
    m_is_create_road = false;

    m_animation_play = false;
    m_animation_speed = 1024;

    m_is_run_navigation = false;
    m_is_run_agent_path = false;
    m_is_run_agent_energy = false;
    m_is_run_step = false;

    m_agent_energy_vis = new QAgentEnergyVis;

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

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));
}

QAgentSimulation::~QAgentSimulation()
{
    //delete m_agent_energy_vis;
}

void QAgentSimulation::initializeGL()
{
    makeCurrent();

    // init glew
    GLenum l_glew_state = glewInit();
    if (l_glew_state != GLEW_OK) QMessageBox::warning(this, tr("GLEW"), tr("初始化错误！"));

    glClearColor(1.0, 1.0, 1.0, 0.8);

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

    if (m_show_selection_road) draw_selected_road();  
    if (m_is_create_road) draw_created_road();
    if (m_is_run_navigation) draw_run_navigation();
    if (m_is_run_agent_path) draw_run_agent_path();
    if (m_is_run_agent_energy) draw_run_agent_energy();

    if (m_show_agent) m_agent.render();
    if (m_show_road) m_road.render();
    if (m_show_main_road || m_is_create_road) m_main_road.render();
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
            // 在创建道路时，为了2D坐标转换3D坐标的精确性，暂时不能旋转
            if (! m_is_create_road)
            {
                m_camera.rotate(m_mouse_x, m_mouse_y, l_x, l_y);
            }
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

            if (m_is_create_road)
            {
                float l_x, l_y, l_z;
                m_camera.screen_to_world_flat_coordinate(m_mouse_x, m_mouse_y, l_x, l_y, l_z);
                m_create_road.point_clicked(l_x, l_y);
                const std::vector<int> & l_index = m_create_road.get_index();
                const std::vector<float> & l_point = m_create_road.get_point();
                m_main_road.update_date(l_index, l_point);
                m_main_road.bind_buffer_data();

                update();
            }

            if (m_is_run_navigation)
            {
                float l_x, l_y, l_z;
                m_camera.screen_to_world_flat_coordinate(m_mouse_x, m_mouse_y, l_x, l_y, l_z);
                printf("Navigation : (%.2f, %.2f)\n", l_x, l_y);
                m_simulation.get_road_to_shelter(m_navigation_vertex, m_navigation_main, m_navigation_weight, l_x, l_y);

                update();
            }

            if (m_is_run_agent_path)
            {
                float l_x, l_y, l_z;
                m_camera.screen_to_world_flat_coordinate(m_mouse_x, m_mouse_y, l_x, l_y, l_z);
                printf("Agent : (%.2f, %.2f)\n", l_x, l_y);
                m_simulation.get_agent_road_to_shelter(m_navigation_vertex, m_navigation_main, m_navigation_weight, l_x, l_y);

                update();
            }

            if (m_is_run_agent_energy)
            {
                float l_x, l_y, l_z;
                m_camera.screen_to_world_flat_coordinate(m_mouse_x, m_mouse_y, l_x, l_y, l_z);
                printf("Agent energy : (%.2f, %.2f)\n", l_x, l_y);
                m_simulation.get_agent_energy(m_energy_agent, m_energy_sample, l_x, l_y);
                m_agent_energy_vis->set_agent_and_sample(m_energy_agent, m_energy_sample);
                m_agent_energy_vis->show();
                m_agent_energy_vis->update();
                update();
            }
        }
        break;
    case Qt::MidButton:
        {
            if (m_is_create_road)
            {
                m_create_road.set_begin(true);
            }
        }
        break;
    case Qt::RightButton:
        {
            if (m_is_create_road)
            {
                m_create_road.delete_recent();
                const std::vector<int> & l_index = m_create_road.get_index();
                const std::vector<float> & l_point = m_create_road.get_point();
                m_main_road.update_date(l_index, l_point);
                m_main_road.bind_buffer_data();
                update();
            }
        }
        break;
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

void QAgentSimulation::draw_selected_road()
{
    glEnable(GL_POINT_SIZE);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(m_camera.get_projection_matrix());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(m_camera.get_model_view_matrix());

    glPopMatrix();

    glPointSize(1.0f);
    glDisable(GL_POINT_SIZE);
}

void QAgentSimulation::draw_created_road()
{
    glEnable(GL_POINT_SIZE);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(m_camera.get_projection_matrix());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(m_camera.get_model_view_matrix());

    glPointSize(10.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.4f, 0.4f);
    glVertex2f(m_create_road.get_x(), m_create_road.get_y());
    glEnd();

    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glColor3f(0.13f, 0.22f, 0.27f);
    const std::vector<float> & points = m_create_road.get_point();
    int point_size = (int)points.size();
    for (int i = 0; i < point_size; i += 3)
    {
        glVertex2f(points[i], points[i + 1]);
    }
    glEnd();

    glPopMatrix();

    glPointSize(1.0f);
    glDisable(GL_POINT_SIZE);
}

void QAgentSimulation::draw_run_navigation()
{
    if (m_navigation_vertex.size() < 2 || m_navigation_main.empty() || m_navigation_weight.empty()) return;

    glEnable(GL_POINT_SIZE);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(m_camera.get_projection_matrix());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(m_camera.get_model_view_matrix());

    // draw vertexs
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.5f, 0.4f);
    glVertex2f(m_navigation_vertex[0].x, m_navigation_vertex[0].y);
    glColor3f(0.5f, 1.0f, 0.4f);
    glVertex2f(m_navigation_vertex.back().x, m_navigation_vertex.back().y);
    glEnd();

    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glColor3f(0.53f, 0.62f, 0.87f);
    for (int i = 1; i + 1 < (int)m_navigation_vertex.size(); ++ i)
    {
        glVertex2f(m_navigation_vertex[i].x, m_navigation_vertex[i].y);
    }
    glEnd();

    // draw edges
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < (int)m_navigation_vertex.size(); ++ i)
    {
        if (i)
        {
            bool l_main = m_navigation_main[i - 1];
            float l_weight = m_navigation_weight[i - 1];
            if (l_main)
            {
                glLineWidth(5.0f);
                glColor3f(0.31f, 0.82f, 0.27f);
            }
            else
            {
                glLineWidth(3.0f);
                glColor3f(0.11f, 0.32f, 0.67f);
            }
        }
        glVertex2f(m_navigation_vertex[i].x, m_navigation_vertex[i].y);
    }
    glEnd();

    glPopMatrix();

    glLineWidth(1.0f);
    glPointSize(1.0f);
    glDisable(GL_POINT_SIZE);
}

void QAgentSimulation::draw_run_agent_path()
{
    if (m_navigation_vertex.size() < 2 || m_navigation_main.empty() || m_navigation_weight.empty()) return;

    glEnable(GL_POINT_SIZE);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(m_camera.get_projection_matrix());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(m_camera.get_model_view_matrix());

    // draw vertexs
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.5f, 0.4f);
    glVertex2f(m_navigation_vertex[0].x, m_navigation_vertex[0].y);
    glColor3f(0.5f, 1.0f, 0.4f);
    glVertex2f(m_navigation_vertex.back().x, m_navigation_vertex.back().y);
    glEnd();

    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glColor3f(0.53f, 0.62f, 0.87f);
    for (int i = 1; i + 1 < (int)m_navigation_vertex.size(); ++ i)
    {
        glVertex2f(m_navigation_vertex[i].x, m_navigation_vertex[i].y);
    }
    glEnd();

    // draw edges
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < (int)m_navigation_vertex.size(); ++ i)
    {
        if (i)
        {
            bool l_main = m_navigation_main[i - 1];
            float l_weight = m_navigation_weight[i - 1];
            if (l_main)
            {
                glLineWidth(5.0f);
                glColor3f(0.31f, 0.82f, 0.27f);
            }
            else
            {
                glLineWidth(3.0f);
                glColor3f(0.11f, 0.32f, 0.67f);
            }
        }
        glVertex2f(m_navigation_vertex[i].x, m_navigation_vertex[i].y);
    }
    glEnd();

    glPopMatrix();

    glLineWidth(1.0f);
    glPointSize(1.0f);
    glDisable(GL_POINT_SIZE);
}

void QAgentSimulation::draw_run_agent_energy()
{
    glEnable(GL_POINT_SIZE);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMultMatrixf(m_camera.get_projection_matrix());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixf(m_camera.get_model_view_matrix());

    // draw agent
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    glColor3f(0.95f, 0.61, 0.07f);
    glVertex2f(m_energy_agent.p.x, m_energy_agent.p.y);
    glColor3f(0.07, 0.61, 0.95);
    glVertex2f(m_energy_agent.cp.x, m_energy_agent.cp.y);
    glEnd();

    glPopMatrix();

    glPointSize(1.0f);
    glDisable(GL_POINT_SIZE);
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

bool QAgentSimulation::is_creat_road() const
{
    return m_is_create_road;
}

void QAgentSimulation::set_creat_road(bool flag)
{
    m_is_create_road = flag;
    update();
}

void QAgentSimulation::create_road()
{
    m_create_road.clear();
    QDialogCreatRoad * dialog = new QDialogCreatRoad;
    if (dialog->exec())
    {
        m_create_road.set_save_path(dialog->get_save_path());
        m_create_road.set_init_path(dialog->get_init_path());
        m_create_road.set_radius(dialog->get_radius());
        m_create_road.set_length(dialog->get_length());
        m_create_road.load_data(m_create_road.get_init_path());
        m_main_road.load_data(m_create_road.get_init_path());
        m_main_road.bind_buffer_data();
        set_creat_road(true);
    }
}

void QAgentSimulation::create_road_finish()
{
    set_creat_road(false);
    m_create_road.save();
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
    if (m_is_run_step)
    {
        m_simulation.next_step();
        update();
    }
    else
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

void QAgentSimulation::run_simulation(const bool flag)
{
    m_agent.start_simulation(flag);
    //m_simulation.initialize();
    //m_simulation.simulation();
    if (flag)
    {
        const int WAIT_TIME = 50;
        m_timer.start(WAIT_TIME);
        m_simulation.start();
    }
    else
    {
        m_simulation.finish();
    }
}

void QAgentSimulation::on_timer_timeout()
{
    int s = m_agent.get_timestamp();
    int t = m_simulation.get_timestamp();
    //printf("%d -- %d\n", s, t);
    if (s + 1 < t)
    {
        const std::vector<float> & l_result = m_simulation.get_timestamp_data(s + 1);
        //printf("-- %d --\n", l_result.size());

        m_agent.update(l_result);
        update();
    }
}

void QAgentSimulation::run_navigation(const bool flag)
{
    if (m_simulation.is_initialized())
    {
        m_is_run_navigation = flag;
        update();
    }

}

void QAgentSimulation::run_agent_path(const bool flag)
{
    if (m_simulation.is_initialized())
    {
        m_is_run_agent_path = flag;
        update();
    }
}

void QAgentSimulation::run_agent_energy(const bool flag)
{
    if (m_simulation.is_initialized())
    {
        m_is_run_agent_energy = flag;
        update();
    }
}

void QAgentSimulation::run_step(const bool flag)
{
    m_is_run_step = flag;
    m_simulation.set_run_step(flag);
}

