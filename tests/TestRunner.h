#ifndef TESTRUNNER_H
#define TESTRUNNER_H

// Qt includes
#include <QDebug>
#include <QObject>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QElapsedTimer>

#include <QtTest/QtTest>

#include <iostream>

#define RUNNER_LINE_SIZE 60

class TestRunner
{
public:

    static TestRunner& instance()
    {
        static TestRunner instance;
        return instance;
    }

    void registerTest(QObject *test)
    {
        m_tests.push_back(QSharedPointer<QObject>(test));
    }

    int runTests(const QStringList &filter)
    {
        QString executableName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
        QStringList failedTests;
        QElapsedTimer timer;
        timer.start();                

        // Header
        printBlock(QString("Start testing of ") + executableName);

        // Run
        int errorCode = 0;
        foreach (QSharedPointer<QObject> test, m_tests) {
            std::cout << std::endl;
            if (!filter.isEmpty() && !filter.contains(test.data()->objectName()))
                continue;

            int result = QTest::qExec(test.data());
            if (result > 0)
                failedTests << test.data()->objectName();
            errorCode += result;            
        }

        // Summary
        std::cout << std::endl;
        printTextInSeparatorLine(QString("Summary of ") + executableName, RUNNER_LINE_SIZE);
        std::cout << "Time elapsed: " << timer.elapsed() / 1000.0 << "s" << std::endl;
        if (!failedTests.isEmpty()) {
            std::cout << std::endl << "Following test classes have failed:" << std::endl;
            foreach (const QString &testName, failedTests)
                std::cout << "    " << testName.toStdString() << std::endl;
        } else
            std::cout << std::endl << "OK" << std::endl;

        // End
        printBlock(QString("End testing of ") + executableName);

        return errorCode;
    }

private:
    void printChar(char c, int count = 1)
    {
        for (int i = 0; i < count; i++)
            std::cout << c;
    }

    void printSeparatorChar(int count = 1)
    {
        printChar('*', count);
    }

    void printSeparatorLine(int lineSize = RUNNER_LINE_SIZE)
    {
        printSeparatorChar(lineSize);
        std::cout << std::endl;
    }

    void printTextInSeparatorLine(QString text, int lineSize = RUNNER_LINE_SIZE)
    {
        int rest = lineSize,
            leftMarginSize = 0,
            rightMarginSize = 0;

        rest -= text.length();
        if (rest > 0) {
            leftMarginSize = rest / 2;
            rightMarginSize = rest - leftMarginSize;
        } else {
            text.resize(RUNNER_LINE_SIZE);
        }

        printSeparatorChar(leftMarginSize - 1);
        printChar(' ', 1);
        std::cout << text.toStdString();
        printChar(' ', 1);
        printSeparatorChar(rightMarginSize - 1);
        std::cout << std::endl;
    }

    void printBlock(const QString &text, int lineSize = RUNNER_LINE_SIZE)
    {
        std::cout << std::endl;
        printSeparatorLine(lineSize);
        printTextInSeparatorLine(text, lineSize);
        printSeparatorLine(lineSize);
    }

private:
    QList<QSharedPointer<QObject>> m_tests;
};

template <class T>
class TestAdder
{
public:
    TestAdder(const QString &name)
    {
        T *newTest = new T();
        newTest->setObjectName(name);
        TestRunner::instance().registerTest(newTest);
    }
};

#endif // TESTRUNNER_H
