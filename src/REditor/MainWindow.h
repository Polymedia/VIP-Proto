#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QtWidgets>

namespace Ui {
class MainWindow;
}

class RConsole;
class Console;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(RConsole &r, QWidget *parent = 0);
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initR();
    void clearR();
    void setEditorFile(const QString &fileName = "");

public slots:
    void clearEditor(bool clearAll);

private slots:
    void onCommand(const QString &command = "");

    void onRMessageOk(const QString &message);
    void onRMessageError(const QString &message);
    void onRParseIncomplete();

    void updatePlot();
    void printOutputBuf();

    void onNew();
    void onOpen();
    void onSave(bool saveAs = false);
    void onSaveAs();
    void onExecute();
    void onDockToggle(bool checked);

    void closeEvent(QCloseEvent *event);

private:
    QString m_fileName;
    QString m_plotFilePath;
    RConsole &m_rconsole;
    bool m_editorTextChanged;

    QString m_outputBuf;
    QString m_lastOutput;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
