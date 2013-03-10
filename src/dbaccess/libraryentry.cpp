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

LibraryEntry::LibraryEntry(const QString &dir, LibraryEntry *parent) :
    _dir(dir)
{
    setParent(parent);
}

LibraryEntry::~LibraryEntry()
{
    qDeleteAll(_childDirs);
}

const QDir &LibraryEntry::dir() const
{
    return _dir;
}

void LibraryEntry::setDirPath(const QString &newDirPath)
{
    _dir.setCurrent(newDirPath);
}

void LibraryEntry::setParent(LibraryEntry *parent)
{
    _parent = parent;
    if(parent != NULL) {
        _parent->addChild(this);
    }
}

bool LibraryEntry::isDir(int child) const
{
    return child < _childDirs.size();
}

void LibraryEntry::addChild(LibraryEntry *childDir)
{
    _childDirs.append(childDir);
}

void LibraryEntry::addChild(const QSqlRecord child)
{
    _children.append(child);
}

void *LibraryEntry::child(int index)
{
    if(index < 0 || index > _childDirs.size() + _children.size() - 1) {
        return NULL;
    } else if(index < _childDirs.size()) {
        return _childDirs[index];
    } else {
        return &_children[index];
    }
}

int LibraryEntry::rowCount() const
{
    return _childDirs.size() + _children.size();
}

