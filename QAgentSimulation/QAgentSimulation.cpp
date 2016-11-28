#include "QAgentSimulation.h"

#include <freeglut.h>

QAgentSimulation::QAgentSimulation(QWidget *parent): QGLWidget(parent)
{
    m_city.set_camera(&m_camera);
    m_city.load_city_building();
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

    glClearColor(0.40, 0.41, 0.40, 1.0);

    m_city.link_program();
    m_city.setup_vertex_array();
    m_city.bind_buffer_data();
}

void QAgentSimulation::paintGL()
{
    makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_city.render();
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
    case Qt::MouseButton::LeftButton:
        {
            m_camera.rotate(m_mouse_x, m_mouse_y, l_x, l_y);
        }
        break;
    case Qt::MouseButton::MidButton:
        {
            m_camera.translate(m_mouse_x, m_mouse_y, l_x, l_y);
        }
        break;
    case Qt::MouseButton::RightButton:
    case Qt::MouseButton::NoButton:
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
}

void QAgentSimulation::mouseReleaseEvent(QMouseEvent * eve)
{
    m_mouse_x = eve->x();
    m_mouse_y = eve->y();
}
