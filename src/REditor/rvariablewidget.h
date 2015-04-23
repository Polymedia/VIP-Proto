#ifndef RVARIABLEWIDGET_H
#define RVARIABLEWIDGET_H

#include <QWidget>
#include <QTableView>

#include <RInside/rmodel.h>
#include <RInside/rconsole.h>

namespace Ui {
class RVariableWidget;
}

class RVariableWidget : public QWidget
{
    Q_OBJECT

public:
    RVariableWidget(RConsole *console, const QString &name, QWidget *parent = 0);
    ~RVariableWidget();

public slots:
    void updateVar();
    void updateSize();

private:
    Ui::RVariableWidget *ui;
    RModel *m_model;
    QTableView *m_view;
    RConsole *m_console;
    QString m_name;

};

#endif // RVARIABLEWIDGET_H
