#include "rvariablewidget.h"
#include "ui_rvariablewidget.h"

#define MAX_ITEM_HEIGHT 300

RVariableWidget::RVariableWidget(RConsole *console, const QString &name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RVariableWidget),
    m_console(console),
    m_name(name)
{
    ui->setupUi(this);

    m_model = new RModel((*m_console)[name]);
    m_view = new QTableView(this);

    m_view->setModel(m_model);
    m_view->update();

    ui->labelName->setText(name);

    ui->VLayout->addWidget(m_view);

    updateSize();
}

RVariableWidget::~RVariableWidget()
{
    delete m_model;
    delete m_view;
    delete ui;
}

// TODO: утечка памяти в R, надо вызывать сборщик мусора для данной переменной
void RVariableWidget::updateVar()
{
    delete m_model;

    m_model = new RModel((*m_console)[m_name]);
    m_view->setModel(m_model);

    updateSize();
}

//TODO: отдельно счить есть header или нет

void RVariableWidget::updateSize()
{
    int rowHeight = m_view->rowHeight(0) != 0 ? m_view->rowHeight(0) : 50; // если пустая таблица

    int modelHeight = (1 + m_model->rowCount()) * rowHeight + ui->labelName->height() + ui->VLayout->spacing() * 3;
    int newHeight = MAX_ITEM_HEIGHT < modelHeight ? MAX_ITEM_HEIGHT : modelHeight;

    setFixedHeight(newHeight);
}

