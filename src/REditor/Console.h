#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>

class Console : public QPlainTextEdit
{
    Q_OBJECT
public:
    Console(QWidget *parent = 0);
    void output(const QString &s);
    void scrollDown();
protected:
    void keyPressEvent(QKeyEvent *);
    void contextMenuEvent(QContextMenuEvent *);

private:
    QString prompt;
    bool isLocked;
    QStringList *history;
    int historyPos;

    void onEnter();
    void insertPrompt(bool insertNewBlock = true);
    void historyAdd(const QString &cmd);
    void historyBack();
    void historyForward();    
    void findLastBlock(int &first, int &last);
    bool isCursorInLastBlock(int curPos = -1);

signals:
    void onCommand(QString);
    void onChange(QString);
};

#endif // CONSOLE_H
