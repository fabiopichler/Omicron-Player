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

#include "Stream.h"
#include "Database.h"

#include <OmicronTK/Qt/AppInfo.hpp>

#include <bass_fx.h>

using namespace OmicronTK;

int EncoderList::current = 0;

Stream::Stream()
{
    volume = 100;
    agcFX = 0;
}

bool Stream::init()
{
    if (HIWORD(BASS_GetVersion()) != BASSVERSION)
    {
        QMessageBox::critical(nullptr,"Incorrect BASS.DLL",
                             "An incorrect version of BASS.DLL was loaded (2.4 is required)");
        return false;
    }

    if (HIWORD(BASS_FX_GetVersion()) != BASSVERSION)
    {
        QMessageBox::critical(nullptr,"Incorrect BASS_FX.DLL",
                             "An incorrect version of BASS_FX.DLL was loaded (2.4 is required)");
        return false;
    }

    int device = Database::value("Config", "device").toInt();
    BASS_DEVICEINFO deviceInfo;
    bool deviceOk = false;

    for (int i = 1; BASS_GetDeviceInfo(i,&deviceInfo); i++)
    {
        if (deviceInfo.flags&BASS_DEVICE_ENABLED)
        {
            if (device == i)
            {
                deviceOk = true;
                break;
            }
        }
    }

    if (!BASS_Init((deviceOk ? device : -1), 44100, 0, 0, nullptr))
    {
        Database::setValue("Config", "device", -1);
        if (!BASS_Init(-1, 44100, 0, 0, nullptr))
        {
            QMessageBox::critical(nullptr,"Erro",Global::getErrorHtml("Não foi possível iniciar o programa!<br>"
                                             "Verifique se seu dispositivo de áudio está funcionando corretamente"
                                                                        " e tente novamente."));
            return false;
        }
    }

    QString path = OTKQT::AppInfo::pluginsPath() + "/audio/";

#ifndef Q_OS_ANDROID // Desativar os plugins no Android, apenas para os testes (ainda não funciona nele).
    if (!BASS_PluginLoad(QString(path+bass_aac).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+bass_aac));

    if (!BASS_PluginLoad(QString(path+bass_ac3).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+bass_ac3));

    if (!BASS_PluginLoad(QString(path+bass_ape).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+bass_ape));

    if (!BASS_PluginLoad(QString(path+bassflac).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+bassflac));

    if (!BASS_PluginLoad(QString(path+basswv).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+basswv));

    if (!BASS_PluginLoad(QString(path+bassopus).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+bassopus));
#endif
#ifdef Q_OS_WIN
    if (!BASS_PluginLoad(QString(path+basswma).toLocal8Bit().constData(),0))
        QMessageBox::warning(nullptr,"Erro",Global::getErrorHtml("Houve um erro com o arquivo: "+basswma));
#endif // Q_OS_WIN

    QString proxy = Database::value("RadioConfig", "net_proxy", "0").toString();

    BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1);
    BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0);
    BASS_SetConfig(BASS_CONFIG_NET_READTIMEOUT, Database::value("RadioConfig", "net_readtimeout", 20000).toInt());
    BASS_SetConfig(BASS_CONFIG_NET_TIMEOUT, Database::value("RadioConfig", "net_timeout", 20000).toInt());
    BASS_SetConfig(BASS_CONFIG_FLOATDSP, TRUE);
    BASS_SetConfigPtr(BASS_CONFIG_NET_PROXY, (proxy == "0" ? NULL : proxy.toLocal8Bit().data()));
    BASS_SetConfig(BASS_CONFIG_CD_AUTOSPEED, TRUE);

    return true;
}

void Stream::free()
{
    BASS_Stop();
    BASS_PluginFree(0);
    BASS_Free();
}

const int &Stream::getVolume() const
{
    return volume;
}

double Stream::getPosition() const
{
    return BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE));
}

