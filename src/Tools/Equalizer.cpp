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
