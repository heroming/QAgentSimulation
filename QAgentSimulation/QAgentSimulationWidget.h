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
    void on_action_show_selection_road();

    void on_action_animation_play();
    void on_action_animation_next();
    void on_action_animation_previous();

    void on_timer_timeout();

    void on_action_creat_road();

protected:
    void init_actions();
    void init_message_maps();
    void init_menus();
    void init_tool_bars();

private:

    // ----------------------- 工具栏 ---------------------
    QToolBar * m_toolbar_view_option;
    QToolBar * m_toolbar_animation_option;

    // ----------------------- 菜单栏 ---------------------
    QMenuBar * m_menubar;

    QMenu * m_menu_file;                    // 文件
    QMenu * m_menu_init;                    // 初始化


    // ----------------------- Action --------------------
    QAction * m_action_show_agent;
    QAction * m_action_show_road;
    QAction * m_action_show_main_road;
    QAction * m_action_show_selection_road;

    QAction * m_action_animation_play;
    QAction * m_action_animation_next;
    QAction * m_action_animation_previous;

    QAction * m_action_creat_road;


    // ----------------------- Timer --------------------
    QTimer * m_timer;

private:
    QAgentSimulation * m_simulation;

};