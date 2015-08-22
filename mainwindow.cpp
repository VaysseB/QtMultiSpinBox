#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QtMultiSpinBoxElements"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->comboHTextAlign->addItem(QLatin1String("Qt::AlignLeft"),     (int)Qt::AlignLeft);
    ui->comboHTextAlign->addItem(QLatin1String("Qt::AlignLeading"),  (int)Qt::AlignLeading);
    ui->comboHTextAlign->addItem(QLatin1String("Qt::AlignRight"),    (int)Qt::AlignRight);
    ui->comboHTextAlign->addItem(QLatin1String("Qt::AlignTrailing"), (int)Qt::AlignTrailing);
    ui->comboHTextAlign->addItem(QLatin1String("Qt::AlignHCenter"),  (int)Qt::AlignHCenter);
    ui->comboHTextAlign->setCurrentIndex(ui->comboHTextAlign->count()-1);

    ui->comboVTextAlign->addItem(QLatin1String("Qt::AlignTop"),      (int)Qt::AlignTop);
    ui->comboVTextAlign->addItem(QLatin1String("Qt::AlignBottom"),   (int)Qt::AlignBottom);
    ui->comboVTextAlign->addItem(QLatin1String("Qt::AlignBaseline"), (int)Qt::AlignBaseline);
    ui->comboVTextAlign->addItem(QLatin1String("Qt::AlignVCenter"),  (int)Qt::AlignVCenter);
    ui->comboVTextAlign->setCurrentIndex(ui->comboVTextAlign->count()-1);

    //
    buildRow1();
    buildRow2();
    buildRow3_4();

    firstUpdateForAll();
    updateAllWithOptions();

    connect(ui->checkEnable, SIGNAL(clicked()),
            this, SLOT(updateAllWithOptions()));
    connect(ui->spinCurrentSection, SIGNAL(valueChanged(int)),
            this, SLOT(updateAllWithOptions()));
    connect(ui->comboHTextAlign, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateAllWithOptions()));
    connect(ui->comboVTextAlign, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateAllWithOptions()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

QList<QtMultiSpinBox *> MainWindow::allMultiSpinBox() const
{
    return findChildren<QtMultiSpinBox*>(QRegExp(".*"));
}

void MainWindow::firstUpdateForAll()
{
    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Maximum);
    foreach(QtMultiSpinBox* m, allMultiSpinBox()) {
        m->setSizePolicy(sp);
    }
}

void MainWindow::updateAllWithOptions() const
{
    foreach(QtMultiSpinBox* m, allMultiSpinBox()) {
        m->setEnabled(ui->checkEnable->isChecked());
        m->setAlignment((Qt::Alignment)ui->comboHTextAlign->currentData().toInt()
                        | (Qt::Alignment)ui->comboVTextAlign->currentData().toInt());
        m->setCurrentSectionIndex(ui->spinCurrentSection->value());
    }
}

void MainWindow::buildRow1() const
{
    ui->qmspbR1C2->setPrefix(QLatin1String("["));

    ui->qmspbR1C3->setEnabled(false);
    ui->qmspbR1C4->setEnabled(false);
}

void MainWindow::buildRow2() const
{
    ui->qmspbR2C1->appendSpinElement(new QtIntMultiSpinBoxElement);

    ui->qmspbR2C2->setPrefix("[");
    ui->qmspbR2C2->appendSpinElement(new QtIntMultiSpinBoxElement);

    ui->qmspbR2C3->appendSpinElement(new QtIntMultiSpinBoxElement);
    ui->qmspbR2C3->setPrefix("[");
    ui->qmspbR2C3->setSuffix(0, QLatin1String("]"));

    ui->qmspbR2C4->appendSpinElement(new QtIntMultiSpinBoxElement, QLatin1String("]"));
}

void MainWindow::buildRow3_4() const
{
    ui->qmspbR3C1->appendSpinElement(new QtIntMultiSpinBoxElement, " ");
    ui->qmspbR3C1->appendSpinElement(new QtIntMultiSpinBoxElement);

    ui->qmspbR3C2->setPrefix(QLatin1String("["));
    ui->qmspbR3C2->appendSpinElement(new QtIntMultiSpinBoxElement, " ");
    ui->qmspbR3C2->appendSpinElement(new QtIntMultiSpinBoxElement);

    ui->qmspbR3C3->setPrefix(QLatin1String("["));
    ui->qmspbR3C3->appendSpinElement(new QtIntMultiSpinBoxElement, QLatin1String("] "));
    ui->qmspbR3C3->appendSpinElement(new QtIntMultiSpinBoxElement);

    ui->qmspbR3C4->appendSpinElement(new QtIntMultiSpinBoxElement, QLatin1String("] "));
    ui->qmspbR3C4->appendSpinElement(new QtIntMultiSpinBoxElement);

    ui->qmspbR4C1->setPrefix(QLatin1String("["));
    ui->qmspbR4C1->appendSpinElement(new QtIntMultiSpinBoxElement, QLatin1String("] "));
    ui->qmspbR4C1->appendSpinElement(new QtIntMultiSpinBoxElement);

    ui->qmspbR4C2->setPrefix(QLatin1String("["));
    ui->qmspbR4C2->appendSpinElement(new QtIntMultiSpinBoxElement, QLatin1String("] ["));
    ui->qmspbR4C2->appendSpinElement(new QtIntMultiSpinBoxElement);

    ui->qmspbR4C3->appendSpinElement(new QtIntMultiSpinBoxElement, " ");
    ui->qmspbR4C3->appendSpinElement(new QtIntMultiSpinBoxElement);
    ui->qmspbR4C3->setPrefix(QLatin1String("["));
    ui->qmspbR4C3->setSuffix(0, QLatin1String("] ["));
    ui->qmspbR4C3->setSuffix(1, QLatin1String("]"));

    ui->qmspbR4C4->setPrefix(QLatin1String("["));
    ui->qmspbR4C4->appendSpinElement(new QtIntMultiSpinBoxElement, QLatin1String("] "));
    ui->qmspbR4C4->appendSpinElement(new QtIntMultiSpinBoxElement, QLatin1String("]"));

}

void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}
