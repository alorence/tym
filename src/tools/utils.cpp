/*****************************************************************************
Copyright 2012-2013 Antoine Lorence. All right reserved.

This file is part of TYM (Tag Your Music).

TYM (Tag Your Music) is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TYM (Tag Your Music) is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TYM (Tag Your Music). If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
#include <QtGlobal>
#include <QDir>

#include "utils.h"
#include "commons.h"

Utils::Utils(QObject *parent) :
    _infoPix(":/status/info"),
    _warningPix(":/status/warning"),
    _errorPix(":/status/error"),
    _successPix(":/status/success"),
    _infoIcon(_infoPix),
    _warningIcon(_warningPix),
    _errorIcon(_errorPix),
    _successIcon(_successPix)
{
}

QString Utils::formatKey(const QString &classicKey)
{
    QSettings settings;
    Settings::KeyStyle keyStyle =
            (Settings::KeyStyle) settings.value(TYM_PATH_KEY_STYLE, TYM_DEFAULT_KEY_STYLE).toInt();

    if(keyStyle != Settings::MixedInKey) {
        return classicKey;
    }

    // IMPORTANT: Ensure your editor is configured to interpret UTF8 to correctly
    // see and/or modify this method. Ensure you can visualize ♯ (sharp) and ♭ (flat)
    // characters before anything
    if(classicKey == "A♭ minor") return "1A";
    else if(classicKey == "G♯ minor") return "1A";
    else if(classicKey == "E♭ minor") return "2A";
    else if(classicKey == "D♯ minor") return "2A";
    else if(classicKey == "B♭ minor") return "3A";
    else if(classicKey == "A♯ minor") return "3A";
    else if(classicKey == "F minor") return "4A";
    else if(classicKey == "C minor") return "5A";
    else if(classicKey == "G minor") return "6A";
    else if(classicKey == "D minor") return "7A";
    else if(classicKey == "A minor") return "8A";
    else if(classicKey == "E minor") return "9A";
    else if(classicKey == "B minor") return "10A";
    else if(classicKey == "F♯ minor") return "11A";
    else if(classicKey == "D♭ minor") return "12A";
    else if(classicKey == "C♯ minor") return "12A";

    else if(classicKey == "B major") return "1B";
    else if(classicKey == "F♯ major") return "2B";
    else if(classicKey == "D♭ major") return "3B";
    else if(classicKey == "C♯ major") return "3B";
    else if(classicKey == "A♭ major") return "4B";
    else if(classicKey == "G♯ major") return "4B";
    else if(classicKey == "E♭ major") return "5B";
    else if(classicKey == "D♯ major") return "5B";
    else if(classicKey == "B♭ major") return "6B";
    else if(classicKey == "A♯ major") return "6B";
    else if(classicKey == "F major") return "7B";
    else if(classicKey == "C major") return "8B";
    else if(classicKey == "G major") return "9B";
    else if(classicKey == "D major") return "10B";
    else if(classicKey == "A major") return "11B";
    else if(classicKey == "E major") return "12B";
    else return classicKey;
}

QString &Utils::simplifySpaces(QString &subject)
{
    return subject.replace(QRegularExpression("\\s{2,}"), " ");
}

QString &Utils::osFilenameSanitize(QString &fileName)
{
#ifdef Q_OS_WIN
    const QRegularExpression forbiddenChars("[\"\\/:*?<>|]",
                             QRegularExpression::CaseInsensitiveOption);
#elif defined Q_OS_MACX
    const QRegularExpression forbiddenChars("[/:]",
                             QRegularExpression::CaseInsensitiveOption);
#else
    const QRegularExpression forbiddenChars("[/]",
                             QRegularExpression::CaseInsensitiveOption);
#endif
    return fileName.replace(forbiddenChars, " ");
}

QString Utils::volumeName(const QFileInfo &file)
{
#if defined Q_OS_MACX
    int bestResultLength = 0;
    QString bestResult;
    // Compute the list of entries in /Volumes system folder
    QDir volumes("/Volumes");
    for(QFileInfo volume : volumes.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)){
        // We only focus on symbolic inks
        if(volume.isSymLink()) {
            QString symLinkTarget = volume.symLinkTarget();
            // Check the target of symLink, it must be contained in the first
            // chars of given file, and be as long as possible to ensure for
            // example /Users/<user> will be a better result than /
            if(symLinkTarget.size() > bestResultLength
                    && file.absoluteFilePath().startsWith(symLinkTarget)) {
                bestResult = volume.fileName();
                bestResultLength = symLinkTarget.size();
            }
        }
    }
    return bestResult;
#elif defined Q_OS_WIN
    // Take the path of the given file
    QString basePath = file.canonicalPath();
    // Return the content before the first '/' (C:, D:, etc.)
    return basePath.mid(0, basePath.indexOf('/'));
#else
    // Linux is not important, since Traktor Pro does not run on this platform
    return "";
#endif
}

QPixmap Utils::pixForStatusType(Utils::StatusType type)

{
    switch(type){
    case Info:
        return _infoPix;
        break;
    case Warning:
        return _warningPix;
        break;
    case Error:
        return _errorPix;
        break;
    case Success:
        return _successPix;
        break;
    default:
        return QPixmap();
        break;
    }
}

QIcon Utils::iconForStatusType(Utils::StatusType type)
{
    switch(type){
    case Info:
        return _infoIcon;
        break;
    case Warning:
        return _warningIcon;
        break;
    case Error:
        return _errorIcon;
        break;
    case Success:
        return _successIcon;
        break;
    default:
        return QIcon();
        break;
    }
}

void Utils::cleanSomeOldStuff()
{
    QSettings settings;

    // == Introduced in 0.1. Useless since 0.3 ==
    settings.remove("settings/network/beatport/apihost");
}
