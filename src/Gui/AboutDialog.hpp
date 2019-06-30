#pragma once

#include <OmicronTK/Qt/Dialog.hpp>

class AboutDialog : public OmicronTK::OTKQT::Dialog
{
public:
    AboutDialog(QWidget *parent = nullptr);

private:
    static int s_rand;
};
