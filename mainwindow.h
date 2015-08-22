#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QtMultiSpinBox;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QList<QtMultiSpinBox*> allMultiSpinBox() const;

public slots:
     void updateAllWithOptions() const;

private slots:
     void on_actionQuit_triggered();

private:
     void buildRow1() const;
     void buildRow2() const;
     void buildRow3_4() const;

     void firstUpdateForAll();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
