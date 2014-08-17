#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    defaultPath(QDir::homePath()),
    debug(false)
{
    ui->setupUi(this);

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
        QString path = dir_left->path() + QDir::separator() + index.data().toString();
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
        QString path = dir_right->path() + QDir::separator() + index.data().toString();
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
void MainWindow::on_actionCp_triggered()
{
    QString fileName;
    QString sourcePath;
    QString fromPath;
    QString destinationPath;

    QStringList fileList;

    // If left side is actual use...
    if(ui->left->hasFocus()) {
        sourcePath = dir_left->path() + QDir::separator() + ui->left->currentIndex().data().toString();
        destinationPath = dir_right->path() + QDir::separator() + ui->left->currentIndex().data().toString();
        fromPath = dir_left->path();
        fileList << sourcePath;
    }

    // If right side is actual use...
    if(ui->right->hasFocus()) {
        sourcePath = dir_right->path() + QDir::separator() + ui->right->currentIndex().data().toString();
        destinationPath = dir_left->path() + QDir::separator() + ui->right->currentIndex().data().toString();
        fromPath = dir_right->path();
        fileList << sourcePath;
    }

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
    if(dirExist(sourcePath)) {
        if(copyDirs(sourcePath, destinationPath))
            QMessageBox::information(this, "Copied!", QString("Dir copied from:\n " + sourcePath + " to " + destinationPath), QMessageBox::NoButton);
        else
            QMessageBox::critical(this, "Error!", QString("Ups! Something is wrong. I can't copied dir from:\n" + sourcePath + " to " + destinationPath), QMessageBox::NoButton);
    }

    // If copy file
    if(!dirExist(sourcePath)) {
        if(copyFiles(fileList, sourcePath, destinationPath)) {
            QMessageBox::information(this, "Copied!", QString("File/s copied from:\n " + sourcePath + " to " + destinationPath), QMessageBox::NoButton);
        }else
            QMessageBox::critical(this, "Error!", QString("Ups! Something is wrong. I can't copied file/s from:\n" + sourcePath + " to " + destinationPath), QMessageBox::NoButton);
    }
}

// Check dir exist
bool MainWindow::dirExist(QString sourcePath)
{
    QDir dir(sourcePath);
    return dir.exists();
}

bool MainWindow::deleteAll(QStringList list)
{
    bool ok = false;

    for(int i = 0; i < list.length(); i++) {
        bool isDir = dirExist(list.at(i));

        if(isDir) {
            QDir(list.at(i)).removeRecursively();
            ok = true;
        }else {
            QDir().remove(list.at(i));
            ok = true;
        }

        if(debug == true)
            qDebug() << "[Delete] Deleted " << list.at(i);

        if(!ok)
            if(debug == true)
                qDebug() << "[Delete] Error:" << list.at(i);

    }

    return ok;
}

// Copy single files function
bool MainWindow::copyFiles(QStringList list, QString sourcePath, QString destinationPath)
{
    bool ok = true;

    for(int i = 0; i < list.length(); i++)
    {
        QFile file(list.at(i));
        QString newFile = list.at(i);
        newFile.replace(sourcePath, destinationPath);

        if(file.copy(newFile)) {
            if(debug == true)
                qDebug() << "[Copy] Copied " << list.at(i);
        }else {
            if(debug == true)
                qDebug() << "[Copy] Error: " << list.at(i);
            ok = false;
        }
    }

    return ok;
}

// Move action
void MainWindow::on_actionMv_triggered()
{
    QString sourcePath;
    QString destinationPath;
    QStringList deleteList;
    bool isDir = dirExist(sourcePath);

    // If left side is actual use...
    if(ui->left->hasFocus()) {
        sourcePath = dir_left->path() + QDir::separator() + ui->left->currentIndex().data().toString();
        destinationPath = dir_right->path() + QDir::separator() + ui->left->currentIndex().data().toString();
    }

    // If right side is actual use...
    if(ui->right->hasFocus()) {
        sourcePath = dir_right->path() + QDir::separator() + ui->right->currentIndex().data().toString();
        destinationPath = dir_left->path() + QDir::separator() + ui->right->currentIndex().data().toString();
    }

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
        if(copyDirs(sourcePath, destinationPath)) {
            if(debug == true)
                qDebug() << "[Move] Copied " << sourcePath;
        }else {
            if(debug == true)
                qDebug() << "[Move] Ups! Error copy dir from " << sourcePath;
            return;
        }
    }

    if(!isDir) {
        if(copyFiles(deleteList, sourcePath, destinationPath)) {
            if(debug == true)
                qDebug() << "[Move] Copied " << sourcePath;
        }else {
            if(debug == true)
                qDebug() << "[Move] Ups! Error copy file/s from " << sourcePath;
            return;
        }
    }

    if(deleteAll(deleteList))
        QMessageBox::information(this, "Moved!", QString("Item moved from:\n" + sourcePath + " to " + destinationPath), QMessageBox::NoButton);
    else
        QMessageBox::critical(this, "Error!", QString("Ups! Something is wrong. I can't moved item from:\n" + sourcePath + " to " + destinationPath), QMessageBox::NoButton);
}

