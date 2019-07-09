/*******************************************************************************

  Omicron Player Classic

  Author: Fábio Pichler
  Website: http://fabiopichler.net
  License: BSD 3-Clause License

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Omicron Player Classic nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#pragma once

#include "../Core/Database.h"

#include <OmicronTK/Qt/Widget.hpp>
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

class RadioPlaylistTreeView;
class RadioPlaylistManager : public OmicronTK::OTKQT::Dialog
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
    OmicronTK::OTKQT::Widget *topWidget, *selectPlWidget;
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

class EditRadio : public OmicronTK::OTKQT::Dialog
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
