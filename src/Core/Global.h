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

#pragma once

#include "../Version.h"
#include <qstring.h>
#include <qobject.h>
#include <QApplication>
#include <qstandardpaths.h>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#define Information(parent, text) QMessageBox::information(parent, "Informação", text)
#define Warning(parent, text) QMessageBox::warning(parent, "Aviso", text)

#ifdef Q_OS_WIN
    #define strncasecmp _strnicmp
    #define stdCout(str) std::wcout << QString(str).toStdWString() << std::endl
    #define ToQString(str) QString::fromLocal8Bit(str)
#else
    #define _strdup strdup
    #define stdCout(str) std::cout << QString(str).toStdString() << std::endl
    #define ToQString(str) QString::fromUtf8(str)
#endif

enum
{
    DIALOG
};

const QString AppName = APP_NAME,
              CurrentVersion = PRODUCT_VERSION_STR,
              CurrentReleaseDate = CURRENT_RELEASE_DATE,
              FileRadiolist = FILE_RADIOLIST,
              FileRadiolistDate = FILE_RADIOLIST_DATE,
              AppConfigDir = TARGET,
              AppNameId = "Omicron-Player-Classic",
              PlaylistExt = "fpmp",
              RadioPlaylistExt = "fprp",

#ifdef Q_OS_WIN
              FileNameFilter = "*.mp3 *.wma *.ogg *.wav *.aac *.mp2 *.mp1 *.ac3 *.m4a *.m4r *.aiff *.aif *.flac *.ape *.wv *.opus *.mo3 *.xm *.mod *.s3m *.it *.mtm *.umx",
              bass_aac = "bass_aac.dll",
              bass_ac3 = "bass_ac3.dll",
              bass_ape = "bass_ape.dll",
              bassflac = "bassflac.dll",
              basswma  = "basswma.dll",
              basswv   = "basswv.dll",
              bassopus = "bassopus.dll",
#elif defined(Q_OS_LINUX)
              FileNameFilter = "*.mp3 *.ogg *.wav *.aac *.mp2 *.mp1 *.ac3 *.m4a *.m4r *.aiff *.aif *.flac *.ape *.wv *.opus *.mo3 *.xm *.mod *.s3m *.it *.mtm *.umx",
              bass_aac = "libbass_aac.so",
              bass_ac3 = "libbass_ac3.so",
              bass_ape = "libbass_ape.so",
              bassflac = "libbassflac.so",
              basswma  = "",
              basswv   = "libbasswv.so",
              bassopus = "libbassopus.so",
#else
              FileNameFilter = "",
              bass_aac = "",
              bass_ac3 = "",
              bass_ape = "",
              bassflac = "",
              basswma  = "",
              basswv   = "",
              bassopus = "",
#endif // Q_OS_WIN

              CheckUpdate = "http://software.fabiopichler.net/updates/omicron-player-classic/check/",
              OfficialSite = OFFICIAL_WEBSITE,
              PageOnFacebook = "https://www.facebook.com/fabiopichler.net",
              DownloadUpdate = "https://fabiopichler.net/download/omicron-player/";

class Global
{
public:
    static bool initAppPath();
    static bool init(const int &);
    static void setupSupportedFiles();

    static QString getConfigPath(const QString &arg = QString());
    static QString getAppPath(const QString &arg = QString());
    static QString getQrcPath(const QString &arg = QString());
    static QString getRadiolistPath();
    static QString getErrorHtml(const QString &arg = QString(), const QString &arg2 = QString(), int = -2);
    static QString getErrorText(const QString &arg = QString(), const QString &arg2 = QString(), int = -2);

    static QString regExp(const QString &, const QString &);
    static QString cStrToQString(const char *);

    static void setRadiolistName(const QString &);

    static int argc;
    static std::vector<int> equalizerValues;
    static QList<QStringList> supportedFiles;

private:
    static QString configPath, appPath, qrcPath, radiolistPath;
};
