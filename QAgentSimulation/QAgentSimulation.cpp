#include "QAgentSimulation.h"

#include <freeglut.h>

QAgentSimulation::QAgentSimulation(QWidget *parent): QGLWidget(parent) {}

QAgentSimulation::~QAgentSimulation() {}

void QAgentSimulation::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.8, 0.8, 0.8, 1.0);
}

void QAgentSimulation::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void QAgentSimulation::resizeGL(int width, int height)
{
}

UniformCamera * QAgentSimulation::getCamera()
{
    return m_camera;
}
