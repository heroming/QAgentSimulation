#pragma once

#include <GL/glew.h>

#include <QtGui>
#include <QGLWidget>

#include "RenderEngine/UniformCamera.h"
#include "RenderEngine/CityBuildingProgram.h"
#include "RenderEngine/RiverProgram.h"
#include "RenderEngine/ShelterProgram.h"
#include "RenderEngine/AgentProgram.h"
#include "RenderEngine/MainRoadProgram.h"
#include "RenderEngine/RoadProgram.h"
#include "RenderEngine/SelectionRoadProgram.h"

#include "CreateRoad.h"
#include "Simulation.h"
#include "QAgentEnergyVis.h"

class QAgentSimulation : public QGLWidget
{
    Q_OBJECT

public:
    QAgentSimulation(QWidget *parent = 0);
    ~QAgentSimulation();

    bool is_show_agent() const;
    void set_show_agent(bool flag);

    bool is_show_road() const;
    void set_show_road(bool flag);

    bool is_show_main_road() const;
    void set_show_main_road(bool flag);

    bool is_show_selection_road() const;
    void set_show_selection_road(bool flag);

    bool is_creat_road() const;
    void set_creat_road(bool flag);
    void create_road();
    void create_road_finish();

    void animation_play(QTimer * timer);
    void animation_next(QTimer * timer);
    void animation_previous(QTimer * timer);
    bool animation_timeout(QTimer * timer);

    void run_simulation(const bool flag);
    void run_navigation(const bool flag);
    void run_agent_path(const bool flag);
    void run_agent_energy(const bool flag);
    void run_step(const bool flag);

public slots:
    void on_timer_timeout();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void wheelEvent(QWheelEvent * eve);
    void mouseMoveEvent(QMouseEvent * eve);
    void mousePressEvent(QMouseEvent * eve);
    void mouseReleaseEvent(QMouseEvent * eve);

private :
    void draw_selected_road();
    void draw_created_road();
    void draw_run_navigation();
    void draw_run_agent_path();
    void draw_run_agent_energy();

private:

    int m_width;
    int m_height;

    int m_mouse_x;
    int m_mouse_y;

    bool m_show_agent;
    bool m_show_road;
    bool m_show_main_road;
    bool m_show_selection_road;

    bool m_is_create_road;
    CreateRoad m_create_road;

    bool m_animation_play;
    int m_animation_speed;

    bool m_is_run_navigation;
    bool m_is_run_agent_path;
    bool m_is_run_agent_energy;
    bool m_is_run_step;

    QTimer m_timer;
    Simulation m_simulation;

    UniformCamera m_camera;

    CityBuildingProgram m_city;
    RiverProgram m_river;
    ShelterProgram m_shelter;
    AgentProgram m_agent;
    RoadProgram m_road;
    MainRoadProgram m_main_road;
    SelectionRoadProgram m_selection_road;


    // For Debug
    std::vector<Point> m_navigation_vertex;
    std::vector<bool> m_navigation_main;
    std::vector<float> m_navigation_weight;

    Agent m_energy_agent;
    Sample m_energy_sample;

    QAgentEnergyVis * m_agent_energy_vis;
};

