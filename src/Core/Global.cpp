/*******************************************************************************
  Omicron Player Classic

  Copyright (c) 2015-2019, Fábio Pichler
  All rights reserved.

  License: BSD 3-Clause License (http://fabiopichler.net/bsd-3-license/)
  Author: Fábio Pichler
  Website: http://fabiopichler.net

*******************************************************************************/

#include "Global.h"
#include <QDir>
#include <QDirIterator>
#include <bass.h>

int Global::argc;
bool Global::inDevelopment = true;
std::vector<int> Global::equalizerValues;
QList<QStringList> Global::supportedFiles;

QString Global::configPath;
QString Global::appPath;
QString Global::qrcPath;
QString Global::themePath;
QString Global::radiolistPath;

bool Global::init(const int &_argc)
{
    argc = _argc;
    appPath = QApplication::applicationDirPath();
    configPath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);

    if (appPath.isEmpty() || configPath.isEmpty())
        return false;

    appPath += "/";
    configPath += "/" + AppConfigDir + "/";
    setupSupportedFiles();

#ifdef QT_NO_DEBUG // RELEASE
    qrcPath = ":/";
#else // DEBUG
    qrcPath = appPath + "../../project/qrc/";
#endif // QT_NO_DEBUG

    return true;
}

void Global::setupSupportedFiles()
{
    supportedFiles += {PlaylistExt,  AppName + " Playlist"};
    supportedFiles += {"mp3",  "MPEG-1/2 Audio Layer 3"};
    supportedFiles += {"wma",  "Windows Media Audio"};
    supportedFiles += {"ogg",  "OGG Audio"};
    supportedFiles += {"wav",  "Waveform Audio Format"};
    supportedFiles += {"aac",  "Advanced Audio Coding"};
    supportedFiles += {"mp2",  "MPEG-1/2 Audio Layer 2"};
    supportedFiles += {"mp1",  "MPEG-1 Audio Layer 1"};
    supportedFiles += {"ac3",  "Dolby Digital Audio"};
    supportedFiles += {"m4a",  "MPEG-4 Audio"};
    supportedFiles += {"m4r",  "MPEG-4 Audio"};
    supportedFiles += {"aiff", "Audio Interchange File Format"};
    supportedFiles += {"aif",  "Audio Interchange File Format"};
    supportedFiles += {"flac", "Free Lossless Audio Codec"};
    supportedFiles += {"ape",  "Monkey's Audio"};
    supportedFiles += {"wv",   "WavPack Audio"};
    supportedFiles += {"opus", "Opus Audio Codec"};
    supportedFiles += {"mo3",  "MO3 mod format"};
    supportedFiles += {"xm",   "Fast Tracker 2"};
    supportedFiles += {"mod",  "MOD Audio"};
    supportedFiles += {"s3m",  "Scream Tracker 3"};
    supportedFiles += {"it",   "Impulse Tracker"};
    supportedFiles += {"mtm",  "MultiTracker"};
    supportedFiles += {"umx",  "U-MYX Audio"};
}

QString Global::getConfigPath(const QString &arg)
{
    return configPath + arg;
}

QString Global::getAppPath(const QString &arg)
{
    return appPath + arg;
}

QString Global::getQrcPath(const QString &arg)
{
    return qrcPath + arg;
}

QString Global::getRadiolistPath()
{
    return radiolistPath;
}

QString Global::getThemePath(const QString &arg)
{
    return themePath + arg;
}

QString Global::getErrorHtml(const QString &arg, const QString &arg2, int code)
{
    if (code == -2)
        code = BASS_ErrorGetCode();

    QString text((arg.isEmpty() ? ""
              : "<div style=\"font-weight:bold;margin-bottom:7px;\">" + arg + "</div>")
                    + "<div><strong>Código do erro:</strong> "+QString::number(code)
                    + "</div><div style=\"margin-bottom:7px;\"><strong>Mensagem:</strong> ");

    text += getErrorText("", "", code);

    return text + "</div>" + (arg2.isEmpty() ? "" : "<div>" + arg2 + "</div>");
}

QString Global::getErrorText(const QString &arg, const QString &arg2, int code)
{
    if (code == -2)
        code = BASS_ErrorGetCode();

    QString text(arg.isEmpty() ? "" : arg+"\nCode: "+QString::number(code)+", ");

    switch(code)
    {
        case 0:  text += "All is OK"; break;
        case 1:  text += "Memory error"; break;
        case 2:  text += "Can't open the file"; break;
        case 3:  text += "Can't find a free/valid driver"; break;
        case 4:  text += "The sample buffer was lost"; break;
        case 5:  text += "Invalid handle"; break;
        case 6:  text += "Unsupported sample format"; break;
        case 7:  text += "Invalid playback position"; break;
        case 8:  text += "BASS_Init has not been successfully called"; break;
        case 9:  text += "BASS_Start has not been successfully called"; break;
        case 10: text += "SSL/HTTPS support isn't available"; break;
        case 12: text += "No CD in drive"; break;
        case 13: text += "Invalid track number"; break;
        case 14: text += "Already initialized/paused/whatever"; break;
        case 16: text += "Not paused"; break;
        case 17: text += "Not an audio track"; break;
        case 18: text += "Can't get a free channel"; break;
        case 19: text += "An illegal type was specified"; break;
        case 20: text += "An illegal parameter was specified"; break;
        case 21: text += "No 3D support"; break;
        case 22: text += "No EAX support"; break;
        case 23: text += "Illegal device number"; break;
        case 24: text += "Not playing"; break;
        case 25: text += "Illegal sample rate"; break;
        case 27: text += "The stream is not a file stream"; break;
        case 29: text += "No hardware voices available"; break;
        case 31: text += "The MOD music has no sequence data"; break;
        case 32: text += "No internet connection could be opened"; break;
        case 33: text += "Couldn't create the file"; break;
        case 34: text += "Effects are not available"; break;
        case 35: text += "The channel is playing"; break;
        case 37: text += "Requested data is not available"; break;
        case 38: text += "The channel is a 'decoding channel'"; break;
        case 39: text += "A sufficient DirectX version is not installed"; break;
        case 40: text += "Connection timedout"; break;
        case 41: text += "Unsupported file format"; break;
        case 42: text += "Unavailable speaker"; break;
        case 43: text += "Invalid BASS version (used by add-ons)"; break;
        case 44: text += "Codec is not available/supported"; break;
        case 45: text += "The channel/file has ended"; break;
        case 46: text += "The device is busy (eg. in \"exclusive\" use by another process)"; break;
        case -1: text += "Some other mystery error"; break;
        default: text  = "Erro indefinido"; break;
    }

    return text + (arg2.isEmpty() ? "" : " "+arg2);
}

QString Global::regExp(const QString &arg, const QString &arg2)
{
    QRegExp regExp(arg2);
    if (regExp.indexIn(arg) > -1)
        return regExp.cap(1);
    return QString();
}

QString Global::cStrToQString(const char *cStr)
{
    QString text(cStr);

    for (int i = 0; i < text.length(); i++)
    {
        if (text.at(i).unicode() == 65533)
        {
            text = QString::fromLatin1(cStr);
            break;
        }
    }

    return text;
}

void Global::setRadiolistName(const QString &arg)
{
    radiolistPath = configPath+arg;
}

void Global::setThemePath(const QString &path)
{
    themePath = path;
}
