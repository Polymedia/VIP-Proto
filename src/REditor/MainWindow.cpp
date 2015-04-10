#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <rconsole.h>
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
    connect(&m_waitOutputTimer, SIGNAL(timeout()), SLOT(onWainExtaInput()));

    // Tempolary file for R plots
    m_plotFilePath = "tmpPlot.png";
    QFile::remove(m_plotFilePath);

    m_outputTimer.setInterval(50);
    m_outputTimer.setSingleShot(true);

    m_waitOutputTimer.setInterval(75);
    m_waitOutputTimer.setSingleShot(true);
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
    m_rconsole->execute(QString("png(\"%1\")").arg(m_plotFilePath));

    connect(m_rconsole, SIGNAL(write(QString)), SLOT(onRMessageOk(QString)));
    connect(m_rconsole, SIGNAL(error(QString)), SLOT(onRMessageError(QString)));

    m_waitOutputTimer.start();

    m_rconsole->execute(command);

    disconnect(m_rconsole, 0, this, 0);
    m_rconsole->execute("dev.off()");
}

void MainWindow::onRMessageOk(const QString &message)
{
    m_waitOutputTimer.stop();

    m_outputBuf.append(message);
    m_outputTimer.start();
}

void MainWindow::onRMessageError(const QString &message)
{
    m_waitOutputTimer.stop();

    if (message != m_outputBuf && message != m_lastOutput) {
        m_outputBuf.append(message);
        printOutputBuf();
    }
}

void MainWindow::updatePlot()
{
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

void MainWindow::onWainExtaInput()
{
    m_guiConsole->extraInput();
}
