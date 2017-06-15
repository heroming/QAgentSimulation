#include <QtGui>

#include "QAgentSimulationWidget.h"
#include "QAgentSimulation.h"


QAgentSimulationWidget::QAgentSimulationWidget(QWidget * parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
    m_timer = new QTimer;
    m_simulation = new QAgentSimulation(this);
    setCentralWidget(m_simulation);

    init_actions();
    init_message_maps();
    init_menus();
    init_tool_bars();

    this->statusBar();
}

QAgentSimulationWidget::~QAgentSimulationWidget() {}

void QAgentSimulationWidget::init_actions()
{
    m_action_show_agent = new QAction(QIcon("Resources/agent.png"), tr("��ʾ������Ա"), this);
    m_action_show_agent->setStatusTip(tr("��ʾ������Ա"));
    m_action_show_agent->setToolTip(tr("��ʾ������Ա"));
    m_action_show_agent->setCheckable(true);
    m_action_show_agent->setChecked(false);

    m_action_show_road = new QAction(QIcon("Resources/road.png"), tr("��ʾ��·"), this);
    m_action_show_road->setStatusTip(tr("��ʾ��·"));
    m_action_show_road->setToolTip(tr("��ʾ��·"));
    m_action_show_road->setCheckable(true);
    m_action_show_road->setChecked(false);

    m_action_show_main_road = new QAction(QIcon("Resources/main_road.png"), tr("��ʾ��Ҫ��·"), this);
    m_action_show_main_road->setStatusTip(tr("��ʾ��Ҫ��·"));
    m_action_show_main_road->setToolTip(tr("��ʾ��Ҫ��·"));
    m_action_show_main_road->setCheckable(true);
    m_action_show_main_road->setChecked(false);

    m_action_show_selection_road = new QAction(QIcon("Resources/selection.png"), tr("ѡ���·"), this);
    m_action_show_selection_road->setStatusTip(tr("ѡ���·"));
    m_action_show_selection_road->setToolTip(tr("ѡ���·"));
    m_action_show_selection_road->setCheckable(true);
    m_action_show_selection_road->setChecked(false);

    m_action_animation_play = new QAction(QIcon("Resources/play.png"), tr("���Ŷ���"), this);
    m_action_animation_play->setStatusTip(tr("���Ŷ���"));
    m_action_animation_play->setToolTip(tr("���Ŷ���"));
    m_action_animation_play->setCheckable(true);

    m_action_animation_next = new QAction(QIcon("Resources/next.png"), tr("��һ֡"), this);
    m_action_animation_next->setStatusTip(tr("��һ֡"));
    m_action_animation_next->setToolTip(tr("��һ֡"));

    m_action_animation_previous = new QAction(QIcon("Resources/previous.png"), tr("��һ֡"), this);
    m_action_animation_previous->setStatusTip(tr("��һ֡"));
    m_action_animation_previous->setToolTip(tr("��һ֡"));

    m_action_run_simulation = new QAction(QIcon("Resources/run.png"), tr("��ʼ�����滮"), this);
    m_action_run_simulation->setStatusTip(tr("��ʼ�����滮"));
    m_action_run_simulation->setToolTip(tr("��ʼ�����滮"));
    m_action_run_simulation->setCheckable(true);
    m_action_run_simulation->setChecked(false);

    m_action_run_navigation = new QAction(QIcon("Resources/navigation.png"), tr("�鿴ĳ����������·��"), this);
    m_action_run_navigation->setStatusTip(tr("�鿴ĳ����������·��"));
    m_action_run_navigation->setToolTip(tr("�鿴ĳ����������·��"));
    m_action_run_navigation->setCheckable(true);
    m_action_run_navigation->setChecked(false);

    m_action_run_agent_path = new QAction(QIcon("Resources/agent_path.png"), tr("�鿴������ʵ������·��"), this);
    m_action_run_agent_path->setStatusTip(tr("�鿴������ʵ������·��"));
    m_action_run_agent_path->setToolTip(tr("�鿴������ʵ������·��"));
    m_action_run_agent_path->setCheckable(true);
    m_action_run_agent_path->setChecked(false);

    m_action_run_agent_energy = new QAction(QIcon("Resources/agent_energy.png"), tr("�鿴�������������ܳ���С"), this);
    m_action_run_agent_energy->setStatusTip(tr("�鿴�������������ܳ���С"));
    m_action_run_agent_energy->setToolTip(tr("�鿴�������������ܳ���С"));
    m_action_run_agent_energy->setCheckable(true);
    m_action_run_agent_energy->setChecked(false);

    m_action_run_step = new QAction(QIcon("Resources/step_next.png"), tr("��һʱ�䲽"), this);
    m_action_run_step->setStatusTip(tr("��һʱ�䲽"));
    m_action_run_step->setToolTip(tr("��һʱ�䲽"));
    m_action_run_step->setCheckable(true);
    m_action_run_step->setChecked(false);

    m_action_creat_road = new QAction(QIcon("Resources/creat_road.png"), tr("������·"), this);
    m_action_creat_road->setStatusTip(tr("������·"));
    m_action_creat_road->setToolTip(tr("������·"));
    m_action_creat_road->setCheckable(true);

}

