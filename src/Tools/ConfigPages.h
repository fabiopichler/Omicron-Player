/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/Global.h"
#include "../Core/Database.h"
#include "../Gui/Widgets/MyWidget.h"
#include "../Gui/Widgets/MyComboBox.h"

#include <QWidget>
#include <QGroupBox>

//================================================================================================================
// GeneralPage
//================================================================================================================
class GeneralPage : public MyWidget
{
    Q_OBJECT

public:
    GeneralPage(QWidget *);

private slots:
    void changeError(int);

private:
    MyComboBox *errorCombo;
};

//================================================================================================================
// InterfacePage
//================================================================================================================
class InterfacePage : public MyWidget
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
    MyComboBox *themeCombo, *styleCombo;
    int currentTheme;
};

//================================================================================================================
// MusicPage
//================================================================================================================
class MusicPage : public MyWidget
{
    Q_OBJECT

public:
    MusicPage(QWidget *);

private:

};

//================================================================================================================
// WebRadioPage
//================================================================================================================
class WebRadioPage : public MyWidget
{
    Q_OBJECT

public:
    WebRadioPage(QWidget *);

public slots:
    void readtimeoutChanged();
    void timeoutChanged();
    void modeChanged();

private:
    MyComboBox *readtimeoutCombo, *timeoutCombo, *modeCombo;
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

class FileTypePage : public MyWidget
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
class AudioPage : public MyWidget
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
class RecordingsPage : public MyWidget
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
class NetworkPage : public MyWidget
{
    Q_OBJECT

public:
    NetworkPage(QWidget *);

private:

};

//================================================================================================================
// UpdatePage
//================================================================================================================
class UpdatePage : public MyWidget
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

