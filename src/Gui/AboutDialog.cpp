#include "AboutDialog.hpp"
#include "../Core/Global.h"

#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>
#include <QDebug>

using namespace OmicronTK;

int AboutDialog::s_rand = -1;

AboutDialog::AboutDialog(QWidget *parent)
    : OTKQT::Dialog(parent)
{
    QString imagePath = Global::getQrcPath("Omicron-Player.png");

    QLabel *leftLabel = new QLabel(QString("<div align=\"center\"><img src=\"").append(imagePath).append("\">"
                             "<h1>").append(AppName).append("</h1>"
                             "<h3>Versão: ").append(CurrentVersion).append(" (").append(CurrentReleaseDate).append(")</h3>"
                             "<strong>por Fábio Pichler</strong></div>"));

    if (s_rand == -1)
        s_rand = rand() % 2;

    if ((s_rand = !s_rand))
        leftLabel->setStyleSheet("background: #68217A; padding: 0 30px; color:#FFFFFF;");
    else
        leftLabel->setStyleSheet("background: #EEEEEE; padding: 0 30px; color:#444444;");

    QLabel *rightLabel = new QLabel(
              QString("<div><strong>Copyright © 2015-2019, Fábio Pichler, Todos os direitos reservados.</strong><br><br>"
              "Contato: <a href=\"mailto:software.fabiopichler@outlook.com\">software.fabiopichler@outlook.com</a><br><br>"
              "Visite: <a href=\"http://fabiopichler.net\">http://fabiopichler.net</a><br>"
              "Facebook: <a href=\"").append(PageOnFacebook).append("\">").append(PageOnFacebook).append("</a><br></div>"
              "<div>Este é um programa livre, gratuito e de código aberto,<br>"
              "está licenciado sobre a <a href=\"http://fabiopichler.net/bsd-3-license/\">BSD 3-Clause License</a><br><br>"
              "O ").append(AppName).append(" é um software para reprodução de arquivos de<br>"
              "áudio (MP3, WMA, WAV, OGG, etc.), Web Rádios e CD de música.</div>"
              "<h3>Bibliotecas/Recursos usados</h3>"
              "<div><strong>Qt:</strong> <a href=\"http://www.qt.io\">http://www.qt.io</a>"
              " <i>[Executando com Qt ").append(qVersion()).append(" (Compilado com Qt ").append(QT_VERSION_STR).append(")]</i><br>"
              "<strong>BASS:</strong> <a href=\"http://www.un4seen.com\">http://www.un4seen.com</a><br>"
              "<strong>Lua:</strong> <a href=\"https://www.lua.org\">https://www.lua.org</a><br>"
              "<strong>Ícones:</strong> <a href=\"http://flaticons.net\">http://flaticons.net</a> | "
              "<a href=\"http://findicons.com/search/media\">http://findicons.com/search/media</a></div>"));

    rightLabel->setOpenExternalLinks(true);

    QPushButton *okButton = new QPushButton("Ok");
    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->setSpacing(20);
    rightLayout->setContentsMargins(30, 20, 40, 20);
    rightLayout->addWidget(rightLabel);
    rightLayout->addWidget(okButton, 0, Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(leftLabel);
    layout->addLayout(rightLayout);

    setWindowTitle(QString("Sobre o ").append(AppName));
    setLayout(layout);
}
