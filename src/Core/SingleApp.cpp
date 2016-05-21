/*******************************************************************************
  Author at: http://wiki.qtcentre.org/index.php?title=SingleApplication

  Modified by: Fábio Pichler

*******************************************************************************/

#include <QLocalSocket>
#include "SingleApp.h"
#include "Global.h"

SingleApplication::SingleApplication(int &argc, char **argv, const QString &uniqueKey) : QApplication(argc, argv), _uniqueKey(uniqueKey)
{
    localServer = nullptr;
    sharedMemory.setKey(_uniqueKey);

    if (sharedMemory.attach())
    {
        _isRunning = true;
    }
    else
    {
        _isRunning = false;

        if (!sharedMemory.create(1))
        {
            qDebug("Unable to create single instance.");
            return;
        }

        createServer();
    }
}

SingleApplication::~SingleApplication()
{
    delete localServer;
}

bool SingleApplication::createServer()
{
    QLocalServer::removeServer(_uniqueKey);

    if ((localServer = new QLocalServer(this)))
    {
        connect(localServer, SIGNAL(newConnection()), this, SLOT(receiveMessage()));

        if (localServer->listen(_uniqueKey))
            return true;
    }

    return false;
}

bool SingleApplication::isRunning()
{
    return _isRunning;
}

bool SingleApplication::sendMessage(const char *message)
{
    if (!_isRunning)
        return false;

    QLocalSocket localSocket(this);
    localSocket.connectToServer(_uniqueKey, QIODevice::WriteOnly);

    if (!localSocket.waitForConnected(timeout))
    {
        qDebug() << localSocket.errorString();
        return false;
    }

    localSocket.write(message);

    if (!localSocket.waitForBytesWritten(timeout))
    {
        qDebug() << localSocket.errorString();
        return false;
    }

    localSocket.disconnectFromServer();
    return true;
}

//================================================================================================================
// public slots
//================================================================================================================

void SingleApplication::receiveMessage()
{
    QLocalSocket *localSocket = localServer->nextPendingConnection();

    if (!localSocket->waitForReadyRead(timeout))
    {
        qDebug() << localSocket->errorString();
        return;
    }

    QString message = Global::cStrToQString(localSocket->readAll());

    message.replace("\\", "/");

    if (message.contains("--open-file:"))
    {
        QStringList list;
        list << "--open-file" << message.remove("--open-file:");
        emit messageAvailable(list);
    }
    else if (message.contains("--add-file:"))
    {
        QStringList list;
        list << "--add-file" << message.remove("--add-file:");
        emit messageAvailable(list);
    }
    else if (message == "--end-arguments")
    {
        emit messageAvailable(allArguments);
        allArguments.clear();
    }
    else
    {
        allArguments << message;
    }

    localSocket->disconnectFromServer();
}
