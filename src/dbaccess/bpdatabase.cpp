/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
*
* TYM (Tag Your Music) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag Your Music) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag Your Music).  If not, see <http://www.gnu.org/licenses/>.
*/

#include "bpdatabase.h"

BPDatabase * BPDatabase::_instance = 0;

const QString BPDatabase::MAIN_DB = "guiHandle";
const QString BPDatabase::THREAD_DB = "threadHandle";

BPDatabase *BPDatabase::instance()
{
    static QMutex mutex;
    mutex.lock();
    if(!_instance) {
        _instance = new BPDatabase();
    }
    mutex.unlock();
    return _instance;
}

void BPDatabase::deleteInstance()
{
    static QMutex mutex;
    mutex.lock();
    _instance->deleteLater();
    mutex.unlock();
}

BPDatabase::BPDatabase(QObject *parent) :
    QObject(parent),
    dbInitialized(false),
    dbMutex(new QMutex(QMutex::Recursive))
{
    initDB();
}

BPDatabase::~BPDatabase()
{
    dbObject(MAIN_DB).close();
    dbObject(THREAD_DB).close();

    delete dbMutex;
}

bool BPDatabase::initDB()
{
    QString dbPath = Constants::dataLocation() + QDir::separator() + "default.db3";

    QSqlDatabase guiDb = QSqlDatabase::addDatabase("QSQLITE", MAIN_DB);
    guiDb.setDatabaseName(dbPath);

    QSqlDatabase threadDb = QSqlDatabase::cloneDatabase(guiDb, THREAD_DB);

    dbInitialized = guiDb.open() && threadDb.open();
    if( ! dbInitialized) {
        qCritical() << tr("Unable to open database at location %1").arg(dbPath);
    } else if(version() == "-1") {
        dbInitialized = initTables();
        if( ! dbInitialized) {
            qCritical() << tr("Unable to initialise database.");
            guiDb.close();
            threadDb.close();
        } else {
            qDebug() << tr("DB opened and initialized for the first time, version %1").arg(version());
            guiDb.exec("PRAGMA cache_size = 16384;");
            threadDb.exec("PRAGMA cache_size = 16384;");
        }
    } else {
        qDebug() << tr("DB opened, version %1").arg(version());
    }

    return dbInitialized;
}

bool BPDatabase::initialized()
{
    return dbInitialized;
}

bool BPDatabase::initTables()
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
                dbMutex->lock();
                query = dbObject().exec(currentRequest.join(" "));
                dbMutex->unlock();
                if( query.lastError().isValid()) {
                    qCritical() << tr("Unable to execute request : %1 (%2)").arg(currentRequest.join(" ")).arg(query.lastError().text());
                    return false;
                } else {
                    currentRequest.clear();
                }
            }
        }
        initFile.close();
    } else {
        qCritical() << tr("Unable to open init file : %1").arg(initFile.errorString());
        return false;
    }
    return true;
}

QString BPDatabase::version()
{
    if(!initialized()) {
        return "-1";
    }

    QSqlQuery query("SELECT value FROM Infos WHERE key='version'", dbObject());

    QMutexLocker locker(dbMutex);
    if( ! query.exec()) {
        return "-1";
    } else {
        query.next();
        return query.value(0).toString();
    }
}

QSqlDatabase BPDatabase::dbObject(const QString &conName)
{
    return QSqlDatabase::database(conName);
}

QSqlRecord BPDatabase::trackInformations(QVariant &bpid)
{
    QSqlQuery query(dbObject());

    query.prepare("SELECT * from TrackFullInfos WHERE bpid=:bpid");
    query.bindValue(":bpid", bpid);

    dbMutex->lock();
    if( ! query.exec() ) {
        qWarning() << tr("Unable to get track informations : %1").arg(query.lastError().text());
    }
    dbMutex->unlock();

    query.next();
    return query.record();
}

QSqlQuery BPDatabase::tracksInformations(QStringList &bpids)
{
    QString queryString = "SELECT * from TrackFullInfos WHERE " +
            bpids.replaceInStrings(QRegExp("^(.*)$"), "bpid=\\1").join(" OR ");

    QSqlQuery query(queryString, dbObject());

    dbMutex->lock();
    if( ! query.exec() ) {
        qWarning() << tr("Unable to get tracks informations : %1").arg(query.lastError().text());
    }
    dbMutex->unlock();

    return query;
}

void BPDatabase::deleteFromLibrary(QStringList uids)
{
    QString queryLib = "DELETE FROM Library WHERE " +
            uids.replaceInStrings(QRegExp("^(.*)$"), "uid=\\1").join(" OR ");
    QString querySR = "DELETE FROM SearchResults WHERE " +
            uids.replaceInStrings("uid=", "libId=").join(" OR ");

    QSqlQuery delLibQuery(queryLib, dbObject());
    QSqlQuery delSRQuery(querySR, dbObject());
    dbMutex->lock();
    if( ! delLibQuery.exec()) {
        qWarning() << tr("Unable to remove tracks from library : %1").arg(delLibQuery.lastError().text());
    }
    if( ! delSRQuery.exec()) {
        qWarning() << tr("Unable to delete search results : %1").arg(delSRQuery.lastError().text());
    }
    dbMutex->unlock();
}

