#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <RInside/rconsole.h>
#include <QFile>

#include "Console.h"
#include <QDebug>

MainWindow::MainWindow(RConsole *r, QWidget *parent) :
    QMainWindow(parent),
    m_rconsole(r),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_guiConsole = new Console(this);

    ui->horizontalLayout->insertWidget(0, m_guiConsole);

    connect(m_guiConsole, SIGNAL(command(QString)), SLOT(onExecuteClicked(QString)));
    connect(ui->btnUpdatePlot, SIGNAL(clicked()), SLOT(updatePlot()));
    connect(&m_outputTimer, SIGNAL(timeout()), SLOT(printOutputBuf()));

    // Tempolary file for R plots
    m_plotFilePath = "tmpPlot.png";
    QFile::remove(m_plotFilePath);

    m_outputTimer.setInterval(50);
    m_outputTimer.setSingleShot(true);

    m_rconsole->execute(QString("png(\"%1\")").arg(m_plotFilePath));
}

MainWindow::~MainWindow()
{
    delete ui;

    // CleanUp tempolary files
    QFile::remove(m_plotFilePath);
}

void MainWindow::onExecuteClicked(const QString &command)
{
    disconnect(m_rconsole, 0, this, 0);

    connect(m_rconsole, SIGNAL(write(QString)), SLOT(onRMessageOk(QString)));
    connect(m_rconsole, SIGNAL(error(QString)), SLOT(onRMessageError(QString)));
    connect(m_rconsole, SIGNAL(parseIncomplete(QString)), SLOT(onRParseIncomplete()));

    m_rconsole->execute(command);
}

void MainWindow::onRMessageOk(const QString &message)
{
    m_outputBuf.append(message);
    m_outputTimer.start();
}

void MainWindow::onRMessageError(const QString &message)
{
    if (message != m_outputBuf) {
        m_outputBuf.append(message);
        printOutputBuf();
    }
}

void MainWindow::onRParseIncomplete()
{
    m_guiConsole->extraInput();
}

void MainWindow::updatePlot()
{
    m_rconsole->execute("dev.off()");

    QImage plot;
    if (plot.load(m_plotFilePath))
        ui->lbPlot->setPixmap(QPixmap::fromImage(plot));
}

void MainWindow::printOutputBuf()
{
    if (m_outputBuf.endsWith("\n"))
        m_outputBuf.chop(1);

    m_guiConsole->output(m_outputBuf);
    m_lastOutput = m_outputBuf;
    m_outputBuf.clear();
}
