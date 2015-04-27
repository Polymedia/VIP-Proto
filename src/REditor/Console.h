#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>

class Console : public QPlainTextEdit
{
    Q_OBJECT
public:
    Console(QWidget *parent = 0);
    void execute(const QString &text, bool all);
    void output(const QString &s);
    void extraInput();
    void scrollDown();

protected:
    void keyPressEvent(QKeyEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

public slots:
    void clear();

private slots:
    void onCursorPositionChanged();
    void onEnter();

private:
    void insertPrompt(bool insertNewBlock = true);
    void historyAdd(const QString &cmd);
    void historyBack();
    void historyForward();
    void findLastBlock(int &first, int &last);
    bool isCursorInLastBlock(int curPos = -1);
    void setEditing(bool allow);
    void clearCurrentBlock();

private:
    static const QString m_defaultPrompt;
    static const QString m_ExtraPrompt;
    QString m_currentPromt;
    QStringList *m_history;
    static const int m_historyCount;
    int m_historyPos;

    bool m_isExtra;

signals:
    void command(QString);
    void commandStringChanged(QString);
    void enterPressed();
};

#endif // CONSOLE_H
