#include <QtGui>

#include "QAgentSimulationWidget.h"
#include "QAgentSimulation.h"

QAgentSimulationWidget::QAgentSimulationWidget(QWidget * parent, Qt::WFlags flags)
    : QMainWindow(parent, flags)
{
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
    m_action_show_road->setChecked(true);

    m_action_show_main_road = new QAction(QIcon("Resources/main_road.png"), tr("显示主要道路"), this);
    m_action_show_main_road->setStatusTip(tr("显示主要道路"));
    m_action_show_main_road->setToolTip(tr("显示主要道路"));
    m_action_show_main_road->setCheckable(true);
    m_action_show_main_road->setChecked(false);
}

void QAgentSimulationWidget::init_message_maps()
{
    connect(m_action_show_agent, SIGNAL(triggered()), this, SLOT(on_action_show_agent()));
    connect(m_action_show_road, SIGNAL(triggered()), this, SLOT(on_action_show_road()));
    connect(m_action_show_main_road, SIGNAL(triggered()), this, SLOT(on_action_show_main_road()));
}

void QAgentSimulationWidget::init_menus()
{
    m_menubar = menuBar();
    m_menu_file = m_menubar->addMenu(tr("文件(&F)"));
}

void QAgentSimulationWidget::init_tool_bars()
{
    m_toolbar_view_option = this->addToolBar(tr("显示选项"));
    m_toolbar_view_option->setObjectName("显示选项");
    m_toolbar_view_option->addAction(m_action_show_agent);
    m_toolbar_view_option->addAction(m_action_show_road);
    m_toolbar_view_option->addAction(m_action_show_main_road);
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
