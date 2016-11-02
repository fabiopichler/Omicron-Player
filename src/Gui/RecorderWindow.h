/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/RecorderStream.h"
#include "../Core/Global.h"
#include "../Tools/Equalizer.h"
#include "Widgets/MySlider.h"
#include "ErrorWindow.h"

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

class RecorderWindow : public MyWidget
{
    Q_OBJECT

public:
    RecorderWindow(QObject *, QWidget *);
    ~RecorderWindow();

private:
    void createMenuBar();
    void createWidgets();
    void createComboBox();
    void createLabels();
    void createButtons();
    void createLayouts();
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
    QWidget *uiWidget;
    RecorderStream *recorderStream;
    RecordList *recordList;
    QAction *displayRecordingsAction,*exitAction,*radioModeAction,*serverModeAction,*musicModeAction,
        *equalizerAction,*checkUpdateAction,*websiteAction, *aboutAction, *configAction;
    ErrorWindow *errorWindow;
    QProgressBar *leftChProgressBar, *rightChProgressBar;
    QLabel *timeLabel, *statusLabel;
    QPushButton *recordButton, *stopButton, *playButton, *deleteButton;
    QComboBox *deviceComboBox, *encoderComboBox, *bitrateComboBox;
    QSlider *timeSlider;
};
