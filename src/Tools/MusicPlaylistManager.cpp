/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "MusicPlaylistManager.h"
#include "../Core/Directory.h"

MusicPlaylistManager::MusicPlaylistManager(QWidget *parent) : DialogBase(parent)
{
    this->parent = parent;
    playlist = new MusicPlaylistTreeView;
    playlistChanged = false;

    createWidgets();
    createLabels();
    createButtons();
    createBoxLayout();
    createEvents();
    setWindowTitle("Gerenciador de Playlist » "+AppName);
    setMinimumWidth(700);

    if (!Database::value("MusicMode", "playlist").toString().isEmpty())
        if (!QFile::exists(Database::value("MusicMode", "playlist").toString()))
            closePlaylist();

    loadPlaylist();
    updatePlaylistStyle();
}

MusicPlaylistManager::~MusicPlaylistManager()
{
}

void MusicPlaylistManager::loadPlaylist()
{
    QString playlistName = Database::value("MusicMode", "playlist").toString();
    QFile file(playlistName.isEmpty() ? Global::getConfigPath("Playlist."+PlaylistExt) : playlistName);

    if (file.exists())
    {
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");

            while (!stream.atEnd())
                playlist->addRow(stream.readLine());

            file.close();
        }
        else
        {
            QMessageBox::warning(nullptr, "Erro!", "Erro ao abrir o Playlist!");
        }
    }
}

//================================================================================================================
// public slots
//================================================================================================================

void MusicPlaylistManager::closePlaylist()
{
    playlistChanged = true;
    Database::clear("MusicMode", "playlist");
    playlist->clear();
    plSelectedLabel->setText("Sem Playlist (usando playlist padrão)");
}

void MusicPlaylistManager::updatePlaylistStyle(bool arg)
{
    if (arg)
    {
        playlist->hideColumn(0);
        playlist->hideColumn(1);
        playlist->setProperty("playlistStyle", "drag");
    }
    else if (playlist->isEmpty())
    {
        playlist->hideColumn(0);
        playlist->hideColumn(1);
        playlist->setToolTip("Arraste arquivos para adicionar ao Playlist");
        playlist->setProperty("playlistStyle", "drop");
    }
    else
    {
        playlist->showColumn(1);
        playlist->setColumnWidth(1,300);
        playlist->showColumn(0);
        playlist->setColumnWidth(0,350);
        playlist->setToolTip("");
        playlist->setProperty("playlistStyle", false);
    }

    MyWidget::updateStyle(playlist);
}

//================================================================================================================
// private
//================================================================================================================

void MusicPlaylistManager::createWidgets()
{
    dropArea = new DropArea(this);

    topWidget = new MyWidget;
    topWidget->setObjectName("topWidgetDialog");
    topTitle = new QLabel("Playlist");
    topTitle->setObjectName("topTitle");
    topDesc = new QLabel("» Aqui você pode criar, editar ou remover seus playlists.");
    topDesc->setObjectName("topDesc");

    selectPlWidget = new MyWidget;
    selectPlWidget->setObjectName("selectPlWidget");
}

void MusicPlaylistManager::createLabels()
{
    QString value = Database::value("MusicMode", "playlist").toString();
    plSelectedLabel = new QLabel(value.isEmpty() ? "Sem Playlist (usando playlist padrão)"
                                                 : QFileInfo(value).completeBaseName());
    plSelectedLabel->setObjectName("plSelectedLabel");
}

void MusicPlaylistManager::createButtons()
{
    menu = new QMenu(this);
    addFilesAction = menu->addAction("Adicionar arquivos");
    addFilesAction->setShortcut(QString("Insert"));
    addDirAction = menu->addAction("Adicionar pasta");
    addDirAction->setShortcut(QString("Ctrl+Insert"));
    menu->addSeparator();

    newPlAction = menu->addAction("Novo playlist");
    newPlAction->setShortcut(QString("Ctrl+N"));
    openPlAction = menu->addAction("Abrir playlist");
    openPlAction->setShortcut(QString("Ctrl+O"));
    savePlAction = menu->addAction("Salvar o playlist");
    savePlAction->setShortcut(QString("Ctrl+S"));
    savePlAsAction = menu->addAction("Salvar o playlist como...");
    savePlAsAction->setShortcut(QString("Ctrl+Alt+S"));
    closePlAction = menu->addAction("Fechar o playlist");
    closePlAction->setShortcut(QString("Ctrl+W"));
    menu->addSeparator();

    shuffleAction = menu->addAction("Embaralhar lista");
    shuffleAction->setShortcut(QString("F3"));
    selectAllRowsAction = menu->addAction("Selecionar todos os arquivos");
    selectAllRowsAction->setShortcut(QString("Ctrl+A"));
    delSelectedAction = menu->addAction("Limpar arquivos selecionados");
    delSelectedAction->setShortcut(QString("Del"));
    delAllAction = menu->addAction("Limpar todo o playlist");
    delAllAction->setShortcut(QString("Ctrl+Del"));

    menuButton = new QPushButton("Menu");
    menuButton->setObjectName("menuButton");
    menuButton->setMenu(menu);
    menu->installEventFilter(this);

    openPlButton = new QPushButton("Abrir");
    okButton = new QPushButton("Ok");
    applyButton = new QPushButton("Salvar");
    cancelButton = new QPushButton("Cancelar");
}

