#include <QApplication>
#include <QVBoxLayout>

#include "mainwindow.h"

#include <QtMultiSpinBox>
#include <QtMultiSpinBoxElements>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setWindowTitle(QLatin1String("QMultiSpinBox testing tool"));
    w.show();

    //    QWidget root;
    //    QtMultiSpinBox* spin = new QtMultiSpinBox(&root);
    //    spin->setPrefix("X=");
    //    spin->appendSpinElement(new QtIntMultiSpinBoxElement /* the x */, QLatin1String(" Y="));
    //    spin->appendSpinElement(new QtIntMultiSpinBoxElement /* the y */, QLatin1String(" Z="));
    //    spin->appendSpinElement(new QtIntMultiSpinBoxElement /* the z */);

    //    root.setLayout(new QVBoxLayout);
    //    root.layout()->addWidget(spin);
    //    root.show();

    return a.exec();
}
