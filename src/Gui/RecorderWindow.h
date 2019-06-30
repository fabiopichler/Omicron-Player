/*******************************************************************************
  Omicron Player Classic

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/RecorderStream.h"
#include "../Core/Global.h"
#include "../Tools/Equalizer.h"

#include <OmicronTK/Qt/Slider.hpp>
#include <OmicronTK/Qt/Widget.hpp>

#include "WindowBase.h"
#include "RecorderUi.hpp"

#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QLineEdit>
#include <QApplication>
#include <QFile>
#include <QtCore>
#include <qfiledialog.h>
#include <QMenuBar>
#include <QProgressBar>

class QComboBox;

class RecorderWindow : public WindowBase
{
    Q_OBJECT

public:
    RecorderWindow(QObject *);
    ~RecorderWindow();

    bool init();

private:
    void createMenuBar();
    void createWidgets();
    void createComboBox();
    void createLabels();
    void createButtons();
    void createEvents();

private slots:
    void deviceChanged(int);
    void encoderChanged(int);
    void bitrateChanged(int);
    void initEqualizer();
    void update(RecorderStream::Event, QVariant = QVariant());
    void update(QWORD, DWORD);
    void startedStream();
    void endOfStream();
    void displayRecordings();
    void displayError(QString);

signals:
    void showError(QString);
    void showNotification(QString);

private:
    QObject *parentMain;
    RecorderStream *recorderStream;
    RecordList *recordList;
    RecorderUi m_ui;
};