// Remove action
void MainWindow::on_actionRm_triggered()
{
    QString sourcePath;
    QStringList deleteList;
    bool left = ui->left->hasFocus();
    bool right = ui->right->hasFocus();

    if(left)
        sourcePath = dir_left->path() + QDir::separator() + ui->left->currentIndex().data().toString();
    if(right)
        sourcePath = dir_right->path() + QDir::separator() + ui->right->currentIndex().data().toString();

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

    if(deleteAll(deleteList)) {
        qDebug() << "[Delete] Deleted " << deleteList.length() << " item/s.";
    }else
        qDebug() << "[Delete] Error not deleted.";
}

// Enter action
void MainWindow::on_actionPressEnter_triggered()
{
    bool left = ui->left->hasFocus();
    bool right = ui->right->hasFocus();

    if(right)
        on_right_doubleClicked(ui->right->currentIndex());
    if(left)
        on_left_doubleClicked(ui->left->currentIndex());
}

// Go Home action F2
void MainWindow::on_actionGoHome_triggered()
{
    bool left = ui->left->hasFocus();
    bool right = ui->right->hasFocus();

    if(left)
        ui->left->setRootIndex(fs_model_left->setRootPath(defaultPath));

    if(right)
        ui->right->setRootIndex(fs_model_right->setRootPath(defaultPath));
}

// Create dirs function
bool MainWindow::mkDirs(QStringList list)
{
    bool ok = true;

    for(int i = 0; i < list.length(); i++) {
        if(QDir().mkpath(list.at(i))) {
            if(debug == true)
                qDebug() << "[mkDirs] Created " << list.at(i);
        }else {
            if(debug == true)
                qDebug() << "[mkDirs] Error: " << list.at(i);
            ok = false;
        }
    }

    return ok;
}

// Copy dirs (files, sub-dirs) functions
bool MainWindow::copyDirs(QString sourcePath, QString destinationPath)
{
    bool ok = true;

    QStringList filesList;
    QStringList dirsList;

    QDirIterator files(sourcePath, QDir::Files, QDirIterator::NoIteratorFlags);
    while(files.hasNext()) {
        files.next();
        filesList << files.filePath();
    }

    // Add all sub dirs in copy dirs list
    QDirIterator subDirs(sourcePath, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    dirsList << QString(sourcePath).replace(sourcePath, destinationPath);
    while(subDirs.hasNext()) {
        subDirs.next();
        dirsList << subDirs.filePath().replace(sourcePath, destinationPath);

        // Add files from sub dirs to copy list
        QDirIterator files(subDirs.filePath(), QDir::Files, QDirIterator::NoIteratorFlags);
        while(files.hasNext()) {
            files.next();
            filesList << files.filePath();
        }
    }

    if(dirsList.length()) {
        if(mkDirs(dirsList)) {
            qDebug() << "[mkDirs] Created " << dirsList.length() << " dirs.";
        }else {
            qDebug() << "[mkDirs] Ups! Something is wrong.";
            ok = false;
        }
    }

    if(filesList.length()) {
        if(copyFiles(filesList, sourcePath, destinationPath)) {
            qDebug() << "[Copy] Copy " << filesList.length() << " files.";
        }else {
            qDebug() << "[Copy] Ups! Something is wrong.";
            ok = false;
        }
    }

    return ok;
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
    bool isDir = dirExist(dir_left->path() + QDir::separator() + ui->left->currentIndex().data().toString());

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
            ui->actionCp->triggered();

        if(choose == "Move")
            ui->actionMv->triggered();

        if(choose == "Delete")
            ui->actionRm->triggered();
    }
}

void MainWindow::on_right_customContextMenuRequested()
{
    QMenu contextMenu;
    bool isDir = dirExist(dir_right->path() + QDir::separator() + ui->right->currentIndex().data().toString());

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
            ui->actionCp->triggered();

        if(choose == "Move")
            ui->actionMv->triggered();

        if(choose == "Delete")
            ui->actionRm->triggered();
    }
}
