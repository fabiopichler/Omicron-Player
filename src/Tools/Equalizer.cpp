/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "Equalizer.h"

Equalizer::Equalizer(QWidget *parent) : DialogBase(parent)
{
    oldValues = Global::equalizerValues;
    currentIndex = Database::value("Current", "EqualizerPreset").toInt();
    isNewPreset = false;
    lastId = false;
    presetComboUpdate = false;

    createLabels();
    createButtons();
    createSlider();
    createBoxLayout();
    createEvents();
    setWindowTitle("Equalizador » "+AppName);
}

Equalizer::~Equalizer()
{
}

//================================================================================================================
// private
//================================================================================================================

void Equalizer::createLabels()
{
    QStringList list;
    list << "Pré Amplificador" << "50Hz" << "75Hz" << "100Hz" << "156Hz" << "220Hz" << "440Hz"
         << "620Hz" << "880Hz" << "1KHz" << "3KHz" << "5KHz" << "7KHz" << "10KHz" << "13KHz" << "16KHz";

    for (int i = 0; i < 16; i++)
    {
        eqFr[i] = new QLabel(list[i]);
        eqDb[i] = new QLabel(QString("%1dB").arg(Global::equalizerValues[i]));

        eqFr[i]->setObjectName("equalizerLabel");
        eqDb[i]->setObjectName("equalizerLabel");
        eqDb[i]->setMinimumWidth(32);
    }

    presetCombo = new ComboBox;
    presetCombo->setObjectName("presetCombo");

    loadPresetIndex();
}

void Equalizer::createButtons()
{
    okButton = new QPushButton("Ok");
    cancelButton = new QPushButton("Cancelar");
    defaultButton = new QPushButton("Redefinir Padrão");

    newButton = new QPushButton("Novo");
    editButton = new QPushButton("Renomear");
    saveButton = new QPushButton("Salvar");
    deleteButton = new QPushButton("Deletar");
}

void Equalizer::createSlider()
{
    eq[0] = new QSlider(Qt::Horizontal);
    eq[0]->setFixedWidth(260);
    eq[0]->setRange(-35,35);
    eq[0]->setSliderPosition(Global::equalizerValues[0]);

    for (int i = 1; i < 16; i++)
    {
        eq[i] = new QSlider(Qt::Vertical);
        eq[i]->setRange(-15,15);
        eq[i]->setSliderPosition(Global::equalizerValues[i]);
    }
}

void Equalizer::createBoxLayout()
{
    QVBoxLayout *volLayout = new QVBoxLayout;
    volLayout->addWidget(eqFr[0]);
    volLayout->addWidget(eq[0]);
    volLayout->addWidget(eqDb[0]);

    QGroupBox *volGroup = new QGroupBox("Ajustes");
    volGroup->setLayout(volLayout);

    QHBoxLayout *presetHLayout = new QHBoxLayout;
    presetHLayout->addWidget(newButton);
    presetHLayout->addWidget(saveButton);
    presetHLayout->addWidget(deleteButton);
    presetHLayout->addWidget(editButton);
    presetHLayout->setSpacing(8);

    QVBoxLayout *presetLayout = new QVBoxLayout;
    presetLayout->addWidget(presetCombo, 1, Qt::AlignHCenter);
    presetLayout->addLayout(presetHLayout);

    QGroupBox *presetGroup = new QGroupBox("Preset");
    presetGroup->setLayout(presetLayout);

    QHBoxLayout *optionsLayout = new QHBoxLayout;
    optionsLayout->addWidget(volGroup);
    optionsLayout->addWidget(presetGroup,1);

    QHBoxLayout *eqLayout = new QHBoxLayout;
    eqLayout->setSpacing(15);

    QVBoxLayout *vLayout[15];

    for (int i = 0; i < 15; i++)
    {
        vLayout[i] = new QVBoxLayout;
        vLayout[i]->addWidget(eqFr[i+1]);
        vLayout[i]->addWidget(eq[i+1]);
        vLayout[i]->addWidget(eqDb[i+1]);
        vLayout[i]->setSpacing(5);

        eqLayout->addLayout(vLayout[i]);
    }

    QGroupBox *eqGroup = new QGroupBox("Equalizador");
    eqGroup->setLayout(eqLayout);

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addWidget(defaultButton);
    buttons->addStretch(1);
    buttons->addWidget(okButton);
    buttons->addWidget(cancelButton);

    QVBoxLayout *layoutMain = new QVBoxLayout;
    layoutMain->addWidget(eqGroup);
    layoutMain->addLayout(optionsLayout);
    layoutMain->addLayout(buttons);
    layoutMain->setContentsMargins(8, 6, 8, 8);
    layoutMain->setSpacing(8);

    setLayout(layoutMain);
}

