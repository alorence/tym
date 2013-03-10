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

#ifndef LIBRARYENTRY_H
#define LIBRARYENTRY_H

#include <QObject>
#include <QtSql>

class LibraryEntry
{
public:
    explicit LibraryEntry(const QString &dir, LibraryEntry *parent = 0);
    ~LibraryEntry();

    const QDir &dir() const;
    void setDirPath(const QString &newDirPath);

    void setParent(LibraryEntry *parent);

    bool isDir(int child) const;

    void addChild(LibraryEntry *childDir);
    void addChild(const QSqlRecord child);

    void* child(int index);
    int rowCount() const;

private:
    LibraryEntry *_parent;
    QDir _dir;

    QList<LibraryEntry*> _childDirs;
    QList<QSqlRecord> _children;
};

#endif // LIBRARYENTRY_H