void QAgentSimulationWidget::init_message_maps()
{
    connect(m_timer, SIGNAL(timeout()), this, SLOT(on_timer_timeout()));

    connect(m_action_show_agent, SIGNAL(triggered()), this, SLOT(on_action_show_agent()));
    connect(m_action_show_road, SIGNAL(triggered()), this, SLOT(on_action_show_road()));
    connect(m_action_show_main_road, SIGNAL(triggered()), this, SLOT(on_action_show_main_road()));
    connect(m_action_show_selection_road, SIGNAL(triggered()), this, SLOT(on_action_show_selection_road()));

    connect(m_action_animation_play, SIGNAL(triggered()), this, SLOT(on_action_animation_play()));
    connect(m_action_animation_next, SIGNAL(triggered()), this, SLOT(on_action_animation_next()));
    connect(m_action_animation_previous, SIGNAL(triggered()), this, SLOT(on_action_animation_previous()));

    connect(m_action_run_simulation, SIGNAL(triggered()), this, SLOT(on_action_run_simulation()));
    connect(m_action_run_navigation, SIGNAL(triggered()), this, SLOT(on_action_run_navigation()));
    connect(m_action_run_agent_path, SIGNAL(triggered()), this, SLOT(on_action_run_agent_path()));
    connect(m_action_run_agent_energy, SIGNAL(triggered()), this, SLOT(on_action_run_agent_energy()));
    connect(m_action_run_step, SIGNAL(triggered()), this, SLOT(on_action_run_step()));

    connect(m_action_creat_road, SIGNAL(triggered()), this, SLOT(on_action_creat_road()));
}

void QAgentSimulationWidget::init_menus()
{
    m_menubar = menuBar();
    m_menu_file = m_menubar->addMenu(tr("�ļ�(&F)"));
    m_menu_init = m_menubar->addMenu(tr("��ʼ��(&I)"));
    m_menu_init->addAction(m_action_creat_road);
}

void QAgentSimulationWidget::init_tool_bars()
{
    m_toolbar_view_option = this->addToolBar(tr("��ʾѡ��"));
    m_toolbar_view_option->setObjectName("��ʾѡ��");
    m_toolbar_view_option->addAction(m_action_show_agent);
    m_toolbar_view_option->addAction(m_action_show_road);
    m_toolbar_view_option->addAction(m_action_show_main_road);
    m_toolbar_view_option->addAction(m_action_show_selection_road);

    m_toolbar_animation_option = this->addToolBar(tr("����ѡ��"));
    m_toolbar_animation_option->setObjectName("����ѡ��");
    m_toolbar_animation_option->addAction(m_action_animation_previous);
    m_toolbar_animation_option->addAction(m_action_animation_play);
    m_toolbar_animation_option->addAction(m_action_animation_next);

    m_toolbar_animation_option = this->addToolBar(tr("����ģ��"));
    m_toolbar_animation_option->setObjectName("����ģ��");
    m_toolbar_animation_option->addAction(m_action_run_simulation);
    m_toolbar_animation_option->addAction(m_action_run_navigation);
    m_toolbar_animation_option->addAction(m_action_run_agent_path);
    m_toolbar_animation_option->addAction(m_action_run_agent_energy);
    m_toolbar_animation_option->addAction(m_action_run_step);
}

void QAgentSimulationWidget::on_action_show_agent()
{
    m_simulation->set_show_agent(m_action_show_agent->isChecked());
}

void QAgentSimulationWidget::on_action_show_road()
{
    m_simulation->set_show_road(m_action_show_road->isChecked());
}

void QAgentSimulationWidget::on_action_show_main_road()
{
    m_simulation->set_show_main_road(m_action_show_main_road->isChecked());
}

void QAgentSimulationWidget::on_action_show_selection_road()
{
    m_simulation->set_show_selection_road(m_action_show_selection_road->isChecked());
}

void QAgentSimulationWidget::on_action_animation_play()
{
    m_simulation->animation_play(m_timer);
}

void QAgentSimulationWidget::on_action_animation_next()
{
    m_simulation->animation_next(m_timer);
}

void QAgentSimulationWidget::on_action_animation_previous()
{
    m_simulation->animation_previous(m_timer);
}

void QAgentSimulationWidget::on_action_run_simulation()
{
    m_action_show_agent->setChecked(true);
    on_action_show_agent();
    m_simulation->run_simulation(m_action_run_simulation->isChecked());
}

void QAgentSimulationWidget::on_timer_timeout()
{
    if (m_simulation->animation_timeout(m_timer))
    {
        m_action_animation_play->setChecked(false);
        m_simulation->animation_play(m_timer);
    }
}

void QAgentSimulationWidget::on_action_creat_road()
{
    if (m_action_creat_road->isChecked())
    {
        m_simulation->create_road();
        m_action_show_main_road->setChecked(true);
        m_simulation->set_show_main_road(true);
        m_action_creat_road->setText(tr("��ɴ���"));
    }
    else
    {
        m_simulation->create_road_finish();
        m_action_creat_road->setText(tr("������·"));
    }
}

void QAgentSimulationWidget::on_action_run_navigation()
{
    m_simulation->run_navigation(m_action_run_navigation->isChecked());
}

void QAgentSimulationWidget::on_action_run_agent_path()
{
    m_simulation->run_agent_path(m_action_run_agent_path->isChecked());
}

void QAgentSimulationWidget::on_action_run_agent_energy()
{
    m_simulation->run_agent_energy(m_action_run_agent_energy->isChecked());
}

void QAgentSimulationWidget::on_action_run_step()
{
    m_simulation->run_step(m_action_run_step->isChecked());
}

