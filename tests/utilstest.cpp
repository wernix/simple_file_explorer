#include "utilstest.h"

UtilsTest::UtilsTest(QObject *parent) :
    QObject(parent)
{
}

void UtilsTest::dir_data()
{
    QTest::addColumn<QString>("dir");
    QTest::addColumn<bool>("verdict");

    QTest::newRow("Path not existing") << "/not/existing/path" << false;
    QTest::newRow("Path existing") << "/tmp" << true;
}

void UtilsTest::dir()
{
    QFETCH(QString, dir);
    QFETCH(bool, verdict);

    QCOMPARE(utils.dirExist(dir), verdict);
}

void UtilsTest::mkdir_data()
{
    QTest::addColumn<QStringList>("dir");
    QTest::addColumn<bool>("verdict");

    QTest::newRow("Path not existing") << QStringList("/tmp/test") << true;
    QTest::newRow("Path not existing, multilevel") << QStringList("/tmp/a/b/c/d/e/f") << true;
    QTest::newRow("Path existing") << QStringList("/tmp") << false;
}

void UtilsTest::mkdir()
{
    QFETCH(QStringList, dir);
    QFETCH(bool, verdict);

    QCOMPARE(utils.mkDirs(dir), verdict);
}

QTEST_APPLESS_MAIN(UtilsTest)
