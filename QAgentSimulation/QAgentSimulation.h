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

    void animation_play(QTimer * timer);
    void animation_next(QTimer * timer);
    void animation_previous(QTimer * timer);
    bool animation_timeout(QTimer * timer);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void wheelEvent(QWheelEvent * eve);
    void mouseMoveEvent(QMouseEvent * eve);
    void mousePressEvent(QMouseEvent * eve);
    void mouseReleaseEvent(QMouseEvent * eve);

private:
    int m_width;
    int m_height;

    int m_mouse_x;
    int m_mouse_y;

    bool m_show_agent;
    bool m_show_road;
    bool m_show_main_road;
    bool m_show_selection_road;

    bool m_animation_play;
    int m_animation_speed;

    UniformCamera m_camera;

    CityBuildingProgram m_city;
    RiverProgram m_river;
    ShelterProgram m_shelter;
    AgentProgram m_agent;
    RoadProgram m_road;
    MainRoadProgram m_main_road;
    SelectionRoadProgram m_selection_road;

};

