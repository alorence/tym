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
    explicit LibraryEntry(const QDir &dir, LibraryEntry *parent = 0);
    explicit LibraryEntry(const QSqlRecord &record, LibraryEntry *parent = 0);
    ~LibraryEntry();

    enum DataIndexes {
        Name = 0,
        Status,
        Results,
        Infos
    };

    bool isDirNode() const;

    const QDir &dir() const;
    void setDir(const QDir &newDir);

    const QVariant data(DataIndexes index) const;
    const QSqlRecord &record();
    void setRecord(const QSqlRecord &record);

    LibraryEntry *parent() const;
    void setParent(LibraryEntry *parent);

    void addChild(LibraryEntry *child);
    LibraryEntry* child(int index);
    QList<LibraryEntry*> children() const;

    int rowCount() const;
    int columnCount() const;

    void setPosition(int position);
    int rowPosition();

private:
    LibraryEntry *_parent;
    QDir _dir;
    QSqlRecord _record;

    QList<LibraryEntry*> _children;
    int _position;
};

#endif // LIBRARYENTRY_H
