/*******************************************************************************
  FPM Player

  Copyright (c) 2016, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#pragma once

#include "../Gui/TitleBar.h"
#include "../Core/Database.h"

#include <QDialog>

class DialogBase : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int marginLayout READ marginLayout WRITE setMarginLayout)
    Q_PROPERTY(int spacingLayout READ spacingLayout WRITE setSpacingLayout)

public:
    explicit DialogBase(QWidget *);
    ~DialogBase();
    void setWindowTitle(const QString &);
    void setLayout(QLayout *);

    int marginLayout() const;
    int spacingLayout() const;
    void setMarginLayout(int);
    void setSpacingLayout(int);

public slots:
    virtual void close();

private:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    Titlebar *titleBar;
    QVBoxLayout *mainLayout;
};
