/*******************************************************************************
  Omicron Media Player

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "RadioPlaylistManager.h"
#include <QHeaderView>

RadioPlaylistManager::RadioPlaylistManager(QWidget *parent) : DialogBase(parent)
{
    this->parent = parent;
    playlistChanged = false;
    playlist = new RadioPlaylistTreeView(this);
    playlist->sortByColumn(0, Qt::AscendingOrder);

    createWidgets();
    createLabels();
    createButtons();
    createBoxLayout();
    createEvents();
    loadPlaylist();
    setMinimumWidth(700);
    setWindowTitle("Gerenciador de Web Rádios » " + AppName);
}

RadioPlaylistManager::~RadioPlaylistManager()
{
}

//================================================================================================================
// private
//================================================================================================================

void RadioPlaylistManager::createWidgets()
{
    topWidget = new MyWidget;
    topWidget->setObjectName("topWidgetDialog");
    topTitle = new QLabel("Web Rádios");
    topTitle->setObjectName("topTitle");
    topDesc = new QLabel("» Adicione, edite ou remova suas Web Rádios personalizadas.");
    topDesc->setObjectName("topDesc");

    selectPlWidget = new MyWidget;
    selectPlWidget->setObjectName("selectPlWidget");
}

void RadioPlaylistManager::createLabels()
{
    QString value = Database::value("RadioMode", "playlist").toString();
    plSelectedLabel = new QLabel(value.isEmpty() ? "Usando Lista de Rádios padrão"
                                                 : QFileInfo(value).completeBaseName());
    plSelectedLabel->setObjectName("plSelectedLabel");
}

void RadioPlaylistManager::createButtons()
{
    menu = new QMenu(this);
    addFilesAction = menu->addAction("Adicionar Web Rádio");
    addFilesAction->setShortcut(QString("Insert"));
    addDirAction = menu->addAction("Editar Web Rádio");
    addDirAction->setShortcut(QString("F2"));
    menu->addSeparator();

    newPlAction = menu->addAction("Nova lista de rádios");
    newPlAction->setShortcut(QString("Ctrl+N"));
    openPlAction = menu->addAction("Abrir lista de rádios");
    openPlAction->setShortcut(QString("Ctrl+O"));
    savePlAction = menu->addAction("Salvar a lista de rádios");
    savePlAction->setShortcut(QString("Ctrl+S"));
    savePlAsAction = menu->addAction("Salvar a lista de rádios como...");
    savePlAsAction->setShortcut(QString("Ctrl+Alt+S"));
    closePlAction = menu->addAction("Fechar a lista de rádios");
    closePlAction->setShortcut(QString("Ctrl+W"));
    menu->addSeparator();

    delSelectedAction = menu->addAction("Apagar rádio selecionada");
    delSelectedAction->setShortcut(QString("Del"));
    delAllAction = menu->addAction("Limpar toda a lista de rádios");
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

void RadioPlaylistManager::createBoxLayout()
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

    setLayout(layoutMain);
}

void RadioPlaylistManager::createEvents()
{
    connect(playlist,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(editRadio()));

    connect(addFilesAction, SIGNAL(triggered()), this, SLOT(addRadio()));
    connect(addDirAction, SIGNAL(triggered()), this, SLOT(editRadio()));
    connect(newPlAction, SIGNAL(triggered()), this, SLOT(newPlaylist()));
    connect(openPlAction, SIGNAL(triggered()), this, SLOT(openPlaylist()));
    connect(savePlAction, SIGNAL(triggered()), this, SLOT(savePlaylist()));
    connect(savePlAsAction, SIGNAL(triggered()), this, SLOT(savePlaylistAs()));
    connect(closePlAction, SIGNAL(triggered()), this, SLOT(closePlaylist()));
    connect(delSelectedAction, SIGNAL(triggered()), playlist, SLOT(removeSelectedRows()));
    connect(delAllAction, SIGNAL(triggered()), playlist, SLOT(clear()));

    connect(openPlButton, SIGNAL(clicked()), this, SLOT(openPlaylist()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(applyButton, SIGNAL(clicked()), this, SLOT(savePlaylist()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

void RadioPlaylistManager::loadPlaylist()
{
    QString radiolist = Database::value("RadioMode", "playlist").toString();
    QFile file(radiolist.isEmpty() ? Global::getConfigPath("RadioList."+RadioPlaylistExt) : radiolist);

    if (!file.exists())
        return;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");

        while (!stream.atEnd())
        {
            QStringList list;
            QRegExp regExp("<title>(.*)<desc>(.*)<genre>(.*)<contry>(.*)<url1>(.*)<url2>(.*)<url3>(.*)<url4>(.*)<url5>(.*)<turl>(.*)<end>");

            if (regExp.indexIn(stream.readLine()) < 0)
                continue;

            list << regExp.cap(1);
            list << regExp.cap(2);
            list << regExp.cap(3);
            list << regExp.cap(4);
            list << regExp.cap(5);
            list << regExp.cap(6);
            list << regExp.cap(7);
            list << regExp.cap(8);
            list << regExp.cap(9);
            list << regExp.cap(10);

            playlist->addRow(list);
        }
        file.close();
    }
    else
    {
        QMessageBox::warning(nullptr, "Erro!", "Erro ao abrir o Playlist!");
    }
}

bool RadioPlaylistManager::eventFilter(QObject * obj, QEvent *event)
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

void RadioPlaylistManager::ok()
{
    savePlaylist(1);
    accept();
}

void RadioPlaylistManager::addRadio()
{
    EditRadio editRadio(this);

    if (editRadio.exec())
    {
        playlist->addRow(editRadio.getValues());
        playlist->sortByColumn(0, Qt::DescendingOrder);
        playlist->sortByColumn(0, Qt::AscendingOrder);
    }
}

void RadioPlaylistManager::editRadio()
{
    int row = playlist->getSelectedRow();

    if (row == -1)
        return;

    EditRadio editRadio(this, playlist->getRow(row));

    if (editRadio.exec())
    {
        playlist->addRow(editRadio.getValues(), row);
    }
}

bool RadioPlaylistManager::newPlaylist(bool close)
{
    bool ok;
    QString fileName = QInputDialog::getText(this, "Nome do RadioList", (close ? "Nome" : "Novo nome")
                                             +QString(" do seu RadioList."), QLineEdit::Normal,
                                             QString("Novo ")+(close ? "RadioList" : "nome"), &ok);
    if (!ok)
        return false;

    if (fileName.isEmpty())
        fileName = "Novo RadioList";

    if (!fileName.endsWith("."+RadioPlaylistExt, Qt::CaseInsensitive))
        fileName += "."+RadioPlaylistExt;

    fileName = QFileDialog::getSaveFileName(this, "Salvar RadioList",
                                            Database::value("Current", "fileDialogPl", QDir::homePath()).toString()+"/"+fileName,
                                            "RadioList (*."+RadioPlaylistExt+")");

    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith("."+RadioPlaylistExt, Qt::CaseInsensitive))
            fileName += "."+RadioPlaylistExt;

        Database::setValue("Current", "fileDialogPl", QFileInfo(fileName).absolutePath());

        QFile filePlaylist(fileName);

        if (filePlaylist.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            if (close) closePlaylist(false);

            Database::setValue("RadioMode", "playlist", fileName);
            plSelectedLabel->setText(QFileInfo(fileName).completeBaseName());

            QTextStream stream(&filePlaylist);
            stream.setCodec("UTF-8");
            stream.flush();
            filePlaylist.close();
        }
        else
        {
            QMessageBox::warning(this, "Erro!", "Erro ao criar o Playlist!");
        }
    }
    else
    {
        return false;
    }
    return true;
}

void RadioPlaylistManager::openPlaylist()
{
    QString fileName;
    QFileDialog fileDialog(this);

    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setDirectory(Database::value("Current", "fileDialogPl", QDir::homePath()).toString());
    fileDialog.setNameFilter("RadioList (*."+RadioPlaylistExt+")");

    if (fileDialog.exec())
    {
        closePlaylist(false);
        fileName = fileDialog.selectedFiles()[0];
        Database::setValue("Current", "fileDialogPl", fileDialog.directory().absolutePath());
        Database::setValue("RadioMode", "playlist", fileName);
        plSelectedLabel->setText(QFileInfo(fileName).completeBaseName());
        loadPlaylist();
    }
}

void RadioPlaylistManager::savePlaylist(int arg)
{
    if (arg == 2 && !newPlaylist(false))
        return;

    playlist->sortByColumn(0, Qt::DescendingOrder);
    playlist->sortByColumn(0, Qt::AscendingOrder);

    QString radiolist = Database::value("RadioMode", "playlist").toString();
    QFile filePlaylist(radiolist.isEmpty() ? Global::getConfigPath("RadioList."+RadioPlaylistExt) : radiolist);

    if (filePlaylist.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&filePlaylist);
        stream.setCodec("UTF-8");

        while (1)
        {
            QStringList ls = playlist->getRow();

            if (ls.isEmpty())
                break;

            stream << "<title>"+ls[0]+"<desc>"+ls[1]+"<genre>"+ls[2]+"<contry>"+ls[3]+"<url1>"+ls[4]
                      +"<url2>"+ls[5]+"<url3>"+ls[6]+"<url4>"+ls[7]+"<url5>"+ls[8]+"<turl>"+ls[9]+"<end>" << endl;
        }

        stream.flush();
        filePlaylist.close();
    }
    else
    {
        QMessageBox::warning(nullptr, "Erro!", "Erro ao Salvar o Playlist!");
    }
}

void RadioPlaylistManager::savePlaylistAs()
{
    savePlaylist(2);
}

void RadioPlaylistManager::closePlaylist(const bool arg)
{
    playlistChanged = true;
    Database::clear("RadioMode", "playlist");
    playlist->clear();
    plSelectedLabel->setText("Usando Lista de Rádios padrão");
    if (arg)
        loadPlaylist();
}

//================================================================================================================
// class RadioPlaylistTreeView
//================================================================================================================

RadioPlaylistTreeView::RadioPlaylistTreeView(QWidget *parent) : QTreeView(parent)
{
    this->parent = parent;
    getAllRowsCount = 0;

    model = new QStandardItemModel(0, 10);
    model->setHeaderData(0, Qt::Horizontal, "Título");
    model->setHeaderData(1, Qt::Horizontal, "Descrição");
    model->setHeaderData(2, Qt::Horizontal, "Gênero");
    model->setHeaderData(3, Qt::Horizontal, "País");
    model->setHeaderData(9, Qt::Horizontal, "Links");

    setAllColumnsShowFocus(true);
    setAlternatingRowColors(true);
    setEditTriggers(NoEditTriggers);
    setSelectionMode(SingleSelection);
    setSelectionBehavior(SelectRows);
    setIndentation(0);
    setFixedHeight(245);
    setSortingEnabled(true);
    setModel(model);

    setColumnWidth(0,140);
    setColumnWidth(1,140);
    setColumnWidth(2,140);
    setColumnWidth(3,140);
    setColumnWidth(9,100);

    setColumnHidden(4,true);
    setColumnHidden(5,true);
    setColumnHidden(6,true);
    setColumnHidden(7,true);
    setColumnHidden(8,true);
}
RadioPlaylistTreeView::~RadioPlaylistTreeView()
{
    delete model;
}

void RadioPlaylistTreeView::addRow(const QStringList &arg, const int &edit)
{
    QList<QStandardItem *> item;
    item << new QStandardItem(arg[0]);
    item << new QStandardItem(arg[1]);
    item << new QStandardItem(arg[2]);
    item << new QStandardItem(arg[3]);
    item << new QStandardItem(arg[4]);
    item << new QStandardItem(arg[5]);
    item << new QStandardItem(arg[6]);
    item << new QStandardItem(arg[7]);
    item << new QStandardItem(arg[8]);
    item << new QStandardItem(arg[9]);

    item[0]->setToolTip(arg[0]);
    item[1]->setToolTip(arg[1]);
    item[2]->setToolTip(arg[2]);
    item[3]->setToolTip(arg[3]);

    if (edit > -1)
    {
        model->removeRow(edit);
        model->insertRow(edit, item);
        selectionModel()->setCurrentIndex(model->index(edit,0), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
    else
    {
        model->appendRow(item);
    }
}

bool RadioPlaylistTreeView::isEmpty()
{
    return !model->hasChildren();
}

int RadioPlaylistTreeView::getSelectedRow()
{
    if (selectionModel()->selectedRows().isEmpty())
        return -1;

    return selectionModel()->selectedRows()[0].row();
}

QStringList RadioPlaylistTreeView::getRow(const int &arg)
{
    QStringList list;

    if (model->hasChildren() && getAllRowsCount < model->rowCount())
    {
        if (arg > -1)
            getAllRowsCount = arg;

        for (int i = 0; i < 10; i++)
            list << model->data(model->index(getAllRowsCount, i)).toString();

        if (arg == -1)
            getAllRowsCount++;
        else
            getAllRowsCount = 0;
    }
    else
    {
        getAllRowsCount = 0;
    }

    return list;
}

//================================================================================================================
// public slots
//================================================================================================================

void RadioPlaylistTreeView::removeSelectedRows()
{
    if (model->hasChildren())
    {
        QList<QModelIndex> selectedRows = selectionModel()->selectedRows();

        for (int i = selectedRows.length()-1; i > -1; --i)
            model->removeRow(selectedRows[i].row());
    }
}

void RadioPlaylistTreeView::clear()
{
    if (model->hasChildren())
        model->removeRows(0, model->rowCount());
}

//================================================================================================================
// private
//================================================================================================================

void RadioPlaylistTreeView::keyPressEvent(QKeyEvent *event)
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

//================================================================================================================
// class EditRadio
//================================================================================================================

EditRadio::EditRadio(QWidget *parent, const QStringList &radio) : DialogBase(parent)
{
    QStringList labelQStringList;
    labelQStringList << "Título" << "Descrição" << "Gênero" << "País" << "Link 1" << "Link 2" << "Link 3" << "Link 4" << "Link 5";
    int i;

    QPushButton *okButton = new QPushButton("Ok");
    QPushButton *cancelButton = new QPushButton("Cancelar");

    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->setContentsMargins(0,4,0,0);
    buttonLayout->setSpacing(5);
    buttonLayout->setAlignment(Qt::AlignRight);

    QVBoxLayout *layout = new QVBoxLayout;

    for (i = 0; i < 9; i++)
    {
        QLabel *label = new QLabel(labelQStringList[i]);
        lineEdit[i] = new QLineEdit;
        lineEdit[i]->setClearButtonEnabled(true);

        layout->addWidget(label);
        layout->addWidget(lineEdit[i]);
        layout->addSpacing(6);
    }

    for (i = 0; i < radio.length()-1; i++)
        lineEdit[i]->setText(radio[i]);

    layout->addLayout(buttonLayout);
    layout->setContentsMargins(10,5,10,10);
    layout->setSpacing(0);

    setLayout(layout);
    setWindowTitle((radio.isEmpty() ? "Adicionar" : "Editar") + QString(" Rádio » ") + AppName);
    setFixedWidth(460);
}

EditRadio::~EditRadio()
{
}

QStringList EditRadio::getValues()
{
    return values;
}

//================================================================================================================
// private slots
//================================================================================================================

void EditRadio::ok()
{
    int i, j = 0;
    values.clear();

    for (i = 0; i < 9; i++)
        values << lineEdit[i]->text();

    for (i = 8; i > 3; i--)
    {
        if (values[i].isEmpty())
            values.removeAt(i);
        else
            j++;
    }

    for (i = 4+j; i < 9; i++)
        values.append("");

    values << QString("%1").arg(j);

    if (!values[0].isEmpty() && !values[4].isEmpty())
        accept();
    else
        Information(this, "Digite, ao menos, um título e um link.");
}
