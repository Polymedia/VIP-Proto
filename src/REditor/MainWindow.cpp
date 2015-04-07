#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <rconsole.h>
#include <QFile>

MainWindow::MainWindow(RConsole *r, QWidget *parent) :
    QMainWindow(parent),
    m_rconsole(r),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->btnExecute, SIGNAL(clicked()), SLOT(onExecuteClicked()));
    connect(ui->leInput, SIGNAL(returnPressed()), SLOT(onExecuteClicked()));

    // Tempolary file for R plots
    m_plotFilePath = "1.png";
    QFile::remove(m_plotFilePath);
}

MainWindow::~MainWindow()
{
    delete ui;

    // CleanUp tempolary files
    QFile::remove(m_plotFilePath);
}

void MainWindow::onExecuteClicked()
{
    disconnect(m_rconsole, 0, this, 0);
    m_rconsole->execute(QString("png(\"%1\")").arg(m_plotFilePath));

    connect(m_rconsole, SIGNAL(write(QString)), SLOT(onRMessage(QString)));
    connect(m_rconsole, SIGNAL(error(QString)), SLOT(onRMessage(QString)));
    m_rconsole->execute(ui->leInput->text());

    disconnect(m_rconsole, 0, this, 0);
    m_rconsole->execute("dev.off()");

    QImage plot;
    if (plot.load(m_plotFilePath))
        ui->lbPlot->setPixmap(QPixmap::fromImage(plot));
}

void MainWindow::onRMessage(const QString &message)
{
    ui->teConsole->append(message);
}
