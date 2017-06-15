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
    m_action_show_agent = new QAction(QIcon("Resources/agent.png"), tr("显示逃生人员"), this);
    m_action_show_agent->setStatusTip(tr("显示逃生人员"));
    m_action_show_agent->setToolTip(tr("显示逃生人员"));
    m_action_show_agent->setCheckable(true);
    m_action_show_agent->setChecked(false);

    m_action_show_road = new QAction(QIcon("Resources/road.png"), tr("显示道路"), this);
    m_action_show_road->setStatusTip(tr("显示道路"));
    m_action_show_road->setToolTip(tr("显示道路"));
    m_action_show_road->setCheckable(true);
    m_action_show_road->setChecked(false);

    m_action_show_main_road = new QAction(QIcon("Resources/main_road.png"), tr("显示主要道路"), this);
    m_action_show_main_road->setStatusTip(tr("显示主要道路"));
    m_action_show_main_road->setToolTip(tr("显示主要道路"));
    m_action_show_main_road->setCheckable(true);
    m_action_show_main_road->setChecked(false);

    m_action_show_selection_road = new QAction(QIcon("Resources/selection.png"), tr("选择道路"), this);
    m_action_show_selection_road->setStatusTip(tr("选择道路"));
    m_action_show_selection_road->setToolTip(tr("选择道路"));
    m_action_show_selection_road->setCheckable(true);
    m_action_show_selection_road->setChecked(false);

    m_action_animation_play = new QAction(QIcon("Resources/play.png"), tr("播放动画"), this);
    m_action_animation_play->setStatusTip(tr("播放动画"));
    m_action_animation_play->setToolTip(tr("播放动画"));
    m_action_animation_play->setCheckable(true);

    m_action_animation_next = new QAction(QIcon("Resources/next.png"), tr("下一帧"), this);
    m_action_animation_next->setStatusTip(tr("下一帧"));
    m_action_animation_next->setToolTip(tr("下一帧"));

    m_action_animation_previous = new QAction(QIcon("Resources/previous.png"), tr("上一帧"), this);
    m_action_animation_previous->setStatusTip(tr("上一帧"));
    m_action_animation_previous->setToolTip(tr("上一帧"));

    m_action_run_simulation = new QAction(QIcon("Resources/run.png"), tr("开始逃生规划"), this);
    m_action_run_simulation->setStatusTip(tr("开始逃生规划"));
    m_action_run_simulation->setToolTip(tr("开始逃生规划"));
    m_action_run_simulation->setCheckable(true);
    m_action_run_simulation->setChecked(false);

    m_action_run_navigation = new QAction(QIcon("Resources/navigation.png"), tr("查看某点理想逃生路线"), this);
    m_action_run_navigation->setStatusTip(tr("查看某点理想逃生路线"));
    m_action_run_navigation->setToolTip(tr("查看某点理想逃生路线"));
    m_action_run_navigation->setCheckable(true);
    m_action_run_navigation->setChecked(false);

    m_action_run_agent_path = new QAction(QIcon("Resources/agent_path.png"), tr("查看逃生者实际逃生路线"), this);
    m_action_run_agent_path->setStatusTip(tr("查看逃生者实际逃生路线"));
    m_action_run_agent_path->setToolTip(tr("查看逃生者实际逃生路线"));
    m_action_run_agent_path->setCheckable(true);
    m_action_run_agent_path->setChecked(false);

    m_action_run_agent_energy = new QAction(QIcon("Resources/agent_energy.png"), tr("查看逃生者虚拟势能场大小"), this);
    m_action_run_agent_energy->setStatusTip(tr("查看逃生者虚拟势能场大小"));
    m_action_run_agent_energy->setToolTip(tr("查看逃生者虚拟势能场大小"));
    m_action_run_agent_energy->setCheckable(true);
    m_action_run_agent_energy->setChecked(false);

    m_action_run_step = new QAction(QIcon("Resources/step_next.png"), tr("下一时间步"), this);
    m_action_run_step->setStatusTip(tr("下一时间步"));
    m_action_run_step->setToolTip(tr("下一时间步"));
    m_action_run_step->setCheckable(true);
    m_action_run_step->setChecked(false);

    m_action_creat_road = new QAction(QIcon("Resources/creat_road.png"), tr("创建道路"), this);
    m_action_creat_road->setStatusTip(tr("创建道路"));
    m_action_creat_road->setToolTip(tr("创建道路"));
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
    m_menu_file = m_menubar->addMenu(tr("文件(&F)"));
    m_menu_init = m_menubar->addMenu(tr("初始化(&I)"));
    m_menu_init->addAction(m_action_creat_road);
}

void QAgentSimulationWidget::init_tool_bars()
{
    m_toolbar_view_option = this->addToolBar(tr("显示选项"));
    m_toolbar_view_option->setObjectName("显示选项");
    m_toolbar_view_option->addAction(m_action_show_agent);
    m_toolbar_view_option->addAction(m_action_show_road);
    m_toolbar_view_option->addAction(m_action_show_main_road);
    m_toolbar_view_option->addAction(m_action_show_selection_road);

    m_toolbar_animation_option = this->addToolBar(tr("动画选项"));
    m_toolbar_animation_option->setObjectName("动画选项");
    m_toolbar_animation_option->addAction(m_action_animation_previous);
    m_toolbar_animation_option->addAction(m_action_animation_play);
    m_toolbar_animation_option->addAction(m_action_animation_next);

    m_toolbar_animation_option = this->addToolBar(tr("逃生模拟"));
    m_toolbar_animation_option->setObjectName("逃生模拟");
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
        m_action_creat_road->setText(tr("完成创建"));
    }
    else
    {
        m_simulation->create_road_finish();
        m_action_creat_road->setText(tr("创建道路"));
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

