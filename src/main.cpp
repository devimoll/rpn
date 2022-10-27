#include "rpn.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    rpn w;
    w.show();
    return a.exec();
}
