#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QAction>
#include <QDesktopServices>
#include <QUrl>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;

    QString defaultPath;
    QFileSystemModel *fs_model_left;
    QFileSystemModel *fs_model_right;
    QDir *dir_left, *dir_right;
    bool debug;

    void prepareFileExplorer();
    void cdUp();
    void prepareDirsList();
    void prepareFilesList();
    bool dirExist(QString path);
    bool mkDirs(QStringList list);
    bool copyFiles(QStringList list, QString sourcePath, QString destinationPath);
    bool copyDirs(QString sourcePath, QString destinationPath);
    bool deleteAll(QStringList list);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void modelLeft_directoryLoaded();
    void modelRight_directoryLoaded();
    bool changeRoot();

    void on_left_doubleClicked(const QModelIndex & index);

    void on_right_doubleClicked(const QModelIndex & index);

    void on_actionCp_triggered();

    void on_actionMv_triggered();

    void on_actionRm_triggered();

    void on_actionPressEnter_triggered();

    void on_actionGoHome_triggered();

    void on_actionQuit_triggered();
};

#endif // MAINWINDOW_H
