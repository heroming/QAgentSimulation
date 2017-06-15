#pragma once

#include <QThread>
#include <QtGui>
#include <QDialog>
#include "Simulation.h"

class QEnergyVisulization : public QWidget
{
    Q_OBJECT

public :
    QEnergyVisulization(QWidget * parent = NULL);
    ~QEnergyVisulization() {};

public :

    void paintEvent(QPaintEvent * paint_event);
    void draw_energy(QPainter & painter);
    void set_agent_and_sample(const Agent & man, const Sample & sam);

protected :

    void mousePressEvent(QMouseEvent * eve);

signals :

    void signal_choose_sample_point(int l_index);

public :

    static const int RATE = 60;
    static const int MARGIN = 20;
    static const int CENTER = 250;

    static const QBrush m_brush_point;
    static const QBrush m_brush_click;
    static const QBrush m_brush_min_point;
    static const QBrush m_brush_background;

    int m_index;
    Agent m_agent;
    Sample m_sample;
};


class QAgentEnergyVis : public QWidget
{
    Q_OBJECT

public :
    QAgentEnergyVis(QWidget * parent = NULL);
    ~QAgentEnergyVis();

public :
    void show_data(const int l_idx = 0);
    void set_agent_and_sample(const Agent & man, const Sample & sam);

public slots :

    void on_choose_sample_point(int l_index);

public :

    QLabel * m_label_speed;
    QLabel * m_label_status;
    QLabel * m_label_energy;
    QLabel * m_label_distance;
    QLabel * m_label_obstacle;
    QLabel * m_label_agent;

    QLineEdit * m_le_speed;
    QLineEdit * m_le_status;
    QLineEdit * m_le_energy;
    QLineEdit * m_le_distance;
    QLineEdit * m_le_obstacle;
    QLineEdit * m_le_agent;

    QEnergyVisulization * m_vis;

    Agent m_agent;
    Sample m_sample;
};