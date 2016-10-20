/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/Database.h"
#include "../Gui/DialogBase.h"
#include "../Core/Global.h"
#include "../Gui/Widgets/MyComboBox.h"

#include <QDebug>
#include <QPushButton>
#include <QSlider>
#include <QGroupBox>

class Equalizer : public DialogBase
{
    Q_OBJECT

public:
    Equalizer(QWidget *);
    ~Equalizer();

private:
    void createLabels();
    void createButtons();
    void createSlider();
    void createBoxLayout();
    void createEvents();
    void loadPresetIndex();

private slots:
    void ok();
    void close() Q_DECL_OVERRIDE;
    void defaultEqualizer();
    void indexChanged();
    void equalizerChanged(int,int);
    void newPreset();
    void editPreset();
    void savePreset();
    void deletePreset();
    void loadPreset();

signals:
    void updateEqualizer(int, int);

private:
    QPushButton *okButton, *cancelButton, *defaultButton, *saveButton, *newButton, *deleteButton, *editButton;
    QLabel *eqFr[16];
    QLabel *eqDb[16];
    QSlider *eq[16];
    QList<int> oldValues;
    MyComboBox *presetCombo;
    bool isNewPreset, presetComboUpdate;
    int lastId, currentIndex;
};
