/*******************************************************************************

  Omicron Player Classic

  Author: Fábio Pichler
  Website: http://fabiopichler.net
  License: BSD 3-Clause License

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of Omicron Player Classic nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

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
              "<a href=\"http://findicons.com\">http://findicons.com</a></div>"));

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
