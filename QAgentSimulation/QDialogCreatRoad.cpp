#include <QtGui>

#include "QDialogCreatRoad.h"

QDialogCreatRoad::QDialogCreatRoad(QWidget * parent): QDialog(parent)
{
    this->resize(533, 327);
    m_label_radius = new QLabel(this);
    m_label_radius->setText(tr("邻域半径"));
    m_label_radius->setGeometry(QRect(30, 30, 71, 31));
    m_label_length = new QLabel(this);
    m_label_length->setText(tr("路径长度"));
    m_label_length->setGeometry(QRect(30, 90, 71, 31));
    m_label_path_init = new QLabel(this);
    m_label_path_init->setText(tr("初始路径"));
    m_label_path_init->setGeometry(QRect(30, 150, 71, 31));
    m_label_path_save = new QLabel(this);
    m_label_path_save->setText(tr("保存路径"));
    m_label_path_save->setGeometry(QRect(30, 200, 71, 31));
    m_sb_radius = new QSpinBox(this);
    m_sb_radius->setGeometry(QRect(100, 30, 64, 24));
    m_sb_radius->setMaximum(32);
    m_sb_radius->setValue(8);
    m_sb_length = new QSpinBox(this);
    m_sb_length->setGeometry(QRect(100, 90, 64, 24));
    m_sb_length->setMinimum(5);
    m_sb_length->setMaximum(128);
    m_sb_length->setValue(50);
    m_le_path_init = new QLineEdit(this);
    m_le_path_init->setGeometry(QRect(100, 150, 300, 24));
    m_le_path_save = new QLineEdit(this);
    m_le_path_save->setGeometry(QRect(100, 200, 300, 24));
    m_le_path_save->setText(tr("E:/Projects/QAgentSimulation/QAgentSimulation/Data/road/10086"));

    m_pb_select_init = new QPushButton(this);
    m_pb_select_init->setText(tr("选择"));
    m_pb_select_save = new QPushButton(this);
    m_pb_select_save->setText(tr("选择"));

    m_pb_select_init->setGeometry(QRect(420, 150, 75, 25));
    m_pb_select_save->setGeometry(QRect(420, 200, 75, 25));

    m_pb_ok = new QPushButton(this);
    m_pb_ok->setText(tr("确定"));
    m_pb_cancel = new QPushButton(this);
    m_pb_cancel->setText(tr("取消"));

    m_pb_ok->setGeometry(QRect(250, 270, 90, 30));
    m_pb_cancel->setGeometry(QRect(390, 270, 90, 30));

    init_connect();
}

QDialogCreatRoad::~QDialogCreatRoad() {}

std::string QDialogCreatRoad::get_init_path() const
{
    std::string l_path(m_le_path_init->text().toLocal8Bit());
    return l_path;
}

std::string QDialogCreatRoad::get_save_path() const
{
    std::string l_path(m_le_path_save->text().toLocal8Bit());
    return l_path;
}

int QDialogCreatRoad::get_radius() const
{
    return m_sb_radius->value();
}

int QDialogCreatRoad::get_length() const
{
    return m_sb_length->value();
}

void QDialogCreatRoad::init_connect()
{
    connect(m_pb_ok, SIGNAL(clicked()), this, SLOT(on_ok_clicked()));
    connect(m_pb_cancel, SIGNAL(clicked()), this, SLOT(on_cancel_clicked()));
    connect(m_pb_select_init, SIGNAL(clicked()), this, SLOT(on_select_init_clicked()));
    connect(m_pb_select_save, SIGNAL(clicked()), this, SLOT(on_select_save_clicked()));
}

void QDialogCreatRoad::on_ok_clicked()
{
    if (m_le_path_save->text().isEmpty())
    {
        QMessageBox::warning(0, tr("警告"), tr("请选择保存路径！"));
        return;
    }
    accept();
}

void QDialogCreatRoad::on_cancel_clicked()
{
    reject();
}

void QDialogCreatRoad::on_select_init_clicked()
{
    QString l_path = QFileDialog::getOpenFileName(this, tr("打开初始路径"), ".", tr("Binary File(*.dat)"));
    if (l_path.size() != 0)
    {
        m_le_path_init->setText(l_path);
    }
}

void QDialogCreatRoad::on_select_save_clicked()
{
    QString l_path = QFileDialog::getOpenFileName(this, tr("打开保存路径"), ".", tr("All File(*.*)"));
    if (l_path.size() != 0)
    {
        m_le_path_save->setText(l_path);
    }
}