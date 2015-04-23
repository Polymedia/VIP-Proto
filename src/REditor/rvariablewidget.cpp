#include "rvariablewidget.h"
#include "ui_rvariablewidget.h"

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
    int modelHeight = m_model->rowCount() * (1 + m_view->rowHeight(0)) + ui->labelName->height() + ui->VLayout->spacing() * 3;
    int maxItemHeight = 300;
    int newHeight = maxItemHeight < modelHeight ? maxItemHeight : modelHeight;

    setFixedHeight(newHeight);
}

