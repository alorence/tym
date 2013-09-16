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

#ifndef UTILS_H
#define UTILS_H

#include <QPixmap>
#include <QIcon>
#include <QFileInfo>

#include "interfaces/genericsingleton.h"

//NOTE: Missing doc on some methods

class Utils : public GenericSingleton<Utils>
{
    friend Utils* GenericSingleton<Utils>::instance();
    friend void GenericSingleton<Utils>::deleteInstance();

public:
    enum StatusType {
        Info,
        Warning,
        Error,
        Success
    };

    /*!
     * \brief Return the key corresponding to given classicKey, in the good
     * format, according to the one choosen in settings
     * \param classicKey The original key, in the format used in database
     * \return The formatted key, as string
     */
    QString formatKey(const QString &classicKey);
    /*!
     * \brief Modify subject replacing multiple spaces into only one
     * \param subject The string to sanitize
     * \return
     */
    QString &simplifySpaces(QString &subject);
    /*!
     * \brief Modify given fileName to remove characters unsupported on current
     * OS, regarding to file system used.
     * \param fileName The fileName to sanitize
     * \return
     */
    QString &osFilenameSanitize(QString &fileName);

    /*!
     * \brief Get the volume name where given file is stored
     * It returns a string like "Macintosh HD" on OSX and like "D:" or "C:"
     * on Windows
     * \param file A file on the disk
     * \return
     */
    QString volumeName(const QFileInfo &file);

    /**
     * @brief Return a picture corresponding to the given status type
     * @param type
     * @return
     */
    QPixmap pixForStatusType(StatusType type);
    /**
     * @brief Return an icon corresponding to the given status type
     * @param type
     * @return
     */
    QIcon iconForStatusType(StatusType type);

    /**
     * @brief Ensure that all artifacts (settings key/values, db entries/tables, etc.) are cleaned
     * Remove all stuff that was introduced in previous version and not needed anymore, but
     * still stored on the user computer
     */
    void cleanSomeOldStuff();

private:
    explicit Utils(QObject *parent = 0);
    ~Utils(){}

    QPixmap _infoPix, _warningPix, _errorPix, _successPix;
    QIcon _infoIcon, _warningIcon, _errorIcon, _successIcon;
};

#endif // UTILS_H
