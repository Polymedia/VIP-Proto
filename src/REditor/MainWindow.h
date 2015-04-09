#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class RConsole;
class Console;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(RConsole *r, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onExecuteClicked(const QString &command = "");

    void onRMessageOk(const QString &message);
    void onRMessageError(const QString &message);

    void updatePlot();
    void printOutputBuf();

private:
    QString m_plotFilePath;
    RConsole *m_rconsole;

    Console *m_guiConsole;

    QString m_outputBuf;
    QString m_lastOutput;
    QTimer m_outputTimer;


    Ui::MainWindow *ui;    
};

#endif // MAINWINDOW_H
