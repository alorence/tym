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

#include <QtCore>
#include <QPixmap>
#include <QIcon>

class Utils
{
public:

    /*!
     * \brief Get the unique instance of this class (Singleton design pattern)
     * \return The instance of Utils class
     */
    static Utils *instance();

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

    QPixmap pixForStatusType(StatusType type);
    QIcon iconForStatusType(StatusType type);

private:
    explicit Utils(QObject *parent = 0);

    static Utils * _instance;
    static QMutex _mutex;

    QPixmap _infoPix, _warningPix, _errorPix, _successPix;
    QIcon _infoIcon, _warningIcon, _errorIcon, _successIcon;
};

#endif // UTILS_H
