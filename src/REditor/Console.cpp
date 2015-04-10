#include "console.h"

#include <QTextBlock>
#include <QScrollBar>

#include <QMenu>

QString const Console::m_prompt = "> ";
int const Console::m_historyCount = 500;

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    m_history = new QStringList;
    m_historyPos = 0;
    insertPrompt(false);
    m_isLocked = false;

    connect(this, SIGNAL(cursorPositionChanged()), SLOT(onCursorPositionChanged()));
}

void Console::keyPressEvent(QKeyEvent *event)
{
    // Если консоль заблочена, или курсор находится не в последнем блоке, то игнорим нажатия клавы
    if (m_isLocked || !isCursorInLastBlock())
        return;

    if (event->modifiers() == Qt::NoModifier) {
        switch (event->key()) {
        case Qt::Key_Return: onEnter(); break;
        case Qt::Key_Up: historyBack(); break;
        case Qt::Key_Down: historyForward(); break;

        case Qt::Key_Left: {
            if (textCursor().positionInBlock() > m_prompt.length())
                QPlainTextEdit::keyPressEvent(event);
            break;
        }
        case Qt::Key_Right: {
                QPlainTextEdit::keyPressEvent(event);
            break;
        }

        case Qt::Key_Backspace: {
            if (textCursor().positionInBlock() > m_prompt.length())
                QPlainTextEdit::keyPressEvent(event);
            break;
        }
        case Qt::Key_Delete: {
            if (textCursor().positionInBlock() >= m_prompt.length())
                QPlainTextEdit::keyPressEvent(event);
            break;
        }
        }
    }

    // Обработчик нажатий видимых символов
    if (event->key() >= 0x20 && event->key() <= 0x7e
            && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier))
        QPlainTextEdit::keyPressEvent(event);

    QString cmd = textCursor().block().text().mid(m_prompt.length());
    emit commandStringChanged(cmd);
}

void Console::contextMenuEvent(QContextMenuEvent *event)
{
    // Убираем контекстное меню
    //QPlainTextEdit::contextMenuEvent(event);
}

void Console::onCursorPositionChanged()
{
    // Если курсор не в последнем блоке, то отключаем редактирование
    setEditing(isCursorInLastBlock());
}

void Console::onEnter()
{
    QTextCursor cursor = textCursor();

    // Ввод пустой строчки
    if (cursor.positionInBlock() == m_prompt.length()) {
        insertPrompt();
        return;
    }

    // Если курсор при нажатии Enter не в конце строчки, то перемещаем в конец
    if (cursor.positionInBlock() != cursor.block().length()) {
        cursor.setPosition(cursor.block().position() + cursor.block().length() - 1);
        setTextCursor(cursor);
    }

    int lastPromtPos = toPlainText().lastIndexOf(m_prompt);

    QString cmd = toPlainText().mid(lastPromtPos + m_prompt.length());
    m_isLocked = true;
    historyAdd(cmd);

    // Если вставили несколько строк с переносом строки, то делим на команды и выполняем по очереди
    QStringList cmdList = cmd.split("\n");
    foreach (auto tmpCmd, cmdList) {
        emit command(tmpCmd);
    }
}

void Console::output(const QString &s)
{
    textCursor().insertBlock();
    textCursor().insertText(s);
    insertPrompt();
    m_isLocked = false;
}

void Console::insertPrompt(bool insertNewBlock)
{
    if (insertNewBlock)
        textCursor().insertBlock();
    textCursor().insertText(m_prompt);
    scrollDown();
}

void Console::scrollDown()
{
    QScrollBar *vbar = verticalScrollBar();
    vbar->setValue(vbar->maximum());
}

void Console::historyAdd(const QString &cmd)
{
    m_history->append(cmd);

    if (m_history->size() > m_historyCount)
        m_history->removeFirst();

    m_historyPos = m_history->length();
}

void Console::historyBack()
{
    if (!m_historyPos)
        return;

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
    cursor.insertText(m_prompt + m_history->at(m_historyPos - 1));
    setTextCursor(cursor);
    m_historyPos--;
}

void Console::historyForward()
{
    if (m_historyPos == m_history->length())
        return;

    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();

    if (m_historyPos == m_history->length() - 1)
        cursor.insertText(m_prompt);
    else
        cursor.insertText(m_prompt + m_history->at(m_historyPos + 1));

    setTextCursor(cursor);
    m_historyPos++;
}


void Console::findLastBlock(int &first, int &last)
{
    QTextCursor cur = textCursor();
    cur.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);

    first = cur.block().position() + m_prompt.length();
    last = cur.position();
}

bool Console::isCursorInLastBlock(int curPos)
{
    int firstPos, lastPos, pos;

    findLastBlock(firstPos, lastPos);

    if (curPos < 0) {
        QTextCursor cur = textCursor();
        pos = cur.position();
    } else
        pos = curPos;

    if (firstPos <= pos && pos <= lastPos && textCursor().selectionStart() >= firstPos)
        return true;
    else
        return false;
}

void Console::setEditing(bool allow)
{
    Qt::TextInteractionFlags flags = textInteractionFlags();

    if (allow)
        flags |= Qt::TextEditable;
    else
        flags &= ~Qt::TextEditable;

    setTextInteractionFlags(flags);
}
