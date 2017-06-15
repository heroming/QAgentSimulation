#include "QAgentEnergyVis.h"

const QBrush QEnergyVisulization::m_brush_point = QBrush(QColor(21, 81, 77), Qt::SolidPattern);
const QBrush QEnergyVisulization::m_brush_click = QBrush(QColor(243, 156, 18), Qt::SolidPattern);
const QBrush QEnergyVisulization::m_brush_min_point = QBrush(QColor(39, 174, 96), Qt::SolidPattern);
const QBrush QEnergyVisulization::m_brush_background = QBrush(QColor(202, 202, 128), Qt::SolidPattern);

QEnergyVisulization::QEnergyVisulization(QWidget * parent) : QWidget(parent)
{
    setMinimumSize(500, 500);
}

void QEnergyVisulization::paintEvent(QPaintEvent * paint_event)
{
    QPainter painter(this);
    draw_energy(painter);
}

void QEnergyVisulization::set_agent_and_sample(const Agent & man, const Sample & sam)
{
    m_index = 0;
    m_agent = man;
    m_sample = sam;
}

void QEnergyVisulization::draw_energy(QPainter & painter)
{
    // fill rectangle
    const int RADIUS = 5;
    const int WIDTH = (CENTER - MARGIN) * 2;
    painter.fillRect(QRect(MARGIN, MARGIN, WIDTH, WIDTH), m_brush_background);
    painter.setBrush(m_brush_point);

    Point o(CENTER, CENTER);
    for (int i = 0; i < Sample::N; ++ i)
    {
        Point s = m_sample.po[i] - m_sample.po[0];
        Point p = o + Point(s.x, -s.y) * RATE;
        painter.drawEllipse(QPoint(p.x, p.y), RADIUS, RADIUS);
    }

    painter.setBrush(m_brush_click);
    Point s = m_sample.po[m_index] - m_sample.po[0];
    Point p = o + Point(s.x, -s.y) * RATE;
    painter.drawEllipse(QPoint(p.x, p.y), RADIUS, RADIUS);

    // find the min energy point
    int l_min = 0;
    for (int i = 0; i < Sample::N; ++ i)
    {
        if (m_sample.weight_all[i] < m_sample.weight_all[l_min])
        {
            l_min = i;
        }
    }
    
    if (l_min != m_index)
    {
        painter.setBrush(m_brush_min_point);
        Point s = m_sample.po[l_min] - m_sample.po[0];
        Point p = o + Point(s.x, -s.y) * RATE;
        painter.drawEllipse(QPoint(p.x, p.y), RADIUS, RADIUS);
    }
}

void QEnergyVisulization::mousePressEvent(QMouseEvent * eve)
{
    Point o(CENTER, CENTER);
    Point c(eve->x(), eve->y());

    int l_index = 0;
    float l_dis = INF;
    for (int i = 0; i < Sample::N; ++ i)
    {
        Point s = m_sample.po[i] - m_sample.po[0];
        Point p = o + Point(s.x, -s.y) * RATE;
        float d = c.distance(p);
        if (d < l_dis)
        {
            l_dis = d;
            l_index = i;
        }
    }
    m_index = l_index;

    emit signal_choose_sample_point(l_index);

    update();
}

