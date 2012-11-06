/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
*
* TYM (Tag your Library) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag your Library) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag your Library).  If not, see <http://www.gnu.org/licenses/>.
*/

#include "bpdatabase.h"

BPDatabase::BPDatabase(QObject *parent) :
    QObject(parent)
{
    if( ! BPDatabase::initialized()) {
        BPDatabase::initDB();
    }

    _libraryModel = new LibraryModel(this, dbObject());
    _libraryModel->setTable("Library");
    _libraryModel->select();

    _searchModel = new QSqlQueryModel(this);

    _searchQuery = QSqlQuery(BPDatabase::dbObject());
    _searchQuery.prepare("SELECT "
                         "tr.bpid as ID, "
                         "(group_concat(a.name, ', ') || ' - ' || tr.title) as Track "
                         "FROM BPTracks as tr "
                         "JOIN SearchResults as sr ON tr.bpid = sr.trackId "
                         "JOIN BPTracksArtistsLink as talink ON talink.trackId = sr.trackId "
                         "JOIN BPArtists as a ON a.bpid = talink.artistId "
                         "WHERE sr.libId=:id GROUP BY tr.bpid");

}

BPDatabase::~BPDatabase()
{
    dbObject().close();
}

bool BPDatabase::initDB()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation)
                    + QDir::separator()
                    + "default.db";

    QFileInfo dbFileInfo(dbPath);
    if( ! dbFileInfo.dir().exists()) {
        QDir dir;
        dir.mkpath(dbFileInfo.dir().path());
    }

    db.setDatabaseName(dbPath);
    qDebug() << dbPath;

    dbInitialized = db.open();
    if( ! dbInitialized) {
        qCritical() << tr("Unable to open database at location %1").arg(dbPath);
    } else if(BPDatabase::version() == "-1") {
        dbInitialized = BPDatabase::initTables();
        if( ! dbInitialized) {
            qCritical() << tr("Unable to initialise database.");
            //QFile(dbPath).remove();
        } else {
            qDebug() << tr("DB opened and initialized for the first time, version %1").arg(version());
        }
    } else {
        qDebug() << tr("DB opened, version %1").arg(version());
    }

    return dbInitialized;
}

const bool BPDatabase::initialized()
{
    return dbInitialized;
}

bool BPDatabase::initTables()
{
    QStringList sqlInitCommands;
    sqlInitCommands <<  "BEGIN TRANSACTION;" <<
                        "PRAGMA foreign_keys = ON;" <<

                        "CREATE TABLE Infos (key TEXT UNIQUE, value TEXT);" <<
                        "INSERT INTO Infos (key, value) VALUES ('version', '0.1');" <<

                        //Main library table
                        "CREATE TABLE Library (uid INTEGER PRIMARY KEY, filePath TEXT, bpid INTEGER REFERENCES BPTracks(bpid), status TEXT);" <<

                        // Main BeatPort infos tables
                        "CREATE TABLE BPTracks  (bpid INTEGER PRIMARY KEY, name TEXT, mixName TEXT, title TEXT, label INTEGER REFERENCES BPLabels(bpid), key TEXT, bpm TEXT, releaseDate INTEGER, publishDate INTEGER, price TEXT, length TEXT, release TEXT, imageUrl TEXT, imagePath TEXT);" <<
                        "CREATE TABLE BPArtists (bpid INTEGER PRIMARY KEY, name TEXT);" <<
                        "CREATE TABLE BPGenres  (bpid INTEGER PRIMARY KEY, name TEXT);" <<
                        "CREATE TABLE BPLabels  (bpid INTEGER PRIMARY KEY, name TEXT);" <<

                        "CREATE TABLE BPTracksArtistsLink (trackId INTEGER REFERENCES BPTracks(bpid), artistId INTEGER REFERENCES BPArtists(bpid), PRIMARY KEY(trackId, artistId));" <<
                        "CREATE TABLE BPTracksRemixersLink (trackId INTEGER REFERENCES BPTracks(bpid), artistId INTEGER REFERENCES BPArtists(bpid), PRIMARY KEY(trackId, artistId));" <<
                        "CREATE TABLE BPTracksGenresLink (trackId INTEGER REFERENCES BPTracks(bpid), genreId INTEGER REFERENCES BPGenres(bpid), PRIMARY KEY(trackId, genreId));" <<
                        "CREATE TABLE SearchResults (libId INTEGER REFERENCES Library(uid), trackId INTEGER REFERENCES BPTracks(bpid), PRIMARY KEY(libId, trackId));" <<

                       "COMMIT;";

    foreach(QString line, sqlInitCommands) {
        QSqlQuery query = dbObject().exec(line);
        if( query.lastError().isValid()) {
            qCritical() << tr("Unable to execute line : %1 (%2)").arg(line).arg(query.lastError().text());
            return false;
        }
    }
    return true;
}

