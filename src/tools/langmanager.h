/*****************************************************************************
Copyright 2013 Antoine Lorence. All right reserved.

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

#ifndef LANGMANAGER_H
#define LANGMANAGER_H

#include <QtCore>

class LangManager : public QObject
{
    Q_OBJECT
public:

    static LangManager *instance();
    static void destroy();

    QMap<QString, QTranslator*> translationsFiles() const;
    bool updateTranslation(const QString &lang) const;

private:
    explicit LangManager(QObject *parent = 0);
    QMap<QString, QTranslator*> _qmFiles;
    QTranslator* _currentTranslator;

    static LangManager *_instance;
    static QMutex _mutex;
};

#endif // LANGMANAGER_H
