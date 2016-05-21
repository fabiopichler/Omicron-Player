/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "StreamBase.h"

#include <bass_fx.h>

StreamBase::StreamBase()
{
    volume = 100;
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
