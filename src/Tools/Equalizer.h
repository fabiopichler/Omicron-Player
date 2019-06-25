#pragma once

#include "../Core/Global.h"
#include <OmicronTK11/Qt/Equalizer.hpp>

class Equalizer : public OmicronTK11::OTKQT::Equalizer
{
public:
    Equalizer(QWidget *, std::vector<int> &values);

    static void loadValues(std::vector<int> &val);

    int getCurrentPresetIndex() override;
    void setCurrentPresetIndex(int) override;
    void removeById(const std::string &) override;

    bool addEqualizerPreset(const std::string &, const std::vector<int> &) override;
    bool setEqualizerValues(const std::string &) override;
    bool setEqualizerPreset(int, const std::string &, const std::vector<int> &) override;

    std::vector<int> getEqualizerPreset(int) override;
    int getEqualizerPresets(std::vector<int> &, std::vector<std::string> &) override;
};
