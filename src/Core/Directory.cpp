/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "Directory.h"
#include "Global.h"

#include <QDirIterator>
#include <QLayout>

Directory::Directory(QWidget *parent) : parent(parent)
{
    dialog = nullptr;
    label = nullptr;
    stop = false;
    countFiles = 0;

    connect(this, &Directory::finished, this, &Directory::deleteLater);
    connect(this, &Directory::updateFiles, this, &Directory::update);
}

Directory::~Directory()
{
    if (execLater)
        execLater();

    delete dialog;
}

void Directory::getAllFiles(QString &dirName, std::function<void(QFileInfo &)> func)
{
    this->dirName = dirName;
    this->func = std::move(func);

    dialog = new DialogBase(parent, false);
    dialog->setFixedWidth(400);

    label = new QLabel("---");

    QPushButton *button = new QPushButton("Cancelar");
    connect(button, &QPushButton::clicked, [=]() { stop = true; });

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel("<strong>Procurado por arquivos de áudio. Aguarde...</strong>"));
    layout->addWidget(label);
    layout->addWidget(button, 0, Qt::AlignRight);

    dialog->setLayout(layout);
    dialog->show();
}

void Directory::executeLater(std::function<void(void)> func)
{
    execLater = std::move(func);
}

//================================================================================================================
// public slots
//================================================================================================================

void Directory::update(QFileInfo fileInfo)
{
    if (func)
        func(fileInfo);

    if (countFiles > 9)
    {
        label->setText(fileInfo.completeBaseName());
        countFiles = 0;
    }
    else
    {
        countFiles++;
    }
}

//================================================================================================================
// private
//================================================================================================================

void Directory::run()
{
    QDirIterator dirIt(dirName, QDirIterator::Subdirectories);
    QRegExp filter(QString(FileNameFilter).replace(" ", "|").remove("*."), Qt::CaseInsensitive);

    while (dirIt.hasNext())
    {
        if (stop)
            break;

        QFileInfo fileInfo(dirIt.next());

        if (fileInfo.isFile() && fileInfo.suffix().contains(filter))
            emit updateFiles(fileInfo);
    }
}