void MusicPlaylistManager::createBoxLayout()
{
    QVBoxLayout *topVLayout = new QVBoxLayout;
    topVLayout->addWidget(topTitle);
    topVLayout->addWidget(topDesc);
    topVLayout->setContentsMargins(0,4,0,9);
    topVLayout->setSpacing(2);

    QHBoxLayout *topHLayout = new QHBoxLayout;
    topHLayout->addLayout(topVLayout,1);
    topHLayout->addWidget(menuButton);
    topHLayout->setContentsMargins(10,0,10,0);
    topHLayout->setSpacing(0);

    topWidget->setLayout(topHLayout);

    QHBoxLayout *selectPlLayout = new QHBoxLayout;
    selectPlLayout->addWidget(openPlButton);
    selectPlLayout->addWidget(plSelectedLabel,1);
    selectPlLayout->setMargin(0);
    selectPlLayout->setSpacing(0);

    selectPlWidget->setLayout(selectPlLayout);

    QHBoxLayout *selectPlHLayout = new QHBoxLayout;
    selectPlHLayout->addWidget(selectPlWidget);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addLayout(selectPlHLayout,1);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(applyButton);
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->setSpacing(8);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(playlist);
    vLayout->addLayout(buttonsLayout);
    vLayout->setContentsMargins(8,8,8,8);
    vLayout->setSpacing(8);

    QVBoxLayout *layoutMain = new QVBoxLayout;
    layoutMain->addWidget(topWidget);
    layoutMain->addLayout(vLayout);
    layoutMain->setContentsMargins(0,0,0,0);
    layoutMain->setSpacing(0);

    dropArea->setLayout(layoutMain);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(dropArea);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    setLayout(layout);
}