const QString BPDatabase::version()
{
    QSqlQuery query("SELECT value FROM Infos WHERE key='version'", BPDatabase::dbObject());
    if( ! query.exec()) {
        return "-1";
    } else {
        query.next();
        return query.value(0).toString();
    }
}

QSqlDatabase BPDatabase::dbObject()
{
    return QSqlDatabase::database();
}

LibraryModel *BPDatabase::libraryModel() const
{
    return _libraryModel;
}

QSqlQueryModel *BPDatabase::searchModel() const
{
    return _searchModel;
}

void BPDatabase::updateSearchResults(const QModelIndex & selected, const QModelIndex &)
{
    if(selected.isValid()) {
        _searchQuery.bindValue(":id", _libraryModel->data(_libraryModel->index(selected.row(), LibraryIndexes::Uid)));
        _searchQuery.exec();
        _searchModel->setQuery(_searchQuery);
    } else {
        _searchQuery.bindValue(":id", -1);
        _searchQuery.exec();
        _searchModel->setQuery(_searchQuery);
    }
}

QVariant BPDatabase::storeTrack(const QVariant track)
{
    QMap<QString, QVariant> trackMap = track.toMap();
    QVariant trackBpId = trackMap.value("id");

    QSqlQuery isExisting(BPDatabase::dbObject());
    isExisting.prepare("SELECT bpid, name FROM BPTracks WHERE bpid=:id");
    isExisting.bindValue(":id", trackBpId);
    isExisting.exec();
    if(isExisting.next()) {
        qDebug() << QString(tr("Track %1 (%2) already stored in database.")).arg(isExisting.record().value(1).toString()).arg(trackBpId.toString());
        return trackBpId;
    }

    QStringList artists;
    QSqlQuery query(BPDatabase::dbObject()), linkQuery(BPDatabase::dbObject());
    query.prepare("INSERT OR IGNORE INTO BPArtists VALUES (:bpid,:name)");
    foreach (QVariant artist, trackMap.value("artists").toList()) {
        QVariant artistBpId = artist.toMap().value("id");

        query.bindValue(":bpid", artistBpId);
        query.bindValue(":name", artist.toMap().value("name"));
        if( ! query.exec()) {
            qWarning() << QString("Unable to insert the artist %1 into the database")
                          .arg(artist.toMap().value("name").toString());
            qWarning() << query.lastError();
        }

        if ( artist.toMap().value("type").toString() == "remixer") {
            linkQuery.prepare("INSERT OR IGNORE INTO BPTracksRemixersLink VALUES (:trackId,:artistId)");
        } else {
            linkQuery.prepare("INSERT OR IGNORE INTO BPTracksArtistsLink VALUES (:trackId,:artistId)");
            artists << query.boundValue(":name").toString();
        }
        linkQuery.bindValue(":trackId", trackBpId);
        linkQuery.bindValue(":artistId", artistBpId);
        if( ! linkQuery.exec()) {
            qWarning() << QString("Unable to insert the artist/remixer <-> track link into the database (%1 for track %2)")
                          .arg(query.boundValue(":name").toString(), linkQuery.boundValue(":trackId").toString());
            qWarning() << linkQuery.lastError();
        }
    }

    query.prepare("INSERT OR IGNORE INTO BPGenres VALUES (:bpid,:name)");
    linkQuery.prepare("INSERT OR IGNORE INTO BPTracksGenresLink VALUES (:trackId,:genreId)");
    foreach (QVariant genre, trackMap.value("genres").toList()) {
        QVariant genreBpId = genre.toMap().value("id");

        query.bindValue(":bpid", genreBpId);
        query.bindValue(":name", genre.toMap().value("name"));
        if(!query.exec()) {
            qWarning() << QString("Unable to insert the genre %1 into the database")
                          .arg(query.boundValue(":name").toString());
            qWarning() << query.lastError();
        }

        linkQuery.bindValue(":trackId", trackBpId);
        linkQuery.bindValue(":genreId", genreBpId);
        if(!linkQuery.exec()){
            qWarning() << QString("Unable to insert the genre <-> track link into the database (%1 for track %2)")
                          .arg(query.boundValue(":name").toString(), linkQuery.boundValue(":trackId").toString());
            qWarning() << linkQuery.lastError();
        }
    }

    query.prepare("INSERT OR IGNORE into BPTracks "
       "(bpid,name,mixName,title,label,key,bpm,releaseDate,publishDate,price,length,release,imageUrl,imagePath) "
       "VALUES (:bpid,:name,:mixName,:title,:label,:key,:bpm,:releaseDate,:publishDate,:price,:length,:release,:imageUrl,:imagePath)");

    query.bindValue(":bpid", trackBpId);
    query.bindValue(":title", trackMap.value("title"));
    query.bindValue(":name", trackMap.value("name"));
    query.bindValue(":mixName", trackMap.value("mixName"));
    query.bindValue(":length", trackMap.value("length"));
    query.bindValue(":bpm", trackMap.value("bpm"));
    query.bindValue(":release", trackMap.value("release").toMap().value("name"));
    query.bindValue(":imageUrl",trackMap.value("images").toMap().value("medium").toMap().value("url"));
    // TODO : download and record image for track
    query.bindValue(":imagePath",QVariant(""));

    QDateTime pubDate(QDate::fromString(trackMap.value("publishDate").toString(), "yyyy-MM-dd"));
    query.bindValue(":publishDate", QVariant(pubDate.toTime_t()));

    QDateTime relDate(QDate::fromString(trackMap.value("releaseDate").toString(), "yyyy-MM-dd"));
    query.bindValue(":releaseDate", QVariant(relDate.toTime_t()));

    QString price = QString::number(trackMap.value("price").toMap().value("value").toDouble() * 0.01, 'g');
    price.append(" ");
    price.append(trackMap.value("price").toMap().value("symbol").toString());
    query.bindValue(":price", price);

    QMap<QString, QVariant> stdKey = trackMap.value("key").toMap().value("standard").toMap();
    QString key = stdKey.value("letter").toString();
    if(stdKey.value("sharp").toBool()) {
        key.append("#");
    }
    if(stdKey.value("flat").toBool()) {
        key.append("\u266D");
    }
    key.append(" ");
    key.append(stdKey.value("chord").toString());
    query.bindValue(":key", key);

    QVariant labelId = trackMap.value("label").toMap().value("id");

    QSqlQuery labelQuery(BPDatabase::dbObject());
    labelQuery.prepare("INSERT OR IGNORE INTO BPLabels VALUES (:bpid,:name)");
    labelQuery.bindValue(":bpid", labelId);
    labelQuery.bindValue(":name", trackMap.value("label").toMap().value("name"));
    if( ! labelQuery.exec()){
        qWarning() << QString("Unable to insert the label %1 into database")
                    .arg(labelQuery.boundValue(":name").toString());
        qWarning() << labelQuery.lastError();
        labelId = QVariant("");
    }
    query.bindValue(":label", labelId);

    if( ! query.exec()){
        qWarning() << QString("Unable to insert the track %1 - %2 into database")
                    .arg(artists.join(", "), query.boundValue(":title").toString());
        qWarning() << query.lastError();
    } else {
        qDebug() << QString("Track %1 - %2 has been corectly stored into database").arg(artists.join(", "), query.boundValue(":title").toString());
    }

    return trackBpId;
}

