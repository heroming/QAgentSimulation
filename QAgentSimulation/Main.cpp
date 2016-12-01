#include "QAgentSimulationWidget.h"
#include "QAgentSimulation.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    QApplication a(argc, argv);
    QAgentSimulationWidget w;
    int l_x = 200, l_y = 128, l_w = 1280, l_h = 800;
    w.setGeometry(l_x, l_y, l_x + l_w, l_y + l_h); 
    w.show();
    return a.exec();
}
