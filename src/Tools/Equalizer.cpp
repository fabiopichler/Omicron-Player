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

#include "Equalizer.h"
#include "../Core/Database.h"

using namespace OmicronTK;

Equalizer::Equalizer(QWidget *parent, std::vector<int> &values) : OTKQT::Equalizer(parent, values)
{
    setPresetIndex(getCurrentPresetIndex());
    loadPresetIndex();
}

void Equalizer::loadValues(std::vector<int> &val)
{
    val.clear();

    for (int i = 0; i < 19; i++)
        val.push_back(Database::value("CurrentEqualizer", QString::number(i)).toInt());
}

int Equalizer::getCurrentPresetIndex()
{
    return Database::value("EqualizerConfig", "EqualizerPreset").toInt();
}

void Equalizer::setCurrentPresetIndex(int v)
{
    Database::setValue("EqualizerConfig", "EqualizerPreset", v);
}

void Equalizer::removeById(const std::string &id)
{
    Database::remove("EqualizerPreset", "id", QString::fromStdString(id));
}

bool Equalizer::addEqualizerPreset(const std::string &title, const std::vector<int> &values)
{
    return Database::addEqualizerPreset(QString::fromStdString(title), values);
}

bool Equalizer::setEqualizerValues(const std::string &values)
{
    return Database::setEqualizerValues(QString::fromStdString(values));
}

bool Equalizer::setEqualizerPreset(int id, const std::string &title, const std::vector<int> &values)
{
    return Database::setEqualizerPreset(id, QString::fromStdString(title), values);
}

std::vector<int> Equalizer::getEqualizerPreset(int id)
{
    return Database::getEqualizerPreset(id);
}

int Equalizer::getEqualizerPresets(std::vector<int> &ids, std::vector<std::string> &titles)
{
    return Database::getEqualizerPresets(ids, titles);
}