void Equalizer::createEvents()
{
    connect(defaultButton, SIGNAL(clicked()), this, SLOT(defaultEqualizer()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    connect(newButton, SIGNAL(clicked()), this, SLOT(newPreset()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(editPreset()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(savePreset()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(deletePreset()));

    connect(presetCombo, SIGNAL(activated(int)), this, SLOT(loadPreset()));
    connect(presetCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged()));

    for (int i = 0; i < 16; i++)
    {
        connect(eq[i], &QSlider::valueChanged, [this,i](int arg) {
            equalizerChanged(i, arg);
            eqDb[i]->setText(QString("%1dB").arg(arg));
        });
    }
}

void Equalizer::loadPresetIndex()
{
    QList<int> ids;
    QStringList titles;
    int eqLen = Database::getEqualizerPresets(ids, titles);

    presetCombo->setEditable(false);

    if (eqLen > 0 && eqLen == ids.length() && eqLen == titles.length())
    {
        lastId = ids[eqLen-1];

        if (presetCombo->count() > 0)
            currentIndex = presetCombo->currentIndex();

        presetCombo->clear();

        for (int i = 0; i < eqLen; i++)
            presetCombo->addItem(titles[i], ids[i]);

        if (presetCombo->count() > 0)
            presetCombo->setCurrentIndex(currentIndex);
    }
    else
    {
        presetCombo->clear();
    }
}

//================================================================================================================
// private slots
//================================================================================================================

void Equalizer::ok()
{
    QDialog::close();
    QString values;

    for (int i = 0; i < 16; i++)
        values += QString("(%1, %2), ").arg(i).arg(Global::equalizerValues[i]);

    values.remove(QRegExp(", $"));

    Database::setValue("Current", "EqualizerPreset", presetCombo->currentIndex());
    Database::setEqualizerValues(values);
}

void Equalizer::close()
{
    Global::equalizerValues = oldValues;

    for (int i = 0; i < 16; i++)
    {
        emit updateEqualizer(i, oldValues[i]);
    }

    QDialog::close();
}

void Equalizer::defaultEqualizer()
{
    for (int i = 0; i < 16; i++)
    {
        eq[i]->setValue(0);
        Global::equalizerValues[i] = 0;
        emit updateEqualizer(i,0);
    }
}

void Equalizer::indexChanged()
{
    if (isNewPreset)
    {
        isNewPreset = false;
        presetCombo->removeItem(presetCombo->count()-1);
    }
}

void Equalizer::equalizerChanged(int id, int value)
{
    Global::equalizerValues[id] = value;
    emit updateEqualizer(id,value);
}

void Equalizer::newPreset()
{
    if (!isNewPreset)
    {
        presetCombo->addItem("", lastId);
        presetCombo->setCurrentIndex(presetCombo->count()-1);
        presetCombo->setEditable(true);
        presetCombo->setFocus();
        isNewPreset = true;
    }
}

void Equalizer::editPreset()
{
    if (!presetComboUpdate && presetCombo->count() > 0)
    {
        presetCombo->setEditable(true);
        presetCombo->setFocus();
        editButton->setText("Cancelar");
        presetComboUpdate = true;
    }
    else
    {
        presetCombo->setEditable(false);
        editButton->setText("Renomear");
        presetComboUpdate = false;
    }

    presetCombo->update();
}

void Equalizer::savePreset()
{
    if (presetCombo->count() < 1)
        return;

    if (presetCombo->currentText().isEmpty())
        presetCombo->setCurrentText("New Preset");

    if (isNewPreset)
    {
        isNewPreset = false;
        if (!Database::addEqualizerPreset(presetCombo->currentText(), Global::equalizerValues))
            presetCombo->removeItem(presetCombo->count()-1);
    }
    else
    {
        Database::setEqualizerPreset(presetCombo->currentData().toInt(), presetCombo->currentText(), Global::equalizerValues);
    }

    loadPresetIndex();
}

void Equalizer::deletePreset()
{
    if (presetCombo->count() > 0 && !isNewPreset)
    {
        if (QMessageBox::question(this, "Apagar", "Confirmar a exclusão de: " + presetCombo->currentText() + "?",
                                  QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
        {
            Database::remove("EqualizerPreset", "id", presetCombo->currentData().toString());
            loadPresetIndex();
        }
    }
}

void Equalizer::loadPreset()
{
    if (!presetCombo->currentText().isEmpty() && presetCombo->currentIndex() != currentIndex)
        presetCombo->setEditable(false);

    QList<int> list = Database::getEqualizerPreset(presetCombo->currentData().toInt());

    if (list.length() < 16)
        return;

    for (int i = 0; i < 16; i++)
    {
        eq[i]->setValue(list[i]);
        Global::equalizerValues[i] = list[i];
        emit updateEqualizer(i,list[i]);
    }

    presetCombo->setEditable(false);
    editButton->setText("Renomear");
    presetComboUpdate = false;
    presetCombo->update();
}
