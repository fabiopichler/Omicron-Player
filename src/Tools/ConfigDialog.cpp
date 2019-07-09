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

#include "ConfigDialog.h"

#include <OmicronTK/Qt/AppInfo.hpp>

#include <QtWidgets>

using namespace OmicronTK;

int ConfigDialog::lastPage = 0;

ConfigDialog::ConfigDialog(QObject *parentMain, QWidget *parent) : OTKQT::Dialog(parent)
{
    this->parent = parent;
    this->parentMain = parentMain;

    connect(this, SIGNAL(restartApp()), parentMain, SLOT(restart()));

    createWidgets();
    createLayouts();
    createEvents();
    setWindowTitle("Configurações do "+AppName);
    setMinimumSize(600, 489);
}

//================================================================================================================
// public slots
//================================================================================================================

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    int row = contentsListWidget->row(current);
    lastPage = row;
    pagesWidget->setCurrentIndex(row);
    contentsTitle->setText(titles[row]);
}

//================================================================================================================
// private
//================================================================================================================

void ConfigDialog::createWidgets()
{
    contentsListWidget = new QListWidget;
    contentsListWidget->setViewMode(QListView::IconMode);
    contentsListWidget->setIconSize(QSize(48, 48));
    contentsListWidget->setMovement(QListView::Static);
    contentsListWidget->setFixedWidth(152);
    contentsListWidget->setSpacing(12);
    contentsListWidget->setStyleSheet("font-size: 10px;");

    pagesWidget = new QStackedWidget;
    pagesWidget->addWidget(new GeneralPage(this));
    pagesWidget->addWidget(new InterfacePage(this));
    pagesWidget->addWidget(new MusicPage(this));
    pagesWidget->addWidget(new WebRadioPage(this));
#ifdef Q_OS_WIN
    pagesWidget->addWidget(new FileTypePage(this));
#endif //Q_OS_WIN
    pagesWidget->addWidget(new AudioPage(parentMain, this));
    pagesWidget->addWidget(new RecordingsPage(this));
    pagesWidget->addWidget(new NetworkPage(this));
    pagesWidget->addWidget(new UpdatePage(this));

    contentsTitle = new QLabel;
    contentsTitle->setObjectName("contentsTitle");

    contentsWidget = new OTKQT::Widget;
    contentsWidget->setObjectName("configContent");

    defaultsButton = new QPushButton("Definir configurações padrão");
    closeButton = new QPushButton("Fechar");

    createIcons();
    contentsListWidget->setCurrentRow(lastPage);
}

void ConfigDialog::createLayouts()
{
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(contentsTitle);
    vLayout->addWidget(pagesWidget);
    vLayout->setMargin(2);
    vLayout->setSpacing(1);
    contentsWidget->setLayout(vLayout);

    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsListWidget);
    horizontalLayout->addWidget(contentsWidget, 1);
    horizontalLayout->setSpacing(8);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(defaultsButton);
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addSpacing(2);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->setMargin(8);

    setLayout(mainLayout);
}

void ConfigDialog::createEvents()
{
    connect(defaultsButton, SIGNAL(clicked()), parentMain, SLOT(defaultConfig()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}

void ConfigDialog::createIcons()
{
    QStringList images;
    int len = 8;

    titles << "Geral" << "Interface" << "Modo Músicas" << "Modo Web Rádio" << "Áudio"
           << "Gravações" << "Rede" << "Atualizações";
    images << "Settings.png" << "Palette.png" << "Device-Headphone.png" << "Device-Radio.png" << "Volume.png"
           << "Voice-Search.png" << "Network.png" << "Synchronize.png";

#ifdef Q_OS_WIN
    titles.insert(4, "Tipo de arquivos");
    images.insert(4, "Document-Music-02.png");
    len = 9;
#endif

    for (int i = 0; i < len; i++)
    {
        QListWidgetItem *configButton = new QListWidgetItem(contentsListWidget);
        configButton->setIcon(QIcon(OTKQT::AppInfo::themePath()+QString("images/config/")+images[i]));
        configButton->setText(titles[i]);
        configButton->setTextAlignment(Qt::AlignHCenter);
        configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        configButton->setSizeHint(QSize(110,68));
    }

    connect(contentsListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}
