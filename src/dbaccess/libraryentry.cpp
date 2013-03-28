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

#include "libraryentry.h"

#include <Logger.h>
#include "commons.h"

LibraryEntry::LibraryEntry(const QDir &dir, LibraryEntry *parent) :
    _dir(dir),
    _position(0)
{
    setParent(parent);
}

LibraryEntry::LibraryEntry(const QSqlRecord &record, LibraryEntry *parent) :
    _record(record),
    _position(0)
{
    setParent(parent);
}

LibraryEntry::~LibraryEntry()
{
    qDeleteAll(_children);
}

bool LibraryEntry::isDirNode() const
{
    return _record.isEmpty();
}

const QDir &LibraryEntry::dir() const
{
    return _dir;
}

void LibraryEntry::setDir(const QDir &newDir)
{
    _dir = newDir;
}

const QVariant LibraryEntry::data(DataIndexes index) const
{
    switch(index) {
    case Name:
        return QFileInfo(_record.value(Library::FilePath).toString()).fileName();
    case Status:
        return _record.value(Library::Status);
    case Results:
        return _record.value(Library::NumResults);
    case Infos:
    default:
        return "";
    }
}

const QSqlRecord &LibraryEntry::record()
{
    return _record;
}

void LibraryEntry::setRecord(const QSqlRecord &record)
{
    _record = record;
}

LibraryEntry *LibraryEntry::parent() const
{
    return _parent;
}

void LibraryEntry::setParent(LibraryEntry *parent)
{
    _parent = parent;
    if(parent != NULL) {
        _parent->addChild(this);
    }
}

void LibraryEntry::addChild(LibraryEntry *child)
{
    child->setPosition(_children.size());
    _children.append(child);
}

LibraryEntry *LibraryEntry::child(int index)
{
    if(index < 0 || index > _children.size()) {
        return NULL;
    } else {
        return _children[index];
    }
}

QList<LibraryEntry *> LibraryEntry::children() const
{
    return _children;
}

int LibraryEntry::rowCount() const
{
    return _children.size();
}

int LibraryEntry::columnCount() const
{
    if(isDirNode()) {
        return 1;
    } else {
        return _record.count();
    }
}

void LibraryEntry::setPosition(int position)
{
    _position = position;
}

int LibraryEntry::rowPosition()
{
    return _position;
}

