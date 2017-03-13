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

protected:
    void init_actions();
    void init_message_maps();
    void init_menus();
    void init_tool_bars();

private:

    // ----------------------- ¹¤¾ßÀ¸ ---------------------
    QToolBar * m_toolbar_view_option;
    QToolBar * m_toolbar_animation_option;

    // ----------------------- ²Ëµ¥À¸ ---------------------
    QMenuBar * m_menubar;

    QMenu * m_menu_file;

    // ----------------------- Action --------------------
    QAction * m_action_show_agent;
    QAction * m_action_show_road;
    QAction * m_action_show_main_road;
    QAction * m_action_show_selection_road;

    QAction * m_action_animation_play;
    QAction * m_action_animation_next;
    QAction * m_action_animation_previous;


    // ----------------------- Timer --------------------
    QTimer * m_timer;

private:
    QAgentSimulation * m_simulation;

};