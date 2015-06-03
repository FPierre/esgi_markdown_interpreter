#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QSysInfo>

#define MYSYSTEM QSysInfo::productType()

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    FileName = QString();
    DestDir = QString();
    ThemeCSS = "Default";
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
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

    if(!ThemeCSS.isEmpty()) {
        result += "CSS theme selected :" + ThemeCSS + "\n";
    }
    if(debugMode) {
        result += "Debug Mode On! \n";
    }

    if(openFile) {
        result += "Open File after convert selected! \n";
    }

    if(openWeb) {
        result += "Open with webBrowser after convert selected! \n";
    }

    result += "Convert Start !\n";
    ui->ResultText->setPlainText(result);
}


void MainWindow::on_SourceFileBtn_clicked()
{
    QString folder = "/home/";
    qDebug() << "sourceButtonClicked";
    qDebug() << MYSYSTEM;

    if (MYSYSTEM == "android" || MYSYSTEM == "osx") {
        folder = "/home/";
    } else if (MYSYSTEM == "windows"){
        folder = "C:/";
    }
    FileName = QFileDialog::getOpenFileName(this,
        tr("Open Markdown"), folder, tr("Markdown Files( *.markdown *.mdown *.mkdn *.md *.mkd *.mdwn *.mdtxt *.mdtext *.text"));
    ui->SourceFileText->setText(FileName);
    if (!FileName.isEmpty() && !DestDir.isEmpty()) {
        ui->Convert->setEnabled(true);
    }
    qDebug() << FileName;
}

void MainWindow::on_DestFolderBtn_clicked()
{
    qDebug() << "DestinationButtonClicked";
    DestDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    ui->DestFolderText->setText(DestDir);
    if (!FileName.isEmpty() && !DestDir.isEmpty()) {
        ui->Convert->setEnabled(true);
    }
    qDebug() << DestDir;
}

void MainWindow::on_DesFolderGroup_clicked()
{
    on_DestFolderBtn_clicked();
}

void MainWindow::on_OpenWebCheck_stateChanged(int arg1)
{
    qDebug() << "Open with webBrowser :" << arg1;
    if (arg1 > 0) {
        openWeb = 1;
    }
}

void MainWindow::on_ThemeCombo_currentIndexChanged(const QString &arg1)
{
    ThemeCSS = arg1;
    qDebug() << "Theme changed :" << arg1;
}