void BPDatabase::deleteSearchResult(QString libId, QString trackId)
{
    QSqlQuery delQuery(dbObject());
    delQuery.prepare("DELETE FROM SearchResults WHERE libId=:libId AND trackId=:trackId");
    delQuery.bindValue(":libId", libId);
    delQuery.bindValue(":trackId", trackId);

    QSqlQuery updadeLibQuery(dbObject());
    updadeLibQuery.prepare("UPDATE Library SET bpid=null WHERE uid=:libId and bpid=:trackId");
    updadeLibQuery.bindValue(":libId", libId);
    updadeLibQuery.bindValue(":trackId", trackId);

    dbMutex->lock();
    if( ! delQuery.exec()) {
        qWarning() << tr("Unable to remove search result %1 / %2 : %3").arg(libId).arg(trackId).arg(delQuery.lastError().text());
    }
    if( ! updadeLibQuery.exec()) {
        qWarning() << tr("Unable to update Library entry %1 to delete default bpid %2 : %3").arg(libId).arg(trackId).arg(updadeLibQuery.lastError().text());
    }
    dbMutex->unlock();
}

void BPDatabase::renameFile(QString &oldFileName, QString &newFileName)
{
    QSqlQuery renameQuery(dbObject(MAIN_DB));
    renameQuery.prepare("UPDATE OR FAIL Library SET filePath=:new WHERE filePath=:old");
    renameQuery.bindValue(":new", QDir::toNativeSeparators(newFileName));
    renameQuery.bindValue(":old", QDir::toNativeSeparators(oldFileName));

    dbMutex->lock();
    if(renameQuery.exec()) {
        qDebug() << tr("Track %1 renamed into %2").arg(oldFileName).arg((newFileName));
    } else {
        qWarning() << tr("Unable to rename track %1 : %2").arg(oldFileName).arg(renameQuery.lastError().text());
    }
    dbMutex->unlock();
}

QString BPDatabase::storeTrack(const QJsonValue track)
{
    QJsonObject trackObject = track.toObject();
    QString trackBpId = trackObject.value("id").toVariant().toString();

    QSqlQuery isExisting(dbObject());
    isExisting.prepare("SELECT bpid, name FROM BPTracks WHERE bpid=:id");
    isExisting.bindValue(":id", trackBpId);

    isExisting.exec();
    if(isExisting.next()) {
        qDebug() << tr("Track %1 (%2) already stored in database.").arg(isExisting.record().value(1).toString()).arg(trackBpId);
        return trackBpId;
    }

    QMutexLocker locker(dbMutex);

    QStringList artists;
    QSqlQuery query(dbObject()), linkQuery(dbObject());
    query.prepare("INSERT OR IGNORE INTO BPArtists VALUES (:bpid,:name)");
    foreach (QJsonValue artist, trackObject.value("artists").toArray()) {
        QString artistBpId = artist.toObject().value("id").toVariant().toString();

        query.bindValue(":bpid", artistBpId);
        query.bindValue(":name", artist.toObject().value("name").toVariant());
        if( ! query.exec()) {
            qWarning() << tr("Unable to insert the artist %1 into the database")
                          .arg(artist.toObject().value("name").toString());
            qWarning() << query.lastError().text();
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
            qWarning() << tr("Unable to insert the track <-> artist/remixer link into the database (%1 for track %2)")
                          .arg(query.boundValue(":name").toString(), linkQuery.boundValue(":trackId").toString());
            qWarning() << linkQuery.lastError().text();
        }
    }

    query.prepare("INSERT OR IGNORE INTO BPGenres VALUES (:bpid,:name)");
    linkQuery.prepare("INSERT OR IGNORE INTO BPTracksGenresLink VALUES (:trackId,:genreId)");
    foreach (QJsonValue genre, trackObject.value("genres").toArray()) {
        QString genreBpId = genre.toObject().value("id").toVariant().toString();

        query.bindValue(":bpid", genreBpId);
        query.bindValue(":name", genre.toObject().value("name").toString());
        if(!query.exec()) {
            qWarning() << tr("Unable to insert the genre %1 into the database")
                          .arg(query.boundValue(":name").toString());
            qWarning() << query.lastError().text();
        }

        linkQuery.bindValue(":trackId", trackBpId);
        linkQuery.bindValue(":genreId", genreBpId);
        if(!linkQuery.exec()){
            qWarning() << tr("Unable to insert the track <-> genre link into the database (%1 for track %2)")
                          .arg(query.boundValue(":name").toString(), linkQuery.boundValue(":trackId").toString());
            qWarning() << linkQuery.lastError().text();
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
            key.append("♯"); // ♯ is unicode char 266F, or L'\u266F' (in a c++ code)
        }
        if(stdKey.value("flat").toBool()) {
            key.append('♭'); // ♭ is unicode char 266D, or L'\u266D' (in a c++ code)
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
            qWarning() << tr("Unable to insert the label %1 into database")
                        .arg(labelQuery.boundValue(":name").toString());
            qWarning() << labelQuery.lastError().text();
            labelId = QVariant();
        }
    }
    query.bindValue(":label", labelId);

    QString picUrl = trackObject.value("dynamicImages").toObject().value("main").toObject().value("url").toString();
    int lastSlashPos = picUrl.lastIndexOf('/') + 1;
    QVariant picId = picUrl.mid(lastSlashPos, picUrl.lastIndexOf('.') - lastSlashPos);
    query.bindValue(":image", picId);

    if( ! query.exec()){
        qWarning() << tr("Unable to insert the track %1 - %2 into database")
                    .arg(artists.join(", "), query.boundValue(":title").toString());
        qWarning() << query.lastError().text();
    } else {
        qDebug() << tr("Track %1 - %2 has been correctly stored into database").arg(artists.join(", "), query.boundValue(":title").toString());
    }

    return trackBpId;
}

