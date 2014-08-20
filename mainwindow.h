#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QLabel>
#include <QFileSystemModel>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QMenu>
#include <QLayout>

#include "utils.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;

    Utils *utils;

    QString defaultPath;
    QFileSystemModel *fs_model_left;
    QFileSystemModel *fs_model_right;
    QDir *dir_left, *dir_right;
    bool debug;

    void prepareFileExplorer();
    QString getSourcePath();
    QString getDestinationPath();
    QString getCurrentPath();
    QString getItemName();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void modelLeft_directoryLoaded();
    void modelRight_directoryLoaded();
    bool changeRoot();

    void on_left_doubleClicked(const QModelIndex & index);
    void on_right_doubleClicked(const QModelIndex & index);
    void on_actionToolbarCopy_triggered();
    void on_actionToolbarMove_triggered();
    void on_actionToolbarRemove_triggered();
    void on_actionPressEnter_triggered();
    void on_actionToolbarGoHome_triggered();
    void on_actionQuit_triggered();
    void on_left_customContextMenuRequested();
    void on_right_customContextMenuRequested();
    void on_actionInfo_triggered();
    void on_actionToolbarQuit_triggered();
    void on_actionToolbarRename_triggered();
    void on_actionToolbarMkdir_triggered();
};

#endif // MAINWINDOW_H
