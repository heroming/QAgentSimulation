#pragma once

#include <GL/glew.h>

#include <QtGui>
#include <QGLWidget>

#include "RenderEngine/UniformCamera.h"
#include "RenderEngine/CityBuildingProgram.h"

class QAgentSimulation : public QGLWidget
{
    Q_OBJECT

public:
    QAgentSimulation(QWidget *parent = 0);
    ~QAgentSimulation();

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

    UniformCamera m_camera;
    CityBuildingProgram m_city;
};

