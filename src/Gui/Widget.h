
#pragma once

#include <QWidget>
#include <QComboBox>
#include <QSlider>

//================================================================================================================
// class Widget
//================================================================================================================

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    QString mode;

private:
    void paintEvent(QPaintEvent *);
};

//================================================================================================================
// class ComboBox
//================================================================================================================

class ComboBox : public QComboBox
{
public:
    ComboBox();
};

//================================================================================================================
// class Slider
//================================================================================================================

class Slider : public QSlider
{
public:
    Slider(Qt::Orientation, QWidget *parent = nullptr);

private:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    bool sendEvent;
};

