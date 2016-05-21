/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/Global.h"
#include "../Core/Database.h"
#include "../Gui/Widget.h"

#include <QWidget>

//================================================================================================================
// GeneralPage
//================================================================================================================
class GeneralPage : public Widget
{
    Q_OBJECT

public:
    GeneralPage(QWidget *);

private slots:
    void changeError(int);

private:
    ComboBox *errorCombo;
};

//================================================================================================================
// InterfacePage
//================================================================================================================
class InterfacePage : public Widget
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

private:
    ComboBox *themeCombo, *styleCombo;
};

//================================================================================================================
// MusicPage
//================================================================================================================
class MusicPage : public Widget
{
    Q_OBJECT

public:
    MusicPage(QWidget *);

private:

};

//================================================================================================================
// WebRadioPage
//================================================================================================================
class WebRadioPage : public Widget
{
    Q_OBJECT

public:
    WebRadioPage(QWidget *);

public slots:
    void readtimeoutChanged();
    void timeoutChanged();
    void modeChanged();

private:
    ComboBox *readtimeoutCombo, *timeoutCombo, *modeCombo;
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

class FileTypePage : public Widget
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
class AudioPage : public Widget
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
class RecordingsPage : public Widget
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
class NetworkPage : public Widget
{
    Q_OBJECT

public:
    NetworkPage(QWidget *);

private:

};

//================================================================================================================
// UpdatePage
//================================================================================================================
class UpdatePage : public Widget
{
    Q_OBJECT

public:
    UpdatePage(QWidget *);

public slots:
    void changeUpdade(int);

private:
};

