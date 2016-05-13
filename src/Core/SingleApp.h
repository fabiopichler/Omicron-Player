/*******************************************************************************
  Author at: http://wiki.qtcentre.org/index.php?title=SingleApplication

  Modified by: Fábio Pichler

*******************************************************************************/

#pragma once

#include <QApplication>
#include <QSharedMemory>
#include <QLocalServer>

class SingleApplication : public QApplication
{
    Q_OBJECT

public:
    SingleApplication(int &argc, char **argv, const QString &uniqueKey);
    ~SingleApplication();
    bool createServer();
    bool isRunning();
    bool sendMessage(const char *message);

public slots:
    void receiveMessage();

signals:
    void messageAvailable(QStringList);

private:
    bool _isRunning;
    QString _uniqueKey;
    QSharedMemory sharedMemory;
    QLocalServer *localServer;
    QStringList allArguments;
    static const int timeout = 1000;
};
