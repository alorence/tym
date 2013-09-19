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

#include "libraryentry.h"

#include <Logger.h>

LibraryEntry::LibraryEntry() :
    _dirName()
{
    setParent(nullptr);
}

LibraryEntry::LibraryEntry(const QString &dirName, LibraryEntry *parent) :
    _dirName(dirName),
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
    _children.clear();
}

bool LibraryEntry::isDirNode() const
{
    return _record.isEmpty();
}

const QString &LibraryEntry::dirName() const
{
    return _dirName;
}

void LibraryEntry::setDirName(const QString &newDirName)
{
    _dirName = newDirName;
}

const QVariant LibraryEntry::data(Library::GuiIndexes index) const
{
    switch(index) {
    case Library::Name:
        return QFileInfo(_record.value(Library::FilePath).toString()).fileName();
    case Library::StatusMessage:
        return statusMessage(_record.value(Library::Status).toInt(),
                             !_record.value(Library::Bpid).isNull());
    case Library::Results:
        return _record.value(Library::NumResults);
    case Library::Infos:
        return infoMessage(_record.value(Library::Status).toInt(),
                           _record.value(Library::NumResults).toInt(),
                           !_record.value(Library::Bpid).isNull());
    default:
        // TODO: return a message
        return "";
    }
}

const QSqlRecord &LibraryEntry::record() const
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

void LibraryEntry::addChild(LibraryEntry *child)
{
    child->setPosition(_children.size());
    _children.append(child);
}

LibraryEntry *LibraryEntry::child(int index) const
{
    if(index < 0 || index > _children.size()) {
        return nullptr;
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
        return 4;
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

void LibraryEntry::setParent(LibraryEntry *parent)
{
    _parent = parent;
    if(parent != nullptr) {
        _parent->addChild(this);
    }
}

QString LibraryEntry::infoMessage(int statusCode, int numResults, bool hasLinkedResult) const
{
    Library::FileStatus status = (Library::FileStatus)statusCode;
    if(status.testFlag(Library::FileNotFound)) {
        return QCoreApplication::translate("LibraryModel","File not found. Did you moved or rename this file ?");
    } else if (status.testFlag(Library::New)) {
        return QCoreApplication::translate("LibraryModel","Use Search to request Beatport for information on this track.");
    } else {
        if(hasLinkedResult) {
            return QCoreApplication::translate("LibraryModel","Ready.");
        } else if (numResults == 0) {
            return QCoreApplication::translate("LibraryModel","No search result has been found, you can try a manual search. Use search tool after selecting only this track.");
        } else {
            return QCoreApplication::translate("LibraryModel","Please check on the right panel and select the better result for this track.");
        }
    }
}

QString LibraryEntry::statusMessage(int statusCode, bool hasLinkedResult) const
{
    Library::FileStatus status = (Library::FileStatus)statusCode;
    if(status.testFlag(Library::FileNotFound)) {
        return QCoreApplication::translate("LibraryModel","Missing");
    } else if (status.testFlag(Library::New)) {
        return QCoreApplication::translate("LibraryModel","New");
    } else {
        if(hasLinkedResult) {
            return QCoreApplication::translate("LibraryModel","Ok");
        } else {
            return QCoreApplication::translate("LibraryModel","Searched");
        }
    }
}

