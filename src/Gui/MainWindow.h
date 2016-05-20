#pragma once

#include "MusicWindow.h"
#include "RadioWindow.h"
#include "TitleBar.h"

class MainWindow : public Widget
{
    Q_OBJECT
    Q_PROPERTY(int marginLayout READ marginLayout WRITE setMarginLayout)
    Q_PROPERTY(int spacingLayout READ spacingLayout WRITE setSpacingLayout)

public:
    MainWindow(QObject *, QSettings *);
    ~MainWindow();
    void initWindow(Widget *);
    QString &currentMode();

    int marginLayout() const;
    int spacingLayout() const;
    void setMarginLayout(int);
    void setSpacingLayout(int);

public slots:
    void setWindowTitle(const QString &);

private:
    void changeWindow();

private:
    QObject *parentMain;
    Widget *centralWidget;
    Titlebar *titleBar;
    QSettings *iniSettings;
    QVBoxLayout *mainLayout;
};