bool BPDatabase::setLibraryTrackReference(int row, QVariant bpid)
{
    QSqlQuery query(BPDatabase::dbObject());
    query.prepare("UPDATE OR FAIL Library SET bpid=:bpid WHERE uid=:uid");
    query.bindValue(":uid", _libraryModel->record(row).value(LibraryIndexes::Uid));
    query.bindValue(":bpid", bpid);
    if( ! query.exec()) {
        qWarning() << QString("Unable to update library row %1 with the bpid %2")
                      .arg(QString::number(row), query.boundValue(":bpid").toString());
        qWarning() << query.lastError();
        return false;
    } else {
        return true;
    }
}

void BPDatabase::storeSearchResults(int row, QVariant result)
{
    QSqlQuery query(BPDatabase::dbObject());
    query.prepare("INSERT OR IGNORE INTO SearchResults VALUES (:libId,:trackId)");

    int libUid = _libraryModel->record(row).value(LibraryIndexes::Uid).toInt();

    // 1 result for each library row
    if(result.type() == QVariant::Map && ! result.toMap().empty()) {
        QVariant bpid = storeTrack(result);

        setLibraryTrackReference(row, bpid);

        query.bindValue(":libId", _libraryModel->record(row).value(LibraryIndexes::Uid));
        query.bindValue(":trackId", bpid);
        if( ! query.exec()) {
            qWarning() << QString("Unable to register search result for track %1").arg(bpid.toString());
            qWarning() << query.lastError();
        }

        updateLibraryStatus(libUid, FileStatus::ResultsAvailable);
        _libraryModel->refreshAndPreserveSelection();

    } else
    // Many results per library row
    if (result.type() == QVariant::List && ! result.toList().empty()) {
        foreach(QVariant track, result.toList()) {
            QVariant bpid = storeTrack(track);

            query.bindValue(":libId", libUid);
            query.bindValue(":trackId", bpid);
            if( ! query.exec()) {
                qWarning() << QString("Unable to register search result for track %1").arg(bpid.toString());
                qWarning() << query.lastError();
            }
        }

        updateLibraryStatus(libUid, FileStatus::ResultsAvailable);
        _libraryModel->refreshAndPreserveSelection();
    }

}


void BPDatabase::updateLibraryStatus(int uid, FileStatus::Status status)
{
    QSqlQuery query(BPDatabase::dbObject());
    query.prepare("UPDATE OR FAIL Library SET status=:status WHERE uid=:uid");
    query.bindValue(":uid", uid);
    query.bindValue(":status", status);
    if( ! query.exec()) {
        qWarning() << QString("Unable to update library elements's %1 status").arg(uid);
        qWarning() << query.lastError();
    }
}

void BPDatabase::importFiles(const QStringList &pathList)
{
    foreach(QString path, pathList){
        importFile(path);
    }
}

void BPDatabase::importFile(QString path)
{
    QSqlQuery query(BPDatabase::dbObject());
    query.prepare("INSERT INTO Library (filePath, status) VALUES (:path, :status);");
    query.bindValue(":path", path);
    query.bindValue(":status", FileStatus::New);

    if( ! query.exec()){
        qWarning() << tr("Unable to import file %1 : %2").arg(path).arg(_libraryModel->lastError().text());
    } else {
        _libraryModel->refreshAndPreserveSelection();
    }
}

