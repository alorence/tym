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

#include "bpdatabase.h"

#include <Logger.h>

QMutex* BPDatabase::_dbMutex = new QMutex(QMutex::Recursive);

BPDatabase::BPDatabase(QString connectionName, QObject *parent) :
    QObject(parent),
    _dbInitialized(false)
{
    if(QSqlDatabase::contains(connectionName)) {

        _dbObject = QSqlDatabase::database(connectionName);
        _dbInitialized = true;

    } else {
        _dbObject = QSqlDatabase::addDatabase("QSQLITE", connectionName);

        QString dbPath = TYM_DATA_LOCATION + "/default.db3";

        _dbObject.setDatabaseName(dbPath);
        LOG_TRACE(QString("Create connection %1").arg(connectionName));
        _dbObject.open();

        if(connectionName == "defaultConnection"){
            if(version() == "-1") {
                _dbInitialized = initTables();

                if( ! _dbInitialized) {
                    LOG_ERROR(QString("Unable to initialise database."));
                    _dbObject.close();
                    QSqlDatabase::removeDatabase(connectionName);
                    return;
                } else {
                    LOG_TRACE(QString("DB opened and initialized for the first time, version %1").arg(version()));
                    _dbObject.exec("PRAGMA cache_size = 16384;");
                }
            } else {
                _dbInitialized = true;
                _dbObject.exec("PRAGMA cache_size = 16384;");
                LOG_INFO(QString("DB opened, version %1").arg(version()));
            }
        } else {
            _dbInitialized = (version() != "-1");
            if(! _dbInitialized) QSqlDatabase::removeDatabase(connectionName);
        }
    }

    if(! _dbObject.isOpen()){
        LOG_ERROR(QString("Unable to open database: %1")
                  .arg(_dbObject.lastError().text()));
        return;
    }
}

BPDatabase::~BPDatabase()
{
    _dbObject.close();
}

const bool BPDatabase::initialized() const
{
    return _dbInitialized;
}

const QString BPDatabase::version() const
{
    if( ! _dbObject.isOpen()) {
        return "-1";
    }

    QSqlQuery query("SELECT value FROM Infos WHERE key='version'", dbObject());

    QMutexLocker locker(_dbMutex);
    if( ! query.exec()) {
        return "-1";
    } else {
        query.next();
        return query.value(0).toString();
    }
}

QSqlDatabase BPDatabase::dbObject() const
{
    return _dbObject;
}

const QSqlRecord BPDatabase::trackInformations(const QString &bpid) const
{
    QSqlQuery query(dbObject());

    query.prepare("SELECT * FROM TrackFullInfos WHERE bpid=:bpid");
    query.bindValue(":bpid", bpid);

    _dbMutex->lock();
    if( ! query.exec() ) {
        LOG_ERROR(QString("Unable to get track informations: %1").arg(query.lastError().text()));
    }
    _dbMutex->unlock();

    query.next();
    return query.record();
}

const QSqlQuery BPDatabase::tracksInformations(const QStringList &bpids) const
{
    QString queryString = "SELECT * FROM TrackFullInfos";
    if( ! bpids.empty()) {
        queryString.append(" WHERE " + QStringList(bpids).replaceInStrings(QRegExp("^(.*)$"), "bpid=\\1").join(" OR "));
    }

    QSqlQuery query(queryString, dbObject());

    _dbMutex->lock();
    if( ! query.exec() ) {
        LOG_ERROR(QString("Unable to get tracks informations: %1").arg(query.lastError().text()));
    }
    _dbMutex->unlock();

    return query;
}

const QSqlQuery BPDatabase::libraryInformations(const QStringList &uids) const
{
    QString queryString = "SELECT * FROM LibraryHelper";
    if( ! uids.empty()) {
        queryString.append(" WHERE " + QStringList(uids).replaceInStrings(QRegExp("^(.*)$"), "uid=\\1").join(" OR "));
    }

    QSqlQuery query(queryString, dbObject());

    _dbMutex->lock();
    if( ! query.exec() ) {
        LOG_ERROR(QString("Unable to get library informations: %1").arg(query.lastError().text()));
    }
    _dbMutex->unlock();

    return query;
}

const QSqlQuery BPDatabase::resultsForTrack(const QString &libId) const
{
    QSqlQuery query("SELECT tr.* FROM TrackFullInfos as tr JOIN SearchResults as s ON s.trackId=tr.bpid WHERE s.libId="+libId, dbObject());

    _dbMutex->lock();
    if( ! query.exec() ) {
        LOG_ERROR(QString("Unable to get results for library entry %1: %2").arg(libId).arg(query.lastError().text()));
    }
    _dbMutex->unlock();

    return query;
}

