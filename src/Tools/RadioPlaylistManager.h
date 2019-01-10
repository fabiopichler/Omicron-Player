/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Core/Database.h"
#include "../Gui/DialogBase.h"

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

class RadioPlaylistTreeView;
class RadioPlaylistManager : public DialogBase
{
    Q_OBJECT

public:
    RadioPlaylistManager(QWidget *);
    ~RadioPlaylistManager();

private:
    void createWidgets();
    void createLabels();
    void createButtons();
    void createBoxLayout();
    void createEvents();
    void loadPlaylist();
    bool eventFilter(QObject * obj, QEvent *event) Q_DECL_OVERRIDE;

private slots:
    void ok();
    void addRadio();
    void editRadio();
    bool newPlaylist(bool close = true);
    void openPlaylist();
    void savePlaylist(int arg = 0);
    void savePlaylistAs();
    void closePlaylist(const bool arg = true);

public:
    RadioPlaylistTreeView *playlist;
    bool playlistChanged;

private:
    QWidget *parent;
    MyWidget *topWidget, *selectPlWidget;
    QMenu *menu;
    QLabel *plSelectedLabel, *topTitle, *topDesc;
    QPushButton *menuButton, *openPlButton, *okButton, *applyButton, *cancelButton;
    QAction *addFilesAction, *addDirAction, *newPlAction, *openPlAction, *savePlAction, *savePlAsAction, *closePlAction,
    *selectAllRowsAction, *delSelectedAction, *delAllAction;
};

class RadioPlaylistTreeView : public QTreeView
{
    Q_OBJECT

public:
    RadioPlaylistTreeView(QWidget *parent = 0);
    ~RadioPlaylistTreeView();
    void addRow(const QStringList &, const int &edit = -1);
    bool isEmpty();
    int getSelectedRow();
    QStringList getRow(const int &arg = -1);


public slots:
    void removeSelectedRows();
    void clear();

private:
    void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;

    QStandardItemModel* model;
    QWidget *parent;
    int getAllRowsCount;
};

class EditRadio : public DialogBase
{
    Q_OBJECT

public:
    EditRadio(QWidget *, const QStringList &radio = QStringList());
    ~EditRadio();
    QStringList getValues();

private slots:
    void ok();

private:
    QLineEdit *lineEdit[9];
    QStringList values;
};
