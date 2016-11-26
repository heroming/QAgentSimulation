#include "QAgentSimulation.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QAgentSimulation w;
    int l_x = 200, l_y = 128, l_w = 1280, l_h = 800;
    w.setGeometry(l_x, l_y, l_x + l_w, l_y + l_h); 
    w.show();
    a.setActiveWindow(&w);
    return a.exec();
}
