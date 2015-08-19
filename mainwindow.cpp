#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    buildRow1();
    buildRow2();

    firstUpdateForAll();
    updateAllWithOptions();

    connect(ui->checkEnable, SIGNAL(clicked()),
            this, SLOT(updateAllWithOptions()));
    connect(ui->spinCurrentSection, SIGNAL(valueChanged(int)),
            this, SLOT(updateAllWithOptions()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

QList<QMultiSpinBox*> MainWindow::allMultiSpinBox() const
{
    return findChildren<QMultiSpinBox*>(QRegExp(".*"));
}

void MainWindow::firstUpdateForAll()
{
    QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Maximum);
    foreach(QMultiSpinBox* m, allMultiSpinBox()) {
        m->setSizePolicy(sp);
    }
}

void MainWindow::updateAllWithOptions() const
{
    foreach(QMultiSpinBox* m, allMultiSpinBox()) {
        m->setEnabled(ui->checkEnable->isChecked());
        m->setTextAlignement(Qt::AlignCenter);
        m->setCurrentSectionIndex(ui->spinCurrentSection->value());
    }
}

void MainWindow::buildRow1() const
{
    ui->qmspbR1C2->setPrefix("[");
    ui->qmspbR1C4->setPrefix("[");
}

void MainWindow::buildRow2() const
{
    ui->qmspbR2C2->setPrefix("[");
    ui->qmspbR2C4->setPrefix("[");

    ui->qmspbR2C1->insertSpinElement(0, new QMultiSpinBoxBinaryElement);
    ui->qmspbR2C2->insertSpinElement(0, new QMultiSpinBoxBinaryElement);
    ui->qmspbR2C3->insertSpinElement(0, new QMultiSpinBoxBinaryElement, QString("]"));
    ui->qmspbR2C4->insertSpinElement(0, new QMultiSpinBoxBinaryElement, QString("]"));
}

void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}