void MusicPlaylistManager::createEvents()
{
    connect(addFilesAction, SIGNAL(triggered()), this, SLOT(addFiles()));
    connect(addDirAction, SIGNAL(triggered()), this, SLOT(addDirectory()));
    connect(newPlAction, SIGNAL(triggered()), this, SLOT(newPlaylist()));
    connect(openPlAction, SIGNAL(triggered()), this, SLOT(openPlaylist()));
    connect(savePlAction, SIGNAL(triggered()), this, SLOT(savePlaylist()));
    connect(savePlAsAction, SIGNAL(triggered()), this, SLOT(savePlaylistAs()));
    connect(closePlAction, SIGNAL(triggered()), this, SLOT(closePlaylist()));
    connect(closePlAction, SIGNAL(triggered()), this, SLOT(closeDefaultPlaylist()));
    connect(selectAllRowsAction, SIGNAL(triggered()), playlist, SLOT(selectAllRows()));
    connect(delSelectedAction, SIGNAL(triggered()), playlist, SLOT(removeSelectedRows()));
    connect(delAllAction, SIGNAL(triggered()), playlist, SLOT(clear()));
    connect(shuffleAction, SIGNAL(triggered()), this, SLOT(shuffle()));

    connect(openPlButton, SIGNAL(clicked()), this, SLOT(openPlaylist()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(savePlaylist()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    connect(playlist, SIGNAL(updatePlaylistStyle()), this, SLOT(updatePlaylistStyle()));

    connect(dropArea, SIGNAL(dragAndDrop(bool)), this, SLOT(updatePlaylistStyle(bool)));
    connect(dropArea, SIGNAL(filesFromDropArea(QStringList)), this, SLOT(receiveMessage(QStringList)));
}

bool MusicPlaylistManager::eventFilter(QObject * obj, QEvent *event)
{
    if (event->type() == QEvent::Show && obj == menuButton->menu())
    {
        menuButton->menu()->move(menuButton->mapToGlobal(QPoint(menuButton->width()-menuButton->menu()->width(),menuButton->height())));
        return true;
    }
    return false;
}

//================================================================================================================
// private slots
//================================================================================================================

void MusicPlaylistManager::ok()
{
    savePlaylist(1);
    accept();
}

void MusicPlaylistManager::addFiles()
{
    QStringList fileNames;
    QFileDialog fileDialog(this);

    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setDirectory(Database::value("Current", "fileDialog", QDir::homePath()).toString());
    fileDialog.setNameFilter("Audio ("+FileNameFilter+");;Todos os Arquivos (*.*)");

    if (fileDialog.exec())
    {
        fileNames = fileDialog.selectedFiles();
        Database::setValue("Current", "fileDialog", fileDialog.directory().absolutePath());

        for (int i = 0; i < fileNames.size(); i++)
            playlist->addRow(fileNames[i]);

        updatePlaylistStyle();
    }
}

void MusicPlaylistManager::addDirectory()
{
    QString dirName = QFileDialog::getExistingDirectory(this, "Adicionar diretório",
                                                        Database::value("Current", "fileDialogDir", QDir::homePath()).toString());

    if (!dirName.isEmpty())
    {
        Database::setValue("Current", "fileDialogDir", dirName);

        Directory *dir = new Directory(this);

        dir->getAllFiles(dirName, [&](QFileInfo &fileInfo) {
            playlist->addRow(fileInfo.filePath());
        });

        dir->executeLater([=]() {
            updatePlaylistStyle();
        });

        dir->start();
    }
}

bool MusicPlaylistManager::newPlaylist(bool close)
{
    bool ok;
    QString fileName = QInputDialog::getText(this,"Nome do Playlist",(close ? "Nome" : "Novo nome")
                                             +QString(" do seu playlist."),QLineEdit::Normal,
                                             QString("Novo ")+(close ? "Playlist" : "nome"),&ok);
    if (!ok)
        return false;

    if (fileName.isEmpty())
        fileName = "Novo Playlist";

    if (!fileName.endsWith("."+PlaylistExt, Qt::CaseInsensitive))
        fileName += "."+PlaylistExt;

    fileName = QFileDialog::getSaveFileName(this, "Salvar Playlist",
                                            Database::value("Current", "fileDialogPl", QDir::homePath()).toString()+"/"+fileName,
                                            "Playlist (*."+PlaylistExt+")");

    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith("."+PlaylistExt, Qt::CaseInsensitive))
            fileName += "."+PlaylistExt;

        Database::setValue("Current", "fileDialogPl", QFileInfo(fileName).absolutePath());

        QFile filePlaylist(fileName);

        if (filePlaylist.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            if (close) closePlaylist();
            closeDefaultPlaylist();

            Database::setValue("MusicMode", "playlist", fileName);
            plSelectedLabel->setText(QFileInfo(fileName).completeBaseName());

            QTextStream stream(&filePlaylist);
            stream.setCodec("UTF-8");
            stream.flush();
            filePlaylist.close();
        }
        else
        {
            QMessageBox::warning(this,"Erro!", "Erro ao criar o Playlist!");
        }
    }
    else
    {
        return false;
    }
    return true;
}

void MusicPlaylistManager::openPlaylist()
{
    QString fileName;
    QFileDialog fileDialog(this);

    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setDirectory(Database::value("Current", "fileDialogPl", QDir::homePath()).toString());
    fileDialog.setNameFilter("Playlist (*."+PlaylistExt+")");

    if (fileDialog.exec())
    {
        closePlaylist();
        closeDefaultPlaylist();
        fileName = fileDialog.selectedFiles()[0];
        Database::setValue("Current", "fileDialogPl", fileDialog.directory().absolutePath());
        Database::setValue("MusicMode", "playlist", fileName);
        plSelectedLabel->setText(QFileInfo(fileName).completeBaseName());
        loadPlaylist();
        updatePlaylistStyle();
    }
}

void MusicPlaylistManager::savePlaylist(int arg)
{
    QStringList fileNames = playlist->getAllRows();

    if (fileNames.isEmpty())
        return;

    //if (arg == 1 && Database::value("MusicMode", "playlist").toString().isEmpty())
      //  return;

    if (/*Database::value("MusicMode", "playlist").toString().isEmpty() || */arg == 2)
        if (!newPlaylist(false))
            return;

    QString playlistName = Database::value("MusicMode", "playlist").toString();
    QFile filePlaylist(playlistName.isEmpty() ? Global::getConfigPath("Playlist."+PlaylistExt) : playlistName);

    if (filePlaylist.open(QIODevice::WriteOnly | QIODevice::Text ))
    {
        QTextStream stream(&filePlaylist);
        stream.setCodec("UTF-8");

        for (int i = 0; i < fileNames.length(); i++)
            stream << fileNames[i] << endl;

        stream.flush();
        filePlaylist.close();
    }
    else
    {
        QMessageBox::warning(nullptr, "Erro!", "Erro ao Salvar o Playlist!");
    }
}

void MusicPlaylistManager::savePlaylistAs()
{
    savePlaylist(2);
}

void MusicPlaylistManager::closeDefaultPlaylist()
{
    QFile filePlaylist(Global::getConfigPath("Playlist."+PlaylistExt));

    if (filePlaylist.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&filePlaylist);
        stream.setCodec("UTF-8");
        stream.flush();
        filePlaylist.close();
    }

    playlist->clear();
    plSelectedLabel->setText("Sem Playlist (usando playlist padrão)");
}