void BPDatabase::deleteLibraryEntry(QStringList uids) const
{
    _dbMutex->lock();
    dbObject().transaction();

    QSqlQuery libDeletion(dbObject());
    libDeletion.prepare("DELETE FROM Library WHERE uid=:uid");
    QSqlQuery searchResultsDeletion(dbObject());
    searchResultsDeletion.prepare("DELETE FROM SearchResults WHERE libId=:libId");
    for(QString uid : uids) {
        libDeletion.bindValue(":uid", uid);
        searchResultsDeletion.bindValue(":libId", uid);
        if( ! libDeletion.exec()) {
            LOG_WARNING(QString("Unable to remove tracks from library: %1").arg(libDeletion.lastError().text()));
        }
        if( ! searchResultsDeletion.exec()) {
            LOG_WARNING(QString("Unable to delete search results: %1").arg(searchResultsDeletion.lastError().text()));
        }
    }
    dbObject().commit();
    _dbMutex->unlock();
}

void BPDatabase::deleteSearchResult(const QString &libId, const QString &trackId) const
{
    QSqlQuery delQuery(dbObject());
    delQuery.prepare("DELETE FROM SearchResults WHERE libId=:libId AND trackId=:trackId");
    delQuery.bindValue(":libId", libId);
    delQuery.bindValue(":trackId", trackId);

    QSqlQuery updadeLibQuery(dbObject());
    updadeLibQuery.prepare("UPDATE Library SET bpid=null WHERE uid=:libId and bpid=:trackId");
    updadeLibQuery.bindValue(":libId", libId);
    updadeLibQuery.bindValue(":trackId", trackId);

    _dbMutex->lock();
    if( ! delQuery.exec()) {
        LOG_WARNING(QString("Unable to remove search result %1 / %2: %3").arg(libId).arg(trackId).arg(delQuery.lastError().text()));
    }
    if( ! updadeLibQuery.exec()) {
        LOG_WARNING(QString("Unable to update Library entry %1 to delete default bpid %2: %3").arg(libId).arg(trackId).arg(updadeLibQuery.lastError().text()));
    }
    _dbMutex->unlock();
}

void BPDatabase::renameFile(const QString &oldFilePath, const QString &newFilePath) const
{
    QSqlQuery renameQuery(dbObject());
    renameQuery.prepare("UPDATE OR FAIL Library SET filePath=:new WHERE filePath=:old");
    renameQuery.bindValue(":new", newFilePath);
    renameQuery.bindValue(":old", oldFilePath);

    _dbMutex->lock();
    if( ! renameQuery.exec()) {
        LOG_WARNING(QString("Unable to rename filename %1 in database: %2").arg(oldFilePath).arg(renameQuery.lastError().text()));
    }
    _dbMutex->unlock();
}


void BPDatabase::storeSearchResults(const QString &libUid, const QJsonValue &result) const
{
    _dbMutex->lock();

    QSqlQuery query(dbObject());
    query.prepare("INSERT OR IGNORE INTO SearchResults VALUES (:libId,:trackId)");

    // 1 result for each library row
    if(result.isObject() && ! result.toObject().empty()) {
        QString bpid = storeTrack(result);

        setLibraryTrackReference(libUid, bpid);

        query.bindValue(":libId", libUid);
        query.bindValue(":trackId", bpid);

        if( ! query.exec()) {
            LOG_WARNING(QString("Unable to register search result for track %1: %2").arg(bpid)
                        .arg(query.lastError().text()));
        }

    } else
    // Many results per library row
    if (result.isArray() && ! result.toArray().empty()) {

        for(QJsonValue track : result.toArray()) {
            QString bpid = storeTrack(track);

            query.bindValue(":libId", libUid);
            query.bindValue(":trackId", bpid);
            if( ! query.exec()) {
                LOG_WARNING(QString("Unable to register search result for track %1: %2").arg(bpid)
                            .arg(query.lastError().text()));
            }
        }
    }
    _dbMutex->unlock();

    updateLibraryStatus(libUid, Library::Searched);
}

void BPDatabase::importFile(const QString &path) const
{
    QSqlQuery query(dbObject());
    query.prepare("INSERT OR IGNORE INTO Library (filePath, status) VALUES (:path, :status);");
    query.bindValue(":path", path);
    query.bindValue(":status", Library::New);

    QMutexLocker locker(_dbMutex);
    if( ! query.exec()){
        LOG_WARNING(QString("Unable to import file %1: %2").arg(path).arg(query.lastError().text()));
    }
}

