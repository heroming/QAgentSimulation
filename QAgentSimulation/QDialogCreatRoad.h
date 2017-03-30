#pragma once

#include <QDialog>

class QLabel;
class QLineEdit;
class QSpinBox;
class QPushButton;

class QDialogCreatRoad : public QDialog
{
    Q_OBJECT

public :
    QDialogCreatRoad(QWidget * parent = 0);
    ~QDialogCreatRoad();

    std::string get_init_path() const;
    std::string get_save_path() const;
    int get_radius() const;
    int get_length() const;

private :
    QLabel * m_label_radius;
    QLabel * m_label_length;
    QLabel * m_label_path_init;
    QLabel * m_label_path_save;

    QSpinBox * m_sb_radius;
    QSpinBox * m_sb_length;;

    QLineEdit * m_le_path_init;
    QLineEdit * m_le_path_save;

    QPushButton * m_pb_select_init;
    QPushButton * m_pb_select_save;

    QPushButton * m_pb_ok;
    QPushButton * m_pb_cancel;

private :
    void init_connect();

private slots :
    void on_ok_clicked();
    void on_cancel_clicked();
    void on_select_init_clicked();
    void on_select_save_clicked();

};