#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    defaultPath(QDir::homePath()),
    debug(false)
{
    ui->setupUi(this);

    utils = new Utils;
    utils->debug = debug;

    // Set default dirs
    dir_left = new QDir(defaultPath);
    dir_right = new QDir(defaultPath);

    // Init app
    prepareFileExplorer();

    // Add actions
    ui->left->addAction(ui->actionPressEnter);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fs_model_left;
    delete fs_model_right;
    delete dir_left;
    delete dir_right;
    delete utils;
}

void MainWindow::prepareFileExplorer()
{
    // setup left side
    fs_model_left = new QFileSystemModel();
    fs_model_left->setReadOnly(true);
    fs_model_left->setFilter(QDir::AllEntries | QDir::NoDot);
    fs_model_left->setRootPath(dir_left->path());
    ui->left->setModel(fs_model_left);
    ui->left->setRootIndex(fs_model_left->index(dir_left->path()));
    ui->left->setStyleSheet("QListView#left:focus { border: 1px solid blue; }");
    ui->left->setIconSize(QSize(12,12));
    ui->left->show();

    // setup right side
    fs_model_right = new QFileSystemModel();
    fs_model_right->setReadOnly(true);
    fs_model_right->setFilter(QDir::AllEntries | QDir::NoDot);
    fs_model_right->setRootPath(dir_right->path());
    ui->right->setModel(fs_model_right);
    ui->right->setRootIndex(fs_model_right->index(dir_right->path()));
    ui->right->setStyleSheet("QListView#right:focus { border: 1px solid blue; }");
    ui->right->setIconSize(QSize(12,12));
    ui->right->show();

}

QString MainWindow::getSourcePath()
{
    QString sourcePath;

    if(ui->left->hasFocus())
        sourcePath = dir_left->path() + QDir::separator() + ui->left->currentIndex().data().toString();
    if(ui->right->hasFocus())
        sourcePath = dir_right->path() + QDir::separator() + ui->right->currentIndex().data().toString();

    return sourcePath;
}

QString MainWindow::getDestinationPath()
{
    QString destinationPath;

    if(ui->left->hasFocus())
        destinationPath = dir_right->path() + QDir::separator() + ui->left->currentIndex().data().toString();
    if(ui->right->hasFocus())
        destinationPath = dir_left->path() + QDir::separator() + ui->right->currentIndex().data().toString();

    return destinationPath;
}

QString MainWindow::getCurrentPath()
{
    QString path;

    if(ui->left->hasFocus())
        path = dir_left->path();
    if(ui->right->hasFocus())
        path = dir_right->path();

    return path;
}

// Function show current index when folder been loaded
void MainWindow::modelLeft_directoryLoaded()
{
    // Set sorting in column
    fs_model_left->sort(0, Qt::AscendingOrder);

    // Set current index in listView
    ui->left->setCurrentIndex(fs_model_left->index(0,0, ui->left->rootIndex()));

    if(debug == true)
        qDebug() << "[LeftSide] Folder loaded.";
}

// Function show current index when folder been loaded
void MainWindow::modelRight_directoryLoaded()
{
    // Set sorting in column
    fs_model_right->sort(0, Qt::AscendingOrder);

    // Set current index in listView
    ui->right->setCurrentIndex(fs_model_right->index(0,0, ui->right->rootIndex()));

    if(debug == true)
        qDebug() << "[RightSide] Folder loaded.";
}

// Function go to directory and show content
bool MainWindow::changeRoot()
{
    bool isDir = true;
    // If left side is actual use...
    if(ui->left->hasFocus()) {
        if(!dir_left->cd(ui->left->currentIndex().data().toString()))
            isDir = false;
        fs_model_left->setRootPath(dir_left->path());
        ui->left->setRootIndex(fs_model_left->index(dir_left->path()));

        if(debug == true)
            qDebug() << "[LeftSide] Go to " + dir_left->dirName() + " directory.";
    }

    // If right side is actual use...
    if(ui->right->hasFocus()) {
        if(!dir_right->cd(ui->right->currentIndex().data().toString()))
            isDir = false;
        fs_model_right->setRootPath(dir_right->path());
        ui->right->setRootIndex(fs_model_right->index(dir_right->path()));

        if(debug == true)
            qDebug() << "[RightSide] Go to " + dir_right->dirName() + " directory.";
    }

    return isDir;
}