void BPDatabase::importFiles(const QStringList &pathList) const
{
    if(pathList.isEmpty()) return;

    QSqlQuery query(dbObject());
    query.prepare("INSERT OR IGNORE INTO Library (filePath, status) VALUES (:path, :status);");
    query.bindValue(":status", Library::New);

    uint errors = 0;
    _dbMutex->lock();
    dbObject().transaction();

    for(QString path : pathList){
        query.bindValue(":path", path);
        if(!query.exec()){
            LOG_WARNING(QString("Unable to import file: %1").arg(query.lastError().text()));
            ++errors;
        }
    }

    dbObject().commit();
    _dbMutex->unlock();

    if(errors) {
        LOG_WARNING(QString("%1 file(s) have not been added due to database error(s)").arg(errors));
    }
}

void BPDatabase::updateLibraryStatus(const QString &uid, const Library::FileStatus &status) const
{
    QSqlQuery query(dbObject());
    query.prepare("UPDATE OR FAIL Library SET status=:status WHERE uid=:uid");
    query.bindValue(":uid", uid);
    query.bindValue(":status", (int) status);

    _dbMutex->lock();
    if( ! query.exec()) {
        _dbMutex->unlock();
        LOG_WARNING(QString("Unable to update library elements's %1 status: %2").arg(uid)
                    .arg(query.lastError().text()));
        return;
    }
    _dbMutex->unlock();
}

void BPDatabase::setLibraryTrackReference(const QString &libUid, const QString &bpid) const
{
    QSqlQuery query(dbObject());
    query.prepare("UPDATE OR FAIL Library SET bpid=:bpid WHERE uid=:uid");
    query.bindValue(":uid", libUid);
    query.bindValue(":bpid", bpid);

    QMutexLocker locker(_dbMutex);
    if( ! query.exec()) {
        LOG_WARNING(QString("Unable to update library element %1 with the bpid %2: %3")
                    .arg(libUid, bpid)
                    .arg(query.lastError().text()));
    }
}

const bool BPDatabase::initTables() const
{
    QFile initFile(":/sql/init/0.1");

    if(initFile.open(QFile::ReadOnly)) {
        QTextStream in(&initFile);
        QString line;
        QSqlQuery query(dbObject());
        QStringList currentRequest;
        while ( ! in.atEnd()) {
            line = in.readLine().trimmed();

            if (line.isEmpty() || line.startsWith('#')) continue;
            else currentRequest << line;

            if(currentRequest.last().endsWith(';')) {
                _dbMutex->lock();
                query = dbObject().exec(currentRequest.join(" "));
                _dbMutex->unlock();
                if( query.lastError().isValid()) {
                    LOG_ERROR(QString("Unable to execute request: %1 (%2)").arg(currentRequest.join(" ")).arg(query.lastError().text()));
                    return false;
                } else {
                    currentRequest.clear();
                }
            }
        }
        initFile.close();
    } else {
        LOG_ERROR(QString("Unable to open init file: %1").arg(initFile.errorString()));
        return false;
    }
    return true;
}

