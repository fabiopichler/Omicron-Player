/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "StreamBase.h"

#include <bass_fx.h>

int EncoderList::current = 0;

StreamBase::StreamBase()
{
    volume = 100;
}

const int &StreamBase::getVolume() const
{
    return volume;
}

double StreamBase::getPosition() const
{
    return BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE));
}

QString StreamBase::getFileType(const DWORD &ctype)
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

void StreamBase::updateFX(int index, int value)
{
    if (index == 0)
    {
        BASS_BFX_VOLUME p;
        p.lChannel = 0;
        p.fVolume = static_cast<float>(pow(10.0, static_cast<double>(value) / 20.0));
        BASS_FXSetParameters(volfx, &p);
    }
    else if (index < 16)
    {
        BASS_BFX_PEAKEQ eq;
        eq.lBand = index;
        BASS_FXGetParameters(fxEQ, &eq);
        eq.fGain = static_cast<float>(value);
        BASS_FXSetParameters(fxEQ, &eq);
    }
}

void StreamBase::setVolume(int volume)
{
    this->volume = volume;
    BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, static_cast<float>(volume) / 100.0f);
}

void StreamBase::setPosition(int arg)
{
    BASS_ChannelSetPosition(stream, BASS_ChannelSeconds2Bytes(stream, arg), BASS_POS_BYTE);
}

//================================================================================================================
// protected
//================================================================================================================

void StreamBase::setupDSP_EQ()
{
    BASS_BFX_PEAKEQ eq;
    int i;
    float allBands[] = {50.0f, 75.0f, 100.0f, 156.0f, 220.0f, 440.0f, 620.0f, 880.0f,
                        1000.0f, 3000.0f, 5000.0f, 7000.0f, 10000.0f, 13000.0f, 16000.0f};

    fxEQ = BASS_ChannelSetFX(stream, BASS_FX_BFX_PEAKEQ, 0);
    volfx = BASS_ChannelSetFX(stream, BASS_FX_BFX_VOLUME, 0);

    eq.fGain = 0.0f;
    eq.fQ = 0.0f;
    eq.fBandwidth = 1.5f;
    eq.lChannel = BASS_BFX_CHANALL;

    for (i = 1; i < 16; i++)
    {
        eq.lBand = i;
        eq.fCenter = allBands[i-1];
        BASS_FXSetParameters(fxEQ, &eq);
    }

    for (i = 0; i < 16; i++)
        updateFX(i, Global::equalizerValues[i]);

    setVolume(volume);
}
