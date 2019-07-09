/*******************************************************************************

  Omicron Player Classic

  Author: Fábio Pichler
  Website: http://fabiopichler.net
  License: BSD 3-Clause License

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Omicron Player Classic nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