const QString BPDatabase::storeTrack(const QJsonValue &track) const
{
    QJsonObject trackObject = track.toObject();
    QString trackBpId = trackObject.value("id").toVariant().toString();

    QSqlQuery isExisting(dbObject());
    isExisting.prepare("SELECT bpid, name FROM BPTracks WHERE bpid=:id");
    isExisting.bindValue(":id", trackBpId);

    isExisting.exec();
    if(isExisting.next()) {
        LOG_INFO(QString("Track %1 (%2) already stored in database.").arg(isExisting.record().value(1).toString()).arg(trackBpId));
        return trackBpId;
    }

    QMutexLocker locker(_dbMutex);

    QStringList artists;
    QSqlQuery query(dbObject()), linkQuery(dbObject());
    query.prepare("INSERT OR IGNORE INTO BPArtists VALUES (:bpid,:name)");
    for(QJsonValue artist : trackObject.value("artists").toArray()) {
        QString artistBpId = artist.toObject().value("id").toVariant().toString();

        query.bindValue(":bpid", artistBpId);
        query.bindValue(":name", artist.toObject().value("name").toVariant());
        if( ! query.exec()) {
            LOG_WARNING(QString("Unable to insert the artist %1 into the database: %2")
                          .arg(artist.toObject().value("name").toString())
                          .arg(query.lastError().text()));
        }

        if (artist.toObject().value("type").toString() == "remixer") {
            linkQuery.prepare("INSERT OR IGNORE INTO BPTracksRemixersLink VALUES (:trackId,:artistId)");
        } else {
            linkQuery.prepare("INSERT OR IGNORE INTO BPTracksArtistsLink VALUES (:trackId,:artistId)");
            artists << query.boundValue(":name").toString();
        }
        linkQuery.bindValue(":trackId", trackBpId);
        linkQuery.bindValue(":artistId", artistBpId);
        if( ! linkQuery.exec()) {
            LOG_WARNING(QString("Unable to insert the track <-> artist/remixer link into the database (%1 for track %2)")
                          .arg(query.boundValue(":name").toString(), linkQuery.boundValue(":trackId").toString()));
            LOG_WARNING(linkQuery.lastError().text());
        }
    }

    query.prepare("INSERT OR IGNORE INTO BPGenres VALUES (:bpid,:name)");
    linkQuery.prepare("INSERT OR IGNORE INTO BPTracksGenresLink VALUES (:trackId,:genreId)");
    for(QJsonValue genre : trackObject.value("genres").toArray()) {
        QString genreBpId = genre.toObject().value("id").toVariant().toString();

        query.bindValue(":bpid", genreBpId);
        query.bindValue(":name", genre.toObject().value("name").toString());
        if(!query.exec()) {
            LOG_WARNING(QString("Unable to insert the genre %1 into the database: %2")
                        .arg(query.boundValue(":name").toString())
                        .arg(query.lastError().text()));
        }

        linkQuery.bindValue(":trackId", trackBpId);
        linkQuery.bindValue(":genreId", genreBpId);
        if(!linkQuery.exec()){
            LOG_WARNING(QString("Unable to insert the track <-> genre link into the database (%1 for track %2)")
                        .arg(query.boundValue(":name").toString(), linkQuery.boundValue(":trackId").toString()));
            LOG_WARNING(linkQuery.lastError().text());
        }
    }

    query.prepare("INSERT OR IGNORE into BPTracks "
       "(bpid,name,mixName,title,label,key,bpm,releaseDate,publishDate,price,length,release,image) "
       "VALUES (:bpid,:name,:mixName,:title,:label,:key,:bpm,:releaseDate,:publishDate,:price,:length,:release,:image)");

    query.bindValue(":bpid", trackBpId);
    query.bindValue(":title", trackObject.value("title").toVariant());
    query.bindValue(":name", trackObject.value("name").toVariant());
    query.bindValue(":mixName", trackObject.value("mixName").toVariant());
    query.bindValue(":length", trackObject.value("length").toVariant());
    query.bindValue(":bpm", trackObject.value("bpm").toVariant());
    query.bindValue(":release", trackObject.value("release").toObject().value("name").toVariant());

    QDateTime pubDate(QDate::fromString(trackObject.value("publishDate").toString(), "yyyy-MM-dd"));
    query.bindValue(":publishDate", QVariant(pubDate.toTime_t()));

    QDateTime relDate(QDate::fromString(trackObject.value("releaseDate").toString(), "yyyy-MM-dd"));
    query.bindValue(":releaseDate", QVariant(relDate.toTime_t()));

    QString price = QString::number(trackObject.value("price").toObject().value("value").toDouble() * 0.01, 'f', 2);
    price.append(" ");
    price.append(trackObject.value("price").toObject().value("symbol").toString());
    query.bindValue(":price", price);

    QJsonObject stdKey = trackObject.value("key").toObject().value("standard").toObject();
    QString key = stdKey.value("letter").toString();
    {
        if(stdKey.value("sharp").toBool()) {
            key.append(L'\u266F'); // ♯ is unicode char 266F, or L'\u266F' (in a c++ code)
        }
        if(stdKey.value("flat").toBool()) {
            key.append(L'\u266D'); // ♭ is unicode char 266D, or L'\u266D' (in a c++ code)
        }
        key.append(" ");
        key.append(stdKey.value("chord").toString());
    }
    query.bindValue(":key", key);

    QVariant labelId = trackObject.value("label").toObject().value("id").toVariant();
    {
        QSqlQuery labelQuery(dbObject());
        labelQuery.prepare("INSERT OR IGNORE INTO BPLabels VALUES (:bpid,:name)");
        labelQuery.bindValue(":bpid", labelId);
        labelQuery.bindValue(":name", trackObject.value("label").toObject().value("name").toVariant());
        if( ! labelQuery.exec()){
            LOG_WARNING(QString("Unable to insert the label %1 into database: %2")
                        .arg(labelQuery.boundValue(":name").toString())
                        .arg(labelQuery.lastError().text()));
            labelId = QVariant();
        }
    }
    query.bindValue(":label", labelId);

    QString picUrl = trackObject.value("dynamicImages").toObject().value("main").toObject().value("url").toString();
    int lastSlashPos = picUrl.lastIndexOf('/') + 1;
    QVariant picId = picUrl.mid(lastSlashPos, picUrl.lastIndexOf('.') - lastSlashPos);
    query.bindValue(":image", picId);

    if( ! query.exec()){
        LOG_WARNING(QString("Unable to insert the track %1 - %2 into database: %3")
                    .arg(artists.join(", "), query.boundValue(":title").toString())
                    .arg(query.lastError().text()));
    } else {
        LOG_INFO(QString("Track \"%1 - %2\" stored")
                    .arg(artists.join(", "), query.boundValue(":title").toString()));
    }

    return trackBpId;
}

