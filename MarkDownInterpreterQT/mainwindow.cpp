#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QFileDialog>
#include <QDebug>
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    FileName = QString();
    DestDir = QString();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_m_sourceFileButton_clicked()
{
    qDebug() << "sourceButtonClicked";
    FileName = QFileDialog::getOpenFileName(this,
        tr("Open Markdown"), "/home/", tr("*"));
    qDebug() << FileName;



}

void MainWindow::on_DestinationFile_clicked()
{
    qDebug() << "DestinationButtonClicked";
    DestDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    qDebug() << DestDir;
}


void MainWindow::on_Debug_stateChanged(int arg1)
{
    qDebug() << "Debug Mode :" << arg1;
    if (arg1 > 0) {
        debugMode = 1;
    }
}

void MainWindow::on_OpenFileAfterConvert_stateChanged(int arg1)
{
    qDebug() << "Open file after convert :" << arg1;
    if (arg1 > 0) {
        openFile = 1;
    }
}

void MainWindow::on_Convert_clicked()
{
    QString result;
    if (FileName.isEmpty() || FileName.isNull()) {
        result += "Choose File!\n";
    }
    if (DestDir.trimmed().isEmpty()) {
        result += "Choose Folder!\n";
    }

    if(debugMode) {
        result += "Debug Mode On! \n";
    }

    if(openFile) {
        result += "Open File after convert selected! \n";
    }

    result += "Convert Start !\n";
    ui->ResultText->setPlainText(result);
}
