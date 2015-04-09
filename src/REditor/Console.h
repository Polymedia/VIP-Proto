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
    void keyPressEvent(QKeyEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

private:
    void onEnter();
    void insertPrompt(bool insertNewBlock = true);
    void historyAdd(const QString &cmd);
    void historyBack();
    void historyForward();
    void findLastBlock(int &first, int &last);
    bool isCursorInLastBlock(int curPos = -1);

private:
    QString m_prompt;
    bool m_isLocked;
    QStringList *m_history;
    int m_historyPos;

signals:
    void command(QString);
    void commandStringChanged(QString);
};

#endif // CONSOLE_H
