#pragma once

#include <GL/glew.h>

#include <QtGui>
#include <QGLWidget>

#include "RenderEngine/UniformCamera.h"

class QAgentSimulation : public QGLWidget
{
    Q_OBJECT

public:
    QAgentSimulation(QWidget *parent = 0);
    ~QAgentSimulation();

    UniformCamera * getCamera();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

private:
    UniformCamera * m_camera;
};

