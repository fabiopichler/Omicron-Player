/*******************************************************************************
  Omicron Player Classic

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/Global.h"
#include "../Core/Database.h"

#include <OmicronTK11/Qt/Widget.hpp>
#include <OmicronTK11/Qt/ComboBox.hpp>

#include <QWidget>
#include <QGroupBox>

//================================================================================================================
// GeneralPage
//================================================================================================================
class GeneralPage : public OmicronTK11::OTKQT::Widget
{
    Q_OBJECT

public:
    GeneralPage(QWidget *);

private slots:
    void changeError(int);

private:
    OmicronTK11::OTKQT::ComboBox *errorCombo;
};

//================================================================================================================
// InterfacePage
//================================================================================================================
class InterfacePage : public OmicronTK11::OTKQT::Widget
{
    Q_OBJECT

public:
    InterfacePage(QWidget *);

private slots:
    void changeTheme(int);
    void changeStyle(int);

private:
    void updateThemeList();
    void updateStyleList();

signals:
    void restartApp();

private:
    OmicronTK11::OTKQT::ComboBox *themeCombo, *styleCombo;
    int currentTheme;
};

//================================================================================================================
// MusicPage
//================================================================================================================
class MusicPage : public OmicronTK11::OTKQT::Widget
{
    Q_OBJECT

public:
    MusicPage(QWidget *);

private:

};

//================================================================================================================
// WebRadioPage
//================================================================================================================
class WebRadioPage : public OmicronTK11::OTKQT::Widget
{
    Q_OBJECT

public:
    WebRadioPage(QWidget *);

public slots:
    void readtimeoutChanged();
    void timeoutChanged();
    void modeChanged();

private:
    OmicronTK11::OTKQT::ComboBox *readtimeoutCombo, *timeoutCombo, *modeCombo;
};

//================================================================================================================
// FileTypePage
//================================================================================================================
#ifdef Q_OS_WIN

#include "../Core/Windows/FileAssociation.h"
#include <QTreeView>
#include <QStandardItemModel>

class FileTypeTree : public QTreeView
{
    Q_OBJECT

public:
    FileTypeTree(QWidget *);
    ~FileTypeTree();
    void addRow(const QStringList &files, bool checked);
    QString getChecked();
    void clear();

public slots:
    void selectAllRows();

private:
    QStandardItemModel* model;
    QWidget *parent;
};

class FileTypePage : public OmicronTK11::OTKQT::Widget
{
    Q_OBJECT

public:
    FileTypePage(QWidget *);
    ~FileTypePage();

private slots:
    void updateTreeView();
    void associateFiles();

private:
    FileTypeTree *fileTypeTree;
    FileAssociation *fileAssoc;
};
#endif // Q_OS_WIN

//================================================================================================================
// AudioPage
//================================================================================================================
class AudioPage : public OmicronTK11::OTKQT::Widget
{
    Q_OBJECT

public:
    AudioPage(QObject *, QWidget *);

private slots:
    void changeDevice(int);

signals:
    void stopStream();

private:
    QObject *parentMain;
};

//================================================================================================================
// RecordingsPage
//================================================================================================================
class RecordingsPage : public OmicronTK11::OTKQT::Widget
{
    Q_OBJECT

public:
    RecordingsPage(QWidget *);

private slots:
    void searchDir();

private:
    QPushButton *recButton;
    QLineEdit *pathEdit;
};

//================================================================================================================
// NetworkPage
//================================================================================================================
class NetworkPage : public OmicronTK11::OTKQT::Widget
{
    Q_OBJECT

public:
    NetworkPage(QWidget *);

private:

};

//================================================================================================================
// UpdatePage
//================================================================================================================
class UpdatePage : public OmicronTK11::OTKQT::Widget
{
    Q_OBJECT

public:
    UpdatePage(QWidget *);

public slots:
    void changeUpdade(int);

private:
};

//================================================================================================================
// FadeConfigBox
//================================================================================================================

class FadeConfigBox : public QGroupBox
{
public:
    FadeConfigBox(const QString &, const QString &, QWidget *);

private:
    const QString configName;
};

