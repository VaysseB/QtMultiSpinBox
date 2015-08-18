#include <QApplication>

#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "qmultispinbox.h"
#include "qmultispinboxelements.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget root;
    root.setLayout(new QVBoxLayout);

    QPushButton* buttonQuit = new QPushButton(QLatin1String("Close"), &root);
    QLabel* titleLabel = new QLabel(QLatin1String("QMultiSpinBox"), &root);
    QMultiSpinBox* msp = new QMultiSpinBox(&root);

    msp->insertSpinElement(0, new QMultiSpinBoxBinaryElement);
    msp->insertSpinElement(1, new QMultiSpinBoxBinaryElement);
    msp->setTextAlignement(Qt::AlignCenter);

    root.layout()->addWidget(titleLabel);
    root.layout()->addWidget(msp);
    root.layout()->addWidget(buttonQuit);

    a.connect(buttonQuit, SIGNAL(clicked()), SLOT(quit()));

    root.show();

    return a.exec();
}
