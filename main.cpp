#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle(QLatin1String("QMultiSpinBox testing tool"));
    w.show();

    return a.exec();
}
