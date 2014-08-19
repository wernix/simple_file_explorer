#ifndef UTILS_H
#define UTILS_H

#include <QObject>
#include <QDir>
#include <QDebug>
#include <QDirIterator>
#include <QFile>

class Utils : public QObject
{
    Q_OBJECT

private:

public:
    explicit Utils(QObject *parent = 0);

    bool debug;

    bool dirExist(QString path);
    bool mkDirs(QStringList list);
    bool copyFiles(QStringList list, QString sourcePath, QString destinationPath);
    bool copyDirs(QString sourcePath, QString destinationPath);
    bool deleteAll(QStringList list);

signals:

public slots:

};

#endif // UTILS_H
