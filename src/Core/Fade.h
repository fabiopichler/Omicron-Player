
#pragma once

#include <QThread>
#include <QMutex>
#include <bass.h>

class Fade : public QThread
{
    Q_OBJECT

public:
    Fade(const QString &, const bool &);
    ~Fade();

    void in(const HSTREAM &, const int &);
    void out(const HSTREAM &);

private:
    void streamFree(const HSTREAM &);
    void run();

    const QString configName;
    QMutex mutex;
    HSTREAM oldStream;
    long waitTimeout;
    const bool &mstop;
};
