/*******************************************************************************
  Omicron Player Classic

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "RecorderWindow.h"
#include "../Core/Database.h"

#include <QDesktopServices>

#include <OmicronTK/Qt/ErrorWindow.hpp>
#include <OmicronTK/Qt/ComboBox.hpp>
#include <OmicronTK/Qt/Label.hpp>

#include <QComboBox>

using namespace OmicronTK;

RecorderWindow::RecorderWindow(QObject *parentMain)
    : WindowBase(parentMain),
      m_ui(this)
{
    this->parentMain = parentMain;
    mode = "Recorder";
    recorderStream = new RecorderStream(this);
    recordList = recorderStream->recordList;

    if (!m_ui.init(recorderStream))
    {
        throw "34: if (!m_ui.init(recordList))";
    }

    createMenuBar();
    createWidgets();
    createComboBox();
    createLabels();
    createButtons();
    createEvents();
}

RecorderWindow::~RecorderWindow()
{
    delete recorderStream;
    delete m_errorWindow;
}

bool RecorderWindow::init()
{
    return true;
}

//================================================================================================================
// private
//================================================================================================================

void RecorderWindow::createMenuBar()
{

}

void RecorderWindow::createWidgets()
{
    m_ui.m_leftChProgressBar->setObjectName("channelsProgressBar");
    m_ui.m_leftChProgressBar->setRange(0,32768);
    m_ui.m_leftChProgressBar->setTextVisible(false);

    m_ui.m_rightChProgressBar->setObjectName("channelsProgressBar");
    m_ui.m_rightChProgressBar->setRange(0,32768);
    m_ui.m_rightChProgressBar->setTextVisible(false);

    m_ui.m_timeSlider->setEnabled(false);
    m_ui.m_timeSlider->setMaximum(0);
}

void RecorderWindow::createComboBox()
{
    int current = EncoderList::current;

    m_ui.m_deviceComboBox->setFixedWidth(220);

    QStringList list = recorderStream->getDeviceInfo();
    int device = Database::value("RecorderMode", "device").toInt();

    for (int i = 0; i < list.length(); i++)
    {
        m_ui.m_deviceComboBox->addItem(list[i], i - 1);

        if ((i - 1) == device)
            m_ui.m_deviceComboBox->setCurrentIndex(i);
    }

    m_ui.m_encoderComboBox->setStyleSheet("min-width: 1px;");

    for (int i = 0; i < 3; i++)
        m_ui.m_encoderComboBox->addItem(recorderStream->encoderList[i].name);

    m_ui.m_encoderComboBox->setCurrentIndex(current);

    m_ui.m_bitrateComboBox->setStyleSheet("min-width: 1px;");

    for (int i = 0; i < recorderStream->encoderList[current].bitrate.length(); i++)
        m_ui.m_bitrateComboBox->addItem(QString("%1 kbps").arg(recorderStream->encoderList[current].bitrate[i]));

    m_ui.m_bitrateComboBox->setCurrentIndex(recorderStream->encoderList[current].index);
}

void RecorderWindow::createLabels()
{
    m_ui.m_timeLabel->setText("--:--:--");

    m_ui.m_statusLabel->setStyleSheet("background: transparent");
    m_ui.m_statusLabel->setText("Parado");
}

void RecorderWindow::createButtons()
{
    m_ui.m_recordButton->setToolTip("Gravar");
    m_ui.m_stopButton->setToolTip("Parar");
    m_ui.m_playButton->setToolTip("Reproduzir");

    m_ui.m_stopButton->setEnabled(false);

    m_ui.m_deleteButton->setText("Apagar");
    m_ui.m_deleteButton->setShortcut(QKeySequence::Delete);
}

void RecorderWindow::createEvents()
{
    connect(this, SIGNAL(showNotification(QString)), parentMain, SLOT(showNotification(QString)));
    connect(this, SIGNAL(showError(QString)), parentMain, SLOT(showError(QString)));
    connect(recorderStream, SIGNAL(showNotification(QString)), parentMain, SLOT(showNotification(QString)));
    connect(recorderStream, SIGNAL(showError(QString)), parentMain, SLOT(showError(QString)));
    connect(recorderStream, SIGNAL(showErrorDlg(QString)), this, SLOT(displayError(QString)));

    connect(m_errorWindow, SIGNAL(stopStream()), recorderStream, SLOT(stop()));
    connect(m_ui.m_timeSlider, SIGNAL(sliderMoved(int)), recorderStream, SLOT(setPosition(int)));

    connect(m_ui.m_displayRecordingsAction, SIGNAL(triggered()), this, SLOT(displayRecordings()));
    connect(m_ui.m_exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(m_ui.m_musicModeAction, SIGNAL(triggered()), parentMain, SLOT(startMusicMode()));
    connect(m_ui.m_radioModeAction, SIGNAL(triggered()), parentMain, SLOT(startRadioMode()));
    //connect(serverModeAction, SIGNAL(triggered()), parentMain, SLOT(startServerMode()));
    connect(m_ui.m_equalizerAction, SIGNAL(triggered()), this, SLOT(initEqualizer()));
    connect(m_ui.m_configAction, SIGNAL(triggered()), this, SLOT(initConfigDialog()));
    connect(m_ui.m_checkUpdateAction, SIGNAL(triggered()), parentMain, SLOT(checkUpdate()));
    connect(m_ui.m_websiteAction, SIGNAL(triggered()), this, SLOT(openSite()));
    connect(m_ui.m_facebookAction, SIGNAL(triggered()), this, SLOT(openFacebook()));
    connect(m_ui.m_aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    connect(m_ui.m_deviceComboBox, SIGNAL(activated(int)), this, SLOT(deviceChanged(int)));
    connect(m_ui.m_encoderComboBox, SIGNAL(activated(int)), this, SLOT(encoderChanged(int)));
    connect(m_ui.m_bitrateComboBox, SIGNAL(activated(int)), this, SLOT(bitrateChanged(int)));

    connect(parentMain, SIGNAL(playStream()), recorderStream, SLOT(record()));
    connect(parentMain, SIGNAL(stopStream()), recorderStream, SLOT(stop()));

    connect(m_ui.m_recordButton, SIGNAL(clicked()), recorderStream, SLOT(record()));
    connect(m_ui.m_stopButton, SIGNAL(clicked()), recorderStream, SLOT(stop()));
    connect(m_ui.m_playButton, SIGNAL(clicked()), recorderStream, SLOT(play()));
    connect(m_ui.m_deleteButton, SIGNAL(clicked()), recorderStream, SLOT(deleteFile()));

    connect(recorderStream, SIGNAL(updateValue(RecorderStream::Event, QVariant)), this, SLOT(update(RecorderStream::Event, QVariant)));
    connect(recorderStream, SIGNAL(updateInfo(QWORD, DWORD)), this, SLOT(update(QWORD, DWORD)));
    connect(recorderStream, SIGNAL(startedStream()), this, SLOT(startedStream()));
    connect(recorderStream, SIGNAL(endOfStream()), this, SLOT(endOfStream()));
}

//================================================================================================================
// private slots
//================================================================================================================

void RecorderWindow::deviceChanged(int)
{
    int device = m_ui.m_deviceComboBox->currentData().toInt();
    recorderStream->initDevice(device);
    Database::setValue("RecorderMode", "device", device);
}

void RecorderWindow::encoderChanged(int current)
{
    EncoderList::current = current;
    m_ui.m_bitrateComboBox->clear();

    for (int i = 0; i < recorderStream->encoderList[current].bitrate.length(); i++)
        m_ui.m_bitrateComboBox->addItem(QString("%1 kbps").arg(recorderStream->encoderList[current].bitrate[i]));

    m_ui.m_bitrateComboBox->setCurrentIndex(recorderStream->encoderList[current].index);
    Database::setValue("RecorderMode", "encoder", current);
}

void RecorderWindow::bitrateChanged(int index)
{
    recorderStream->encoderList[EncoderList::current].index = index;

    switch (EncoderList::current)
    {
    case 0:
        Database::setValue("RecorderMode", "aac", index);
        break;

    case 1:
        Database::setValue("RecorderMode", "ogg", index);
        break;

    case 2:
        Database::setValue("RecorderMode", "mp3", index);
        break;
    }
}

void RecorderWindow::initEqualizer()
{
    Equalizer eq(this, Global::equalizerValues);
    connect(&eq, SIGNAL(updateEqualizer(int,int)), recorderStream, SLOT(updateFX(int,int)));
    eq.exec();
    disconnect(&eq, SIGNAL(updateEqualizer(int,int)), recorderStream, SLOT(updateFX(int,int)));
}

void RecorderWindow::update(RecorderStream::Event index, QVariant value)
{
    switch (index)
    {
    case RecorderStream::Status:
        m_ui.m_statusLabel->setText(value.toString());
        break;

    case RecorderStream::TimeSlider:
        m_ui.m_timeSlider->setEnabled(true);
        m_ui.m_timeSlider->setMaximum(value.toInt());
        break;

    case RecorderStream::Recording:
        m_ui.m_statusLabel->setProperty("recording", value.toBool());
        OTKQT::Widget::updateStyle(m_ui.m_statusLabel);

        m_ui.m_timeLabel->setProperty("recording", value.toBool());
        OTKQT::Widget::updateStyle(m_ui.m_timeLabel);
        break;
    }
}

void RecorderWindow::update(QWORD time, DWORD level)
{
    m_ui.m_timeLabel->setText(QString().sprintf("%02i:%02i:%02i",
                                         static_cast<unsigned int>(time/3600),
                                         static_cast<unsigned int>((time/60)%60),
                                         static_cast<unsigned int>(time%60)));

    m_ui.m_timeSlider->setValue(time);
    m_ui.m_leftChProgressBar->setValue(LOWORD(level));
    m_ui.m_rightChProgressBar->setValue(HIWORD(level));
}

void RecorderWindow::startedStream()
{
    m_ui.m_recordButton->setEnabled(false);
    m_ui.m_stopButton->setEnabled(true);
    m_ui.m_playButton->setEnabled(false);
    m_ui.m_deviceComboBox->setEnabled(false);
    m_ui.m_encoderComboBox->setEnabled(false);
    m_ui.m_bitrateComboBox->setEnabled(false);
}

void RecorderWindow::endOfStream()
{
    m_ui.m_timeLabel->setText("--:--:--");
    m_ui.m_statusLabel->setText("Parado");
    m_ui.m_leftChProgressBar->setValue(0);
    m_ui.m_rightChProgressBar->setValue(0);
    m_ui.m_timeSlider->setMaximum(0);
    m_ui.m_timeSlider->setEnabled(false);
    m_ui.m_recordButton->setEnabled(true);
    m_ui.m_stopButton->setEnabled(false);
    m_ui.m_playButton->setEnabled(true);
    m_ui.m_deviceComboBox->setEnabled(true);
    m_ui.m_encoderComboBox->setEnabled(true);
    m_ui.m_bitrateComboBox->setEnabled(true);
}

void RecorderWindow::displayRecordings()
{
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(Database::value("RadioConfig", "recordPath").toString() + "/Recorder")))
        Warning(this, "Não foi possível localizar a pasta com as gravações.");
}

void RecorderWindow::displayError(QString arg)
{
    m_errorWindow->addError(arg);
    m_errorWindow->show();
}
