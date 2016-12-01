#pragma once

#include <QtGui>

class QAgentSimulation;
class QAgentSimulationWidget : public QMainWindow
{
    Q_OBJECT

public:
    QAgentSimulationWidget(QWidget * parent = 0, Qt::WFlags flags = 0);
    ~QAgentSimulationWidget();

public slots:
    void on_action_show_agent();
    void on_action_show_road();
    void on_action_show_main_road();

protected:
    void init_actions();
    void init_message_maps();
    void init_menus();
    void init_tool_bars();

private:

    // ----------------------- ¹¤¾ßÀ¸ ---------------------
    QToolBar * m_toolbar_view_option;

    // ----------------------- ²Ëµ¥À¸ ---------------------
    QMenuBar * m_menubar;

    QMenu * m_menu_file;

    // ----------------------- Action --------------------
    QAction * m_action_show_agent;
    QAction * m_action_show_road;
    QAction * m_action_show_main_road;

private:
    QAgentSimulation * m_simulation;

};