// Double click
void MainWindow::on_left_doubleClicked(const QModelIndex & index)
{
    // If changeRoot(false) run app
    if(!changeRoot()) {
        QString path = getCurrentPath();
        switch(QMessageBox::question(this,
                                     "Run?",
                                     QString("Run this file:\n " + path + " ?"),
                                     QMessageBox::Yes, QMessageBox::No))
        {
        case QMessageBox::Yes:
            QDesktopServices::openUrl(QUrl(path));
            break;
        }
    }
}

// Double click
void MainWindow::on_right_doubleClicked(const QModelIndex & index)
{
    // If changeRoot(false) run app
    if(!changeRoot()) {
        QString path = getCurrentPath();
        switch(QMessageBox::question(this,
                                     "Run?",
                                     QString("Run this file:\n " + path + " ?"),
                                     QMessageBox::Yes, QMessageBox::No))
        {
        case QMessageBox::Yes:
            QDesktopServices::openUrl(QUrl(path));
            break;
        }
    }
}

// Copy action
void MainWindow::on_actionToolbarCopy_triggered()
{
    QString sourcePath = getSourcePath();
    //QString fromPath = getCurrentPath();
    QString destinationPath = getDestinationPath();

    QStringList fileList;

    fileList << sourcePath;

    // Show question
    switch(QMessageBox::question(this,
                                 "Copy?",
                                 QString("Are you sure to copy this item:"
                                         "\n '" + sourcePath + "' to '" + destinationPath + "' ?"),
                                 QMessageBox::Yes, QMessageBox::No))
    {
    case QMessageBox::Yes:
        break;
    case QMessageBox::No:
        qDebug() << "[Copy] Aborted.";
        return;
    }

    // If copy dir
    if(utils->dirExist(sourcePath)) {
        if(utils->copyDirs(sourcePath, destinationPath))
            QMessageBox::information(this, "Copied!", QString("Dir copied from:\n " + sourcePath + " to " + destinationPath), QMessageBox::NoButton);
        else
            QMessageBox::critical(this, "Error!", QString("Ups! Something is wrong. I can't copied dir from:\n" + sourcePath + " to " + destinationPath), QMessageBox::NoButton);
    }

    // If copy file
    if(!utils->dirExist(sourcePath)) {
        if(utils->copyFiles(fileList, sourcePath, destinationPath)) {
            QMessageBox::information(this, "Copied!", QString("File/s copied from:\n " + sourcePath + " to " + destinationPath), QMessageBox::NoButton);
        }else
            QMessageBox::critical(this, "Error!", QString("Ups! Something is wrong. I can't copied file/s from:\n" + sourcePath + " to " + destinationPath), QMessageBox::NoButton);
    }
}


// Move action
void MainWindow::on_actionToolbarMove_triggered()
{
    QString sourcePath = getSourcePath();
    QString destinationPath = getDestinationPath();
    QStringList deleteList;
    bool isDir = utils->dirExist(sourcePath);


    // Show question
    switch(QMessageBox::question(this,
                                 "Move?",
                                 QString("Are you sure to move this item:"
                                         "\n '" + sourcePath + "' to '" + destinationPath + "' ?"),
                                 QMessageBox::Yes, QMessageBox::No))
    {
    case QMessageBox::Yes:
        break;
    case QMessageBox::No:
        qDebug() << "[Move] Aborted.";
        return;
    }

    deleteList << sourcePath;

    if(isDir) {
        if(utils->copyDirs(sourcePath, destinationPath)) {
            if(debug == true)
                qDebug() << "[Move] Copied " << sourcePath;
        }else {
            if(debug == true)
                qDebug() << "[Move] Ups! Error copy dir from " << sourcePath;
            return;
        }
    }

    if(!isDir) {
        if(utils->copyFiles(deleteList, sourcePath, destinationPath)) {
            if(debug == true)
                qDebug() << "[Move] Copied " << sourcePath;
        }else {
            if(debug == true)
                qDebug() << "[Move] Ups! Error copy file/s from " << sourcePath;
            return;
        }
    }

    if(utils->deleteAll(deleteList))
        QMessageBox::information(this, "Moved!", QString("Item moved from:\n" + sourcePath + " to " + destinationPath), QMessageBox::NoButton);
    else
        QMessageBox::critical(this, "Error!", QString("Ups! Something is wrong. I can't moved item from:\n" + sourcePath + " to " + destinationPath), QMessageBox::NoButton);
}

// Remove action
void MainWindow::on_actionToolbarRemove_triggered()
{
    QString sourcePath = getSourcePath();
    QStringList deleteList;

    // Show question
    switch(QMessageBox::question(this,
                                 "Usunać?",
                                 QString("Are you sure to delete this item:\n"
                                         "'" + sourcePath + "' ?"),
                                 QMessageBox::Yes, QMessageBox::No))
    {
    case QMessageBox::Yes:
        break;
    case QMessageBox::No:
        qDebug() << "[Delete] Aborted.";
        return;
    }

    deleteList << sourcePath;

    if(utils->deleteAll(deleteList)) {
        qDebug() << "[Delete] Deleted " << deleteList.length() << " item/s.";
    }else
        qDebug() << "[Delete] Error not deleted.";
}

