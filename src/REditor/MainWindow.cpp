#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QtCore>
#include <QtWidgets>

#include <RInside/rconsole.h>
#include <RInside/rmodel.h>
#include <RInside/csvmodel.h>

#include "Console.h"
#include "rvariablewidget.h"

const QString editorName = "REditor";

MainWindow::MainWindow(RConsole &r, QWidget *parent) :
    QMainWindow(parent),
    m_rconsole(r),
    m_editorTextChanged(false),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->editor->setAcceptDrops(false);
    ui->console->setAcceptDrops(false);

    // для dock widgets нужен centralWidget. скрываем его установив размер (1,1)
    // для правильной работы виджетов распологаем их в две строки, поделенные пополам
    // editorDock  | envDock
    // consoleDock | graphicsDock

    centralWidget()->setMaximumSize(1,1);

    splitDockWidget(ui->editorDock, ui->envDock, Qt::Horizontal);
    splitDockWidget(ui->consoleDock, ui->graphicsDock, Qt::Horizontal);

    // Tempolary file for R plots
    m_plotFilePath = "tmpPlot.png";
    QFile::remove(m_plotFilePath);

    initR();
    clearEditor(false);

    ui->listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    // after clear & execute png
    connect(ui->editor, &QPlainTextEdit::textChanged, [&] () {m_editorTextChanged = true;});
    connect(ui->console, SIGNAL(command(QString)), SLOT(onCommand(QString)));
    connect(ui->console, SIGNAL(enterPressed()), this, SLOT(updateVariables()));
}

MainWindow::~MainWindow()
{
    delete ui;

    // CleanUp tempolary files
    QFile::remove(m_plotFilePath);
}

void MainWindow::initR()
{
    //Load data from csv
    QDir inputs("inputs");
    int counter = 0;
    foreach (QString filename, inputs.entryList(QStringList() << "*.csv")) {
        counter++;
        CsvModel model;

        QFile file(inputs.absoluteFilePath(filename));
        if (!file.open(QFile::ReadOnly)) {
            qWarning() << "Cannot open input data file";
            continue;
        }

        model.load(&file, ';', true);

        QString inVarName = "In_" + QString::number(counter);
        m_rconsole[inVarName] = RObject::fromModel(&model);
        addVar(inVarName);
    }
}

void MainWindow::clearR()
{
    m_rconsole.execute("rm(list=ls()");
}


void MainWindow::setEditorFile(const QString &fileName)
{
    m_editorTextChanged = false;

    setWindowTitle(editorName + " - " + fileName);
    m_fileName = fileName;
}

void MainWindow::onCommand(const QString &command)
{
    disconnect(&m_rconsole, 0, this, 0);

    connect(&m_rconsole, SIGNAL(write(QString)), SLOT(onRMessageOk(QString)));
    connect(&m_rconsole, SIGNAL(error(QString)), SLOT(onRMessageError(QString)));
    connect(&m_rconsole, SIGNAL(parseIncomplete(QString)), SLOT(onRParseIncomplete()));

    m_rconsole.execute(QString("png(\"%1\")").arg(m_plotFilePath));
    m_rconsole.execute(command);

    printOutputBuf();
    m_lastOutput.clear();
}

void MainWindow::addVar(const QString &s)
{
    RVariableWidget *widget = new RVariableWidget(&m_rconsole, s);
    QListWidgetItem *newWidget = new QListWidgetItem(ui->listWidget, QListWidgetItem::UserType);
    newWidget->setSizeHint(widget->size());
    ui->listWidget->setItemWidget(newWidget, widget);
}

void MainWindow::updateVariables()
{
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        RVariableWidget *rVar = dynamic_cast<RVariableWidget *>(ui->listWidget->itemWidget(ui->listWidget->item(i)));
        rVar->updateVar();
        ui->listWidget->item(i)->setSizeHint(rVar->size());
    }
}

void MainWindow::onRMessageOk(const QString &message)
{
    m_outputBuf.append(message);
    m_lastOutput = message;
}

void MainWindow::onRMessageError(const QString &message)
{
    if (message != m_lastOutput)
        m_outputBuf.append(message);
}

void MainWindow::onRParseIncomplete()
{
    ui->console->extraInput();
}

void MainWindow::updatePlot()
{
    m_rconsole.execute("dev.off()");

    QImage plot;
    if (plot.load(m_plotFilePath)) {
        QPixmap pm = QPixmap::fromImage(plot).scaled(ui->lbPlot->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->lbPlot->setPixmap(pm);
    }
}

void MainWindow::printOutputBuf()
{
    if (m_outputBuf.endsWith("\n"))
        m_outputBuf.chop(1);

    ui->console->output(m_outputBuf);
    m_outputBuf.clear();
}

void MainWindow::clearEditor(bool clearAll)
{
    m_fileName = "";

    ui->editor->clear();
    m_editorTextChanged = false;

    if (clearAll) {
        ui->console->clear();
        clearR();
    }
    setWindowTitle(editorName + " - *");
}

void MainWindow::onNew()
{
    if (m_editorTextChanged) {
        QMessageBox mb(this);
        mb.setWindowTitle("New file");
        mb.setText("Save current document before create new?");
        mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        mb.setDefaultButton(QMessageBox::Cancel);

        int res = mb.exec();

        switch (res) {
        case QMessageBox::Yes:
            onSave();
            clearEditor(false);
            break;
        case QMessageBox::No:
            clearEditor(false);
            break;
        case QMessageBox::Cancel:
        default:
            break;
        }
    }
}

void MainWindow::onOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open file",
                                                    QDir::homePath(),
                                                    "R files" + QString(" (*.r *.txt)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        ui->editor->setPlainText(file.readAll());

        setEditorFile(fileName);
    }
}

void MainWindow::onSave(bool saveAs)
{
    QString fileName;
    if (saveAs || m_fileName.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this,
                                                "Save file",
                                                QDir::homePath(),
                                                "R files" + QString(" (*.r *.txt)"));
        if (fileName.isEmpty())
            return;

        m_fileName = fileName;
    }

    QFile file(m_fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(ui->editor->toPlainText().toLocal8Bit().data());

    setEditorFile(m_fileName);
}

void MainWindow::onSaveAs()
{
    onSave(true);

}

void MainWindow::onExecute()
{
    ui->console->execute(ui->editor->toPlainText(), true);

    updateVariables();
}

void MainWindow::onDockToggle(bool checked)
{
    QObject *object = sender();

    if (object == ui->actionEditor)
        ui->editorDock->setVisible(checked);

    else if (object == ui->actionConsole)
        ui->consoleDock->setVisible(checked);

    else if (object == ui->actionGraphics)
        ui->graphicsDock->setVisible(checked);

    else if (object == ui->actionEnvironment)
        ui->envDock->setVisible(checked);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_editorTextChanged) {
        QMessageBox mb(this);
        mb.setWindowTitle("Exit?");
        mb.setText("Save current document before exit?");
        mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        mb.setDefaultButton(QMessageBox::Cancel);

        int res = mb.exec();

        switch (res) {
        case QMessageBox::Yes:
            onSave();
            event->accept();
            break;
        case QMessageBox::No:
            event->accept();
            break;
        case QMessageBox::Cancel:
            event->ignore();
        default:
            break;
        }
    }
}
