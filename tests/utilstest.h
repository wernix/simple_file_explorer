#ifndef COPYTEST_H
#define COPYTEST_H
#include <QObject>
#include <QtTest/QtTest>
#include "utils.h"

class UtilsTest : public QObject
{
    Q_OBJECT
public:
    explicit UtilsTest(QObject *parent = 0);

private slots:
    void dir_data();
    void dir();

    void mkdir_data();
    void mkdir();

private:
    Utils utils;
};

#endif // COPYTEST_H
