/*******************************************************************************
  Omicron Player Classic

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/Database.h"

#include <OmicronTK/Qt/DropArea.hpp>
#include <OmicronTK/Qt/Dialog.hpp>

#include <qdialog.h>
#include <QTreeView>
#include <QStandardItem>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QMenu>
#include <QFileDialog>
#include <QInputDialog>

class MusicPlaylistTreeView;
class MusicPlaylistManager : public OmicronTK::OTKQT::Dialog
{
    Q_OBJECT

public:
    MusicPlaylistManager(QWidget *);
    ~MusicPlaylistManager();
    void loadPlaylist();

public slots:
    void closePlaylist();
    void updatePlaylistStyle(bool arg = false);

private:
    void createWidgets();
    void createLabels();
    void createButtons();
    void createBoxLayout();
    void createEvents();
    bool eventFilter(QObject * obj, QEvent *event) Q_DECL_OVERRIDE;

private slots:
    void ok();
    void addFiles();
    void addDirectory();
    bool newPlaylist(bool close = true);
    void openPlaylist();
    void savePlaylist(int arg = 0);
    void savePlaylistAs();
    void closeDefaultPlaylist();
    void shuffle();
    void receiveMessage(QVector<QString>);

public:
    MusicPlaylistTreeView *playlist;
    bool playlistChanged;

private:
    QWidget *parent;
    OmicronTK::OTKQT::Widget *topWidget, *selectPlWidget;
    OmicronTK::OTKQT::DropArea *dropArea;
    QMenu *menu;
    QLabel *plSelectedLabel, *topTitle, *topDesc;
    QPushButton *menuButton, *openPlButton, *okButton, *applyButton, *cancelButton;
    QAction *addFilesAction, *addDirAction, *newPlAction, *openPlAction, *savePlAction, *savePlAsAction, *closePlAction,
    *selectAllRowsAction, *delSelectedAction, *delAllAction, *shuffleAction;
};

class MusicPlaylistTreeView : public QTreeView
{
    Q_OBJECT

public:
    MusicPlaylistTreeView(QWidget *parent = 0);
    ~MusicPlaylistTreeView();
    void addRow(const QString &);
    int length();
    bool isEmpty();
    QStringList getAllRows();

public slots:
    void selectAllRows();
    void removeSelectedRows();
    void clear(bool update = true);

private:
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;

signals:
    void updatePlaylistStyle();

private:
    QStandardItemModel* model;
    QWidget *parent;
};