QString Stream::getFileType(const DWORD &ctype)
{
    switch (ctype)
    {
        case BASS_CTYPE_STREAM_OGG:       return "OGG";
        case BASS_CTYPE_STREAM_MP1:       return "MP1";
        case BASS_CTYPE_STREAM_MP2:       return "MP2";
        case BASS_CTYPE_STREAM_MP3:       return "MP3";
        case BASS_CTYPE_STREAM_AIFF:      return "AIFF";
        case BASS_CTYPE_STREAM_WAV_PCM:   return "PCM WAVE";
        case BASS_CTYPE_STREAM_WAV_FLOAT: return "PCM WAVE";
        case BASS_CTYPE_STREAM_WAV:       return "WAVE";
        case 327682:                      return "ADPCM WAVE";
        case 70144:                       return "Opus";
        case 68352:                       return "AAC";
        case 66304:                       return "WMA";
        case BASS_CTYPE_STREAM_CA:        return "CA";
        case BASS_CTYPE_STREAM_MF:        return "MF";
        case BASS_CTYPE_MUSIC_MOD:        return "MOD";
        case BASS_CTYPE_MUSIC_MTM:        return "MTM";
        case BASS_CTYPE_MUSIC_S3M:        return "S3M";
        case BASS_CTYPE_MUSIC_XM:         return "XM";
        case BASS_CTYPE_MUSIC_IT:         return "IT";
        case BASS_CTYPE_MUSIC_MO3:        return "MO3";
        case 66048:                       return "CDA";
        case 67328:                       return "APE";
        case 67840:                       return "FLAC";
        case 68353:                       return "M4A";
        case 66816:                       return "WavPack";
    }

    return QString();
}

//================================================================================================================
// public slots
//================================================================================================================

void Stream::updateFX(int index, int value)
{
    if (index == 0)
    {
        BASS_BFX_VOLUME p;
        p.lChannel = 0;
        p.fVolume = static_cast<float>(pow(10.0, static_cast<double>(value) / 20.0));
        BASS_FXSetParameters(volfx, &p);
    }
    else if (index < 19)
    {
        BASS_BFX_PEAKEQ eq;
        eq.lBand = index;
        BASS_FXGetParameters(fxEQ, &eq);
        eq.fGain = static_cast<float>(value);
        BASS_FXSetParameters(fxEQ, &eq);
    }
}

void Stream::automaticGainControl(bool enable)
{
    if (enable && agcFX == 0)
    {
        BASS_BFX_DAMP damp;

        damp.fTarget = 0.94f;
        damp.fQuiet = 0.02f;
        damp.fRate = 0.02f;
        damp.fGain = 15.0f;
        damp.fDelay = 1.0f;
        damp.lChannel = BASS_BFX_CHANALL;

        agcFX = BASS_ChannelSetFX(stream, BASS_FX_BFX_DAMP, 1);
        BASS_FXSetParameters(agcFX, &damp);
    }
    else if (!enable && agcFX != 0)
    {
        BASS_ChannelRemoveFX(stream, agcFX);
        agcFX = 0;
    }
}

void Stream::setVolume(int volume)
{
    this->volume = volume;
    BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, static_cast<float>(volume) / 100.0f);
}

void Stream::setPosition(int arg)
{
    BASS_ChannelSetPosition(stream, BASS_ChannelSeconds2Bytes(stream, arg), BASS_POS_BYTE);
}

//================================================================================================================
// protected
//================================================================================================================

void Stream::finishedStream()
{
    agcFX = 0;
}

void Stream::setupDSP_EQ()
{
    BASS_BFX_PEAKEQ eq;
    int i;
    float allBands[] = {35.0f, 50.0f, 75.0f, 100.0f, 150.0f, 200.0f, 400.0f, 600.0f, 800.0f,
                        1000.0f, 2500.0f, 4000.0f, 6000.0f, 8000.0f, 10000.0f, 12000.0f, 14000.0f, 15999.9f};

    fxEQ = BASS_ChannelSetFX(stream, BASS_FX_BFX_PEAKEQ, 0);
    volfx = BASS_ChannelSetFX(stream, BASS_FX_BFX_VOLUME, 0);

    eq.fGain = 0.0f;
    eq.fQ = 0.0f;
    eq.fBandwidth = 1.5f;
    eq.lChannel = BASS_BFX_CHANALL;

    for (i = 1; i < 19; i++)
    {
        eq.lBand = i;
        eq.fCenter = allBands[i-1];
        BASS_FXSetParameters(fxEQ, &eq);
    }

    for (i = 0; i < 19; i++)
        updateFX(i, Global::equalizerValues[i]);

    setVolume(volume);
}
