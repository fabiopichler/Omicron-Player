/*******************************************************************************
  Tchê Media Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "ErrorWindow.h"

ErrorWindow::ErrorWindow(QWidget *parent) : DialogBase(parent)
{
    createWidgets();
    createLayouts();
    createEvents();
    setWindowTitle("Erro » "+AppName);
    setMinimumSize(500, 300);
}

ErrorWindow::~ErrorWindow()
{
}

void ErrorWindow::addError(QString msg)
{
    if (!contentTextEdit->toPlainText().isEmpty())
        msg.push_front("<div>---------------------------------------------------</div>");

    contentTextEdit->append(msg);
}

//================================================================================================================
// private
//================================================================================================================

void ErrorWindow::createWidgets()
{
    infoLabel = new QLabel("Lista dos erros ocorridos.");

    stopButton = new QPushButton("Parar Reprodução");
    clearButton = new QPushButton("Limpar");
    closeButton = new QPushButton("Fechar");

    contentTextEdit = new QTextEdit;
    contentTextEdit->setReadOnly(true);
}

void ErrorWindow::createLayouts()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(stopButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(closeButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(infoLabel);
    mainLayout->addWidget(contentTextEdit);
    mainLayout->addLayout(buttonLayout);
    mainLayout->setMargin(8);
    mainLayout->setSpacing(8);

    setLayout(mainLayout);
}

void ErrorWindow::createEvents()
{
    connect(stopButton, SIGNAL(clicked()), this, SIGNAL(stopStream()));
    connect(clearButton, SIGNAL(clicked()), contentTextEdit, SLOT(clear()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
}
