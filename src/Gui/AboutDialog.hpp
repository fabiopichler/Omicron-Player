#pragma once

#include "DialogBase.h"

class AboutDialog : public DialogBase
{
public:
    AboutDialog(QWidget *parent = nullptr);

private:
    static int s_rand;
};
