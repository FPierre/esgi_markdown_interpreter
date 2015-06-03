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
    QString ThemeCSS;
    bool debugMode;
    bool openFile;
    bool openWeb;

private slots:

 void on_Debug_stateChanged(int arg1);

 void on_OpenFileAfterConvert_stateChanged(int arg1);

 void on_Convert_clicked();

 void on_SourceFileBtn_clicked();

 void on_DestFolderBtn_clicked();

 void on_DesFolderGroup_clicked();

 void on_OpenWebCheck_stateChanged(int arg1);

 void on_ThemeCombo_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
