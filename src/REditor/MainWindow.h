#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class RConsole;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(RConsole *r, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onExecuteClicked();

    void onRMessage(const QString &message);

private:
    QString m_plotFilePath;
    RConsole *m_rconsole;

    Ui::MainWindow *ui;    
};

#endif // MAINWINDOW_H
