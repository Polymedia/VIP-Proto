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
    void onWainExtaInput();

private:
    QString m_plotFilePath;
    RConsole *m_rconsole;

    Console *m_guiConsole;

    QString m_outputBuf;
    QString m_lastOutput;
    // Таймер, чтобы дождаться всего вывода из консоли, если он многострочный
    QTimer m_outputTimer;
    // Таймер, чтобы узнать, есть ли вывод от R или нет (некоторые команды ждут дополнительный ввод)
    QTimer m_waitOutputTimer;

    Ui::MainWindow *ui;    
};

#endif // MAINWINDOW_H
