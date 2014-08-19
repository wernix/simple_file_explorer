#include "utils.h"

Utils::Utils(QObject *parent) :
    QObject(parent)
{

}


// Create dirs function
bool Utils::mkDirs(QStringList list)
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
bool Utils::copyDirs(QString sourcePath, QString destinationPath)
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


// Copy single files function
bool Utils::copyFiles(QStringList list, QString sourcePath, QString destinationPath)
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


bool Utils::deleteAll(QStringList list)
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


// Check dir exist
bool Utils::dirExist(QString sourcePath)
{
    QDir dir(sourcePath);
    return dir.exists();
}


