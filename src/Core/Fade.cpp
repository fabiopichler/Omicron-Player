/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "Fade.h"
#include "Database.h"

Fade::Fade(const QString &name) : configName(name)
{
    oldStream = 0;
    waitTimeout = 0;

    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

Fade::~Fade()
{
    //qDebug("Fade::~Fade()");
}

void Fade::in(const HSTREAM &stream, const int &volume)
{
    int fadeIn = Database::value(configName, "fadeIn", 0).toInt();

    if (fadeIn > 0)
    {
        BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, 0.0f);
        BASS_ChannelPlay(stream, 0);
        BASS_ChannelSlideAttribute(stream, BASS_ATTRIB_VOL, static_cast<float>(volume) / 100.0f, (fadeIn * 1000));
    }
    else
    {
        BASS_ChannelPlay(stream, 0);
    }
}

void Fade::out(const HSTREAM &old)
{
    int time = Database::value(configName, "fadeOut", 0).toInt();

    if (time > 0)
    {
        waitTimeout = time * 1000;
        oldStream = old;
        start();
    }
    else
    {
        streamFree(old);
        deleteLater();
    }
}

//================================================================================================================
// private
//================================================================================================================

void Fade::streamFree(const HSTREAM &stream)
{
    BASS_MusicFree(stream);
    BASS_StreamFree(stream);
}

void Fade::run()
{
    const HSTREAM stream = oldStream;
    oldStream = 0;

    if (waitTimeout > 0)
    {
        BASS_ChannelSlideAttribute(stream, BASS_ATTRIB_VOL, 0, waitTimeout);

        while (BASS_ChannelIsSliding(stream, 0))
            msleep(1);
    }

    streamFree(stream);
    waitTimeout = 0;
}