QAgentEnergyVis::QAgentEnergyVis(QWidget * parent) : QWidget(parent)
{
    setFixedSize(900, 550);
    QSpacerItem * spacer1 = new QSpacerItem(80, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    QGroupBox * gb1 = new QGroupBox(tr("可视化结果"), this);
    QVBoxLayout * vlayout1 = new QVBoxLayout(gb1);
    m_vis = new QEnergyVisulization(gb1);
    vlayout1->addWidget(m_vis);

    QGroupBox * gb2 = new QGroupBox(tr("数据"), this);

    m_label_speed = new QLabel(tr("最大速度"), gb2);
    m_label_status = new QLabel(tr("状态"), gb2);
    m_label_energy = new QLabel(tr("能量大小"), gb2);
    m_label_distance = new QLabel(tr("距离能量"), gb2);
    m_label_obstacle = new QLabel(tr("障碍能量"), gb2);
    m_label_agent = new QLabel(tr("人员密度能量"), gb2);

    m_le_speed = new QLineEdit(tr("0.000"), gb2);
    m_le_speed->setEnabled(false);
    m_le_status = new QLineEdit(tr("0.000"), gb2);
    m_le_status->setEnabled(false);
    m_le_energy = new QLineEdit(tr("0.000"), gb2);
    m_le_energy->setEnabled(false);
    m_le_distance = new QLineEdit(tr("0.000"), gb2);
    m_le_distance->setEnabled(false);
    m_le_obstacle = new QLineEdit(tr("0.000"), gb2);
    m_le_obstacle->setEnabled(false);
    m_le_agent = new QLineEdit(tr("0.000"), gb2);
    m_le_agent->setEnabled(false);

    QVBoxLayout * vlayout2 = new QVBoxLayout(gb2);
    {
        QHBoxLayout * hlayout1 = new QHBoxLayout;
        hlayout1->addWidget(m_label_speed);
        hlayout1->addSpacerItem(spacer1);
        hlayout1->addWidget(m_le_speed);

        QHBoxLayout * hlayout2 = new QHBoxLayout;
        hlayout2->addWidget(m_label_status);
        hlayout2->addSpacerItem(spacer1);
        hlayout2->addWidget(m_le_status);

        QHBoxLayout * hlayout3 = new QHBoxLayout;
        hlayout3->addWidget(m_label_energy);
        hlayout3->addSpacerItem(spacer1);
        hlayout3->addWidget(m_le_energy);

        QHBoxLayout * hlayout4 = new QHBoxLayout;
        hlayout4->addWidget(m_label_distance);
        hlayout4->addSpacerItem(spacer1);
        hlayout4->addWidget(m_le_distance);

        QHBoxLayout * hlayout5 = new QHBoxLayout;
        hlayout5->addWidget(m_label_obstacle);
        hlayout5->addSpacerItem(spacer1);
        hlayout5->addWidget(m_le_obstacle);

        QHBoxLayout * hlayout6 = new QHBoxLayout;
        hlayout6->addWidget(m_label_agent);
        hlayout6->addSpacerItem(spacer1);
        hlayout6->addWidget(m_le_agent);

        vlayout2->addLayout(hlayout1);
        vlayout2->addLayout(hlayout2);
        vlayout2->addLayout(hlayout3);
        vlayout2->addLayout(hlayout4);
        vlayout2->addLayout(hlayout5);
        vlayout2->addLayout(hlayout6);
    }

    QVBoxLayout * left_layout = new QVBoxLayout;
    QVBoxLayout * right_layout = new QVBoxLayout;
    QHBoxLayout * main_layout = new QHBoxLayout;

    left_layout->addWidget(gb1);
    right_layout->addWidget(gb2);

    main_layout->addLayout(left_layout);
    main_layout->addLayout(right_layout);
    this->setLayout(main_layout);

    connect(m_vis, SIGNAL(signal_choose_sample_point(int)), this, SLOT(on_choose_sample_point(int)));
}

QAgentEnergyVis::~QAgentEnergyVis()
{
    /*
    delete m_label_speed;
    delete m_label_status;
    delete m_label_distance;
    delete m_label_obstacle;
    delete m_label_damage;
    delete m_label_agent;

    delete m_le_speed;
    delete m_le_status;
    delete m_le_distance;
    delete m_le_obstacle;
    delete m_le_damage;
    delete m_le_agent;

    delete m_vis;
    */
}

void QAgentEnergyVis::set_agent_and_sample(const Agent & man, const Sample & sam)
{
    m_agent = man;
    m_sample = sam;
    m_vis->set_agent_and_sample(man, sam);
    show_data();
}

void QAgentEnergyVis::show_data(const int l_idx)
{
    const float EPS = 1e-6;
    float l_speed = m_agent.max_velocity;
    if (std::abs(l_speed) < EPS) l_speed = 0.0;
    m_le_speed->setText(QString::number(l_speed, 'f', 6));
    QString l_status = "";
    switch (m_agent.status)
    {
    case NOTHING :
        l_status = "NOTHING";
        break;
    case RUNNING :
        l_status = "NOTHING";
        break;
    case ARRIVED :
        l_status = "ARRIVED";
        break;
    case BLOCKED :
        l_status = "BLOCKED";
        break;
    case DEAD :
        l_status = "DEAD";
        break;
    case ARRIVING :
        l_status = "ARRIVING";
        break;
    }
    m_le_status->setText(l_status);
    float l_energy = m_sample.weight_all[l_idx];
    if (std::abs(l_energy) < EPS) l_energy = 0.0;
    float l_dis = m_sample.distance_weight(m_sample.dis[l_idx]);
    //float l_dis = m_sample.dis[l_idx];
    if (std::abs(l_dis) < EPS) l_dis = 0.0;
    float l_obs = m_sample.weight[l_idx] - l_dis;
    if (std::abs(l_obs) < EPS) l_obs = 0.0;
    float l_agent = m_sample.weight_all[l_idx] - m_sample.weight[l_idx];
    if (std::abs(l_agent) < EPS) l_agent = 0.0;
    m_le_energy->setText(QString::number(l_energy, 'f', 6));
    m_le_distance->setText(QString::number(l_dis, 'f', 6));
    m_le_obstacle->setText(QString::number(l_obs, 'f', 6));
    m_le_agent->setText(QString::number(l_agent, 'f', 6));
}

void QAgentEnergyVis::on_choose_sample_point(int l_index)
{
    show_data(l_index);
}