void MusicPlaylistManager::shuffle()
{
    QStringList list = playlist->getAllRows();
    playlist->clear(false);

    for (int i = list.length(); i > 0; i--)
    {
        qsrand(QTime::currentTime().msec()+i);
        int j = qrand() % i;
        playlist->addRow(list[j]);
        list.removeAt(j);
    }
}

void MusicPlaylistManager::receiveMessage(QStringList list)
{
    if (!list.isEmpty() && list[0] == "--nohide")
        list.removeAt(0);

    for (int i = 0; i < list.length(); i++)
        playlist->addRow(list[i]);
}

//================================================================================================================
// class MusicPlaylistTreeView
//================================================================================================================

MusicPlaylistTreeView::MusicPlaylistTreeView(QWidget *parent) : QTreeView(parent)
{
    this->parent = parent;

    model = new QStandardItemModel(0, 3);
    model->setHeaderData(0, Qt::Horizontal, "Nome do Arquivo");
    model->setHeaderData(1, Qt::Horizontal, "Diretório Atual");

    setAllColumnsShowFocus(true);
    setAlternatingRowColors(true);
    setEditTriggers(NoEditTriggers);
    setSelectionMode(ExtendedSelection);
    setSelectionBehavior(SelectRows);
    setIndentation(0);
    setFixedHeight(245);

    //setDragEnabled(true);
    //setAcceptDrops(true);
    //setDropIndicatorShown(true);

    setModel(model);

    setColumnWidth(0,350);
    setColumnWidth(1,300);
    setColumnHidden(2,true);
}
MusicPlaylistTreeView::~MusicPlaylistTreeView()
{
    delete model;
}

void MusicPlaylistTreeView::addRow(const QString &file)
{
    QFileInfo fileInfo(file);
    QList<QStandardItem *> item;

    item << new QStandardItem(fileInfo.completeBaseName());
    item[0]->setToolTip(fileInfo.fileName());

    item << new QStandardItem(fileInfo.dir().dirName());
    item[1]->setToolTip(fileInfo.absolutePath());

    item << new QStandardItem(file);

    model->appendRow(item);
}

int MusicPlaylistTreeView::length()
{
    return model->rowCount();
}

bool MusicPlaylistTreeView::isEmpty()
{
    return !model->hasChildren();
}

QStringList MusicPlaylistTreeView::getAllRows()
{
    QStringList list;

    if (model->hasChildren())
        for (int i = 0; i < model->rowCount(); i++)
            list << model->data(model->index(i, 2)).toString();

    return list;
}

//================================================================================================================
// public slots
//================================================================================================================

void MusicPlaylistTreeView::selectAllRows()
{
    if (model->hasChildren())
        selectAll();
}

void MusicPlaylistTreeView::removeSelectedRows()
{
    if (model->hasChildren())
    {
        QList<QModelIndex> selectedRows = selectionModel()->selectedRows();
        for (int i = selectedRows.length()-1; i > -1; --i)
            model->removeRow(selectedRows[i].row());
    }

    if (isEmpty())
        emit updatePlaylistStyle();
}

void MusicPlaylistTreeView::clear(bool update)
{
    if (model->hasChildren())
        model->removeRows(0, model->rowCount());

    if (update)
        emit updatePlaylistStyle();
}

//================================================================================================================
// private
//================================================================================================================

void MusicPlaylistTreeView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space || event->key() == Qt::Key_Return)
    {
        event->accept();
        emit doubleClicked(selectionModel()->selectedRows().value(0));
    }
    else
    {
        QTreeView::keyPressEvent(event);
    }
}
