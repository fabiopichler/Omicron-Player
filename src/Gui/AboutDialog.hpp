#pragma once

#include <OmicronTK11/Qt/Dialog.hpp>

class AboutDialog : public OmicronTK11::OTKQT::Dialog
{
public:
    AboutDialog(QWidget *parent = nullptr);

private:
    static int s_rand;
};