bool BPDatabase::setLibraryTrackReference(QString libUid, QString bpid)
{
    // FIXME : this method can be called from storeSearchResults() or manually from
    // on_actionSetDefaultResult_triggered. These methods are not executed from the same
    // thread. The right dbObject() need to be used in each case.

    QSqlQuery query(dbObject());
    query.prepare("UPDATE OR FAIL Library SET bpid=:bpid WHERE uid=:uid");
    query.bindValue(":uid", libUid);
    query.bindValue(":bpid", bpid);

    dbMutex->lock();
    if( ! query.exec()) {
        dbMutex->unlock();
        qWarning() << tr("Unable to update library element %1 with the bpid %2")
                      .arg(libUid, bpid);
        qWarning() << query.lastError().text();
        return false;
    } else {
        dbMutex->unlock();
        emit referenceForTrackUpdated(libUid);
        return true;
    }
}

void BPDatabase::storeSearchResults(QString libUid, QJsonValue result)
{
    dbMutex->lock();
    dbObject().exec("BEGIN TRANSACTION");

    QSqlQuery query(dbObject());
    query.prepare("INSERT OR IGNORE INTO SearchResults VALUES (:libId,:trackId)");

    // 1 result for each library row
    if(result.isObject() && ! result.toObject().empty()) {
        QString bpid = storeTrack(result);

        setLibraryTrackReference(libUid, bpid);

        query.bindValue(":libId", libUid);
        query.bindValue(":trackId", bpid);

        if( ! query.exec()) {
            qWarning() << tr("Unable to register search result for track %1").arg(bpid);
            qWarning() << query.lastError().text();
        }

    } else
    // Many results per library row
    if (result.isArray() && ! result.toArray().empty()) {

        foreach(QJsonValue track, result.toArray()) {
            QString bpid = storeTrack(track);

            query.bindValue(":libId", libUid);
            query.bindValue(":trackId", bpid);
            if( ! query.exec()) {
                qWarning() << tr("Unable to register search result for track %1").arg(bpid);
                qWarning() << query.lastError().text();
            }
        }
    }
    dbObject().exec("COMMIT");
    dbMutex->unlock();

    updateLibraryStatus(libUid, Library::ResultsAvailable);
}


void BPDatabase::updateLibraryStatus(QString uid, Library::FileStatus status)
{
    QSqlQuery query(dbObject());
    query.prepare("UPDATE OR FAIL Library SET status=:status WHERE uid=:uid");
    query.bindValue(":uid", uid);
    query.bindValue(":status", status);

    dbMutex->lock();
    if( ! query.exec()) {
        dbMutex->unlock();
        qWarning() << tr("Unable to update library elements's %1 status").arg(uid);
        qWarning() << query.lastError().text();
        return;
    }
    dbMutex->unlock();
    emit libraryEntryUpdated(uid);
}

void BPDatabase::importFiles(const QStringList &pathList)
{
    QString baseQuery = "INSERT OR IGNORE INTO Library (filePath, status) ";
    QString value = QString("SELECT '%2', %1").arg(Library::New);

    QStringList values;
    foreach(QString path, pathList){
        values << value.arg(QDir::toNativeSeparators(path).replace("'", "''"));
    }
    dbMutex->lock();
    QSqlQuery query = dbObject().exec(baseQuery.append(values.join(" UNION ")).append(";"));
    dbMutex->unlock();
    if(query.lastError().isValid()){
        qWarning() << tr("Unable to import files : %2").arg(query.lastError().text());
    } else {
        emit libraryEntryUpdated(query.lastInsertId().toString());
    }
}

void BPDatabase::importFile(QString path)
{
    QSqlQuery query(dbObject());
    query.prepare("INSERT OR IGNORE INTO Library (filePath, status) VALUES (:path, :status);");
    query.bindValue(":path", QDir::toNativeSeparators(path));
    query.bindValue(":status", Library::New);

    dbMutex->lock();
    if( ! query.exec()){
        dbMutex->unlock();
        qWarning() << tr("Unable to import file %1 : %2").arg(path).arg(query.lastError().text());
    } else {
        dbMutex->unlock();
        emit libraryEntryUpdated(query.lastInsertId().toString());
    }
}