// Enter action
void MainWindow::on_actionPressEnter_triggered()
{
    if(ui->right->hasFocus())
        on_right_doubleClicked(ui->right->currentIndex());
    if(ui->left->hasFocus())
        on_left_doubleClicked(ui->left->currentIndex());
}

// Go Home action F2
void MainWindow::on_actionToolbarGoHome_triggered()
{
    if(ui->left->hasFocus()) {
        dir_left->setPath(defaultPath);
        ui->left->setRootIndex(fs_model_left->setRootPath(defaultPath));
    }

    if(ui->right->hasFocus()) {
        dir_right->setPath(defaultPath);
        ui->right->setRootIndex(fs_model_right->setRootPath(defaultPath));
    }
}


// Quit action
void MainWindow::on_actionQuit_triggered()
{
    // Show question
    switch(QMessageBox::question(this,
                                 "Quit?",
                                 QString("Are you sure to quit File Explorer ?"),
                                 QMessageBox::Yes, QMessageBox::No))
    {
    case QMessageBox::Yes:
        QApplication::quit();
        break;
    case QMessageBox::No:
        return;
    }
}

void MainWindow::on_left_customContextMenuRequested()
{
    QMenu contextMenu;
    bool isDir = utils->dirExist(dir_left->path() + QDir::separator() + ui->left->currentIndex().data().toString());

    if(!isDir) {
        contextMenu.addAction("Run");
        contextMenu.addSeparator();
    }

    contextMenu.addAction("Copy");
    contextMenu.addAction("Move");
    contextMenu.addAction("Delete");
    contextMenu.setDefaultAction(NULL);

    // Get option from context menu
    QAction *selectedItem = contextMenu.exec(ui->left->cursor().pos());

    if(selectedItem != NULL) {
        QString choose = selectedItem->text();

        if(choose == "Run")
            on_left_doubleClicked(ui->left->currentIndex());

        if(choose == "Copy")
            ui->actionToolbarCopy->triggered();

        if(choose == "Move")
            ui->actionToolbarMove->triggered();

        if(choose == "Delete")
            ui->actionToolbarRemove->triggered();
    }
}

void MainWindow::on_right_customContextMenuRequested()
{
    QMenu contextMenu;
    bool isDir = utils->dirExist(dir_right->path() + QDir::separator() + ui->right->currentIndex().data().toString());

    // If selected item isn't dir
    // add this option to menu
    if(!isDir) {
        contextMenu.addAction("Run");
        contextMenu.addSeparator();
    }

    contextMenu.addAction("Copy");
    contextMenu.addAction("Move");
    contextMenu.addAction("Delete");
    contextMenu.setDefaultAction(NULL);

    // Get option from context menu
    QAction *selectedItem = contextMenu.exec(ui->right->cursor().pos());

    if(selectedItem != NULL) {
        QString choose = selectedItem->text();

        if(choose == "Run")
            on_left_doubleClicked(ui->right->currentIndex());

        if(choose == "Copy")
            ui->actionToolbarCopy->triggered();

        if(choose == "Move")
            ui->actionToolbarMove->triggered();

        if(choose == "Delete")
            ui->actionToolbarRemove->triggered();
    }
}

void MainWindow::on_actionInfo_triggered()
{
    QDialog info;
    info.setWindowTitle("Information");
    info.setMinimumWidth(200);
    info.setMinimumHeight(70);
    info.setContentsMargins(QMargins(10, 10, 10, 10));

    QLabel *title = new QLabel(&info);
    title->setText("File Explorer");
    title->setAlignment(Qt::AlignCenter);
    title->setFont(QFont("Arial", 20, 600));
    title->setFixedWidth(info.minimumWidth());

    QLabel *author = new QLabel(&info);
    author->setText("by wernix");
    author->setFont(QFont("Arial", 10, 400, true));
    author->setAlignment(Qt::AlignRight);
    author->setFixedWidth(info.minimumWidth() - 20);

    QVBoxLayout *layout = new QVBoxLayout(&info);
    layout->addWidget(title);
    layout->addWidget(author);

    info.setLayout(layout);
    info.exec();
}

void MainWindow::on_actionToolbarQuit_triggered()
{
    on_actionQuit_triggered();
}
