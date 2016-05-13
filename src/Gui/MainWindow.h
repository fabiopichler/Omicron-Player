#pragma once

#include "MusicWindow.h"
#include "RadioWindow.h"
#include "TitleBar.h"

class MainWindow : public Widget
{
    Q_OBJECT

public:
    MainWindow(QObject *, QSettings *);
    ~MainWindow();
    void initWindow(Widget *);
    QString &currentMode();

public slots:
    void setWindowTitle(const QString &);

private:
    void changeWindow();

private:
    QObject *parentMain;
    Widget *centralWidget;
    Titlebar *titleBar;
    QSettings *iniSettings;
};
