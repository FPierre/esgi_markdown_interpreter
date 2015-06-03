#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString FileName;
    QString DestDir;
    bool debugMode;
    bool openFile;

private slots:
 void on_m_sourceFileButton_clicked();

 void on_DestinationFile_clicked();

 void on_Debug_stateChanged(int arg1);

 void on_OpenFileAfterConvert_stateChanged(int arg1);

 void on_Convert_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
