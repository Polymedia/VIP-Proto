#include "console.h"

#include <QTextBlock>
#include <QScrollBar>

#include <QDebug>
#include <QMenu>

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    prompt = "> ";

    history = new QStringList;
    historyPos = 0;
    insertPrompt(false);
    isLocked = false;
}

void Console::keyPressEvent(QKeyEvent *event)
{
    // Если консоль заблочена, или курсор находится не в последнем блоке, то игнорим нажатия клавы
    if (isLocked || !isCursorInLastBlock())
        return;

    if (event->modifiers() == Qt::NoModifier) {
        switch (event->key()) {
        case Qt::Key_Return: onEnter(); break;
        case Qt::Key_Up: historyBack(); break;
        case Qt::Key_Down: historyForward(); break;

        case Qt::Key_Left: {
            if (textCursor().positionInBlock() > prompt.length())
                QPlainTextEdit::keyPressEvent(event);
            break;
        }
        case Qt::Key_Right: {
                QPlainTextEdit::keyPressEvent(event);
            break;
        }

        case Qt::Key_Backspace:
        case Qt::Key_Delete: {
            if (textCursor().positionInBlock() > prompt.length())
                QPlainTextEdit::keyPressEvent(event);
            break;
        }
        }
    }

    // Обработчик нажатий видимых символов
    if (event->key() >= 0x20 && event->key() <= 0x7e
            && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier))
        QPlainTextEdit::keyPressEvent(event);

    QString cmd = textCursor().block().text().mid(prompt.length());
    emit onChange(cmd);
}

void Console::contextMenuEvent(QContextMenuEvent *ev)
{
    // Убираем контекстное меню
    //QPlainTextEdit::contextMenuEvent(ev);
}

void Console::onEnter()
{
    QTextCursor cursor = textCursor();

    // Ввод пустой строчки
    if (cursor.positionInBlock() == prompt.length()) {
        insertPrompt();
        return;
    }

    // Если курсор при нажатии не в конце строчки, то перемещаем в конец
    if (cursor.positionInBlock() != cursor.block().length()) {
        cursor.setPosition(cursor.block().position() + cursor.block().length() - 1);
        setTextCursor(cursor);
    }

    int lastPromtPos = toPlainText().lastIndexOf(prompt);

    QString cmd = toPlainText().mid(lastPromtPos + prompt.length());
    isLocked = true;
    historyAdd(cmd);

    // Если вставили несколько строк с переносом строки, то делим на команды и выполняем по очереди
    QStringList cmdList = cmd.split("\n");
    //qDebug() << cmdList << lastPromtPos;
    foreach (auto tmpCmd, cmdList) {
        emit onCommand(tmpCmd);
    }
}

void Console::output(const QString &s)
{
    textCursor().insertBlock();
    //    QTextCharFormat format;
    //    format.setForeground(error ? Qt::red : Qt::black);
    //    textCursor().setBlockCharFormat(format);
    textCursor().insertText(s);
    insertPrompt();
    isLocked = false;
}

void Console::insertPrompt(bool insertNewBlock)
{
    if (insertNewBlock)
        textCursor().insertBlock();
    //    QTextCharFormat format;
    //    format.setForeground(Qt::green);
    //    textCursor().setBlockCharFormat(format);
    textCursor().insertText(prompt);
    scrollDown();
}

void Console::scrollDown()
{
    QScrollBar *vbar = verticalScrollBar();
    vbar->setValue(vbar->maximum());
}

void Console::historyAdd(const QString &cmd)
{
    history->append(cmd);
    historyPos = history->length();
}

void Console::historyBack()
{
    if (!historyPos)
        return;

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText(prompt + history->at(historyPos - 1));
    setTextCursor(cursor);
    historyPos--;
}

void Console::historyForward()
{
    if (historyPos == history->length())
        return;

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    if (historyPos == history->length() - 1)
        cursor.insertText(prompt);
    else
        cursor.insertText(prompt + history->at(historyPos + 1));

    setTextCursor(cursor);
    historyPos++;
}


void Console::findLastBlock(int &first, int &last)
{
    QTextCursor cur = textCursor();
    cur.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

    first = cur.block().position() + prompt.length();
    last = cur.position();
}

bool Console::isCursorInLastBlock(int curPos)
{
    int firstPos, lastPos, pos;

    findLastBlock(firstPos, lastPos);

    if (curPos < 0) {
        QTextCursor cur = textCursor();
        pos = cur.position();
    } else {
        pos = curPos;
    }

    if (firstPos <= pos && pos <= lastPos && textCursor().selectionStart() >= firstPos)
        return true;
    else
        return false;
}
