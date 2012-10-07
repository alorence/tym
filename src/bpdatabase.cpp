#include "bpdatabase.h"

BPDatabase::BPDatabase(QObject *parent) :
    QObject(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("../bpmanager.bpl");

    if(db.open()) {
        if(version() == "-1") {
            initTables();
            qDebug() << "DB opened and initialized for the first time, version " << version();
        } else {
            qDebug() << "DB opened, version " << version();
        }

        _libraryModel = new LibraryModel(this, db);
        _libraryModel->setTable("Library");
        _libraryModel->setJoinMode(QSqlRelationalTableModel::LeftJoin);
        _libraryModel->setRelation(LibraryIndexes::Bpid, QSqlRelation("BPTracks", "bpid", "title"));
        _libraryModel->select();

        _tracksModel = new QSqlRelationalTableModel(this, db);
        _tracksModel->setTable("BPTracks");
        _tracksModel->setRelation(BPTracksIndexes::Label, QSqlRelation("BPLabels", "bpid", "name"));

        _artistsModel = new QSqlTableModel(this, db);
        _artistsModel->setTable("BPArtists");

        _genresModel = new QSqlTableModel(this, db);
        _genresModel->setTable("BPGenres");

        _labelsModel = new QSqlTableModel(this, db);
        _labelsModel->setTable("BPLabels");

        _searchResultsModel = new QSqlRelationalTableModel(this, db);
        _searchResultsModel->setTable("SearchResults");
        _searchResultsModel->setRelation(0, QSqlRelation("Library", "uid", "filePath"));
        _searchResultsModel->setRelation(1, QSqlRelation("BPTracks", "bpid", "title"));
        _searchResultsModel->select();

        _tracksArtistsLink = new QSqlRelationalTableModel(this, db);
        _tracksArtistsLink->setTable("BPTracksArtistsLink");
        _tracksArtistsLink->setRelation(0, QSqlRelation("BPTracks", "bpid", "name"));
        _tracksArtistsLink->setRelation(1, QSqlRelation("BPArtists", "bpid", "name"));

        _tracksRemixersLink = new QSqlRelationalTableModel(this, db);
        _tracksRemixersLink->setTable("BPTracksRemixersLink");
        _tracksRemixersLink->setRelation(0, QSqlRelation("BPTracks", "bpid", "name"));
        _tracksRemixersLink->setRelation(1, QSqlRelation("BPArtists", "bpid", "name"));

        _tracksGenresLink = new QSqlRelationalTableModel(this, db);
        _tracksGenresLink->setTable("BPTracksGenresLink");
        _tracksGenresLink->setRelation(0, QSqlRelation("BPTracks", "bpid", "name"));
        _tracksGenresLink->setRelation(1, QSqlRelation("BPGenres", "bpid", "name"));

    } else {
        qDebug() << "Error : " << db.lastError().text();
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

QSqlRelationalTableModel *BPDatabase::tracksModel() const
{
    return _tracksModel;
}

QSqlRelationalTableModel *BPDatabase::searchResultsModel() const
{
    return _searchResultsModel;
}

void BPDatabase::storeSearchResults(const QMap<int, QVariant> trackList)
{
    QMapIterator<int, QVariant> e(trackList);
    while(e.hasNext()) {
        e.next();
        int libraryRow = e.key();
        QVariant result = e.value();

        // 1 result for each library row
        if(result.type() == QVariant::Map) {
            QVariant bpid = storeTrack(result);

            QSqlRecord entry = _libraryModel->record(libraryRow);
            entry.setValue(LibraryIndexes::Bpid, bpid);
            _libraryModel->setRecord(libraryRow, entry);

            QSqlRecord searchResultEntry = _searchResultsModel->record();
            searchResultEntry.setValue("libId", entry.value(LibraryIndexes::Uid));
            searchResultEntry.setValue("trackId", bpid);
            if( ! _searchResultsModel->insertRecord(-1, searchResultEntry)) {
                qWarning() << "Unable to register search result :" << _searchResultsModel->lastError().text();
            }

        } else
        // Many results per library row
        if (result.type() == QVariant::List) {

            foreach(QVariant track, result.toList()) {
                QVariant bpid = storeTrack(track);

                QSqlRecord searchResultEntry = _searchResultsModel->record();
                searchResultEntry.setValue("libId", _libraryModel->record(libraryRow).value(LibraryIndexes::Uid));
                searchResultEntry.setValue("trackId", bpid);
                if( ! _searchResultsModel->insertRecord(-1, searchResultEntry)) {
                    qWarning() << "Unable to register search result :" << _searchResultsModel->lastError().text();
                }
            }
        }
    }
}

QVariant BPDatabase::storeTrack(const QVariant track)
{
    QMap<QString, QVariant> trackMap = track.toMap();
    QVariant trackBpId = trackMap.value("id");

    QSqlRecord trackRecord = _tracksModel->record();

    trackRecord.setValue(BPTracksIndexes::Bpid, trackBpId);
    trackRecord.setValue(BPTracksIndexes::Title, trackMap.value("title"));
    trackRecord.setValue(BPTracksIndexes::TrackName, trackMap.value("name"));
    trackRecord.setValue(BPTracksIndexes::MixName, trackMap.value("mixName"));
    trackRecord.setValue(BPTracksIndexes::Length, trackMap.value("length"));
    trackRecord.setValue(BPTracksIndexes::Bpm, trackMap.value("bpm"));
    trackRecord.setValue(BPTracksIndexes::Release, trackMap.value("release").toMap().value("name"));

    trackRecord.setValue(BPTracksIndexes::ImageUrl,
                         trackMap.value("image").toMap().value("medium").toMap().value("url"));
    QDateTime pubDate(QDate::fromString(trackMap.value("publishDate").toString(), "yyyy-MM-dd"));
    trackRecord.setValue(BPTracksIndexes::PublishDate, QVariant(pubDate.toTime_t()));

    QDateTime relDate(QDate::fromString(trackMap.value("releaseDate").toString(), "yyyy-MM-dd"));
    trackRecord.setValue(BPTracksIndexes::ReleaseDate, QVariant(relDate.toTime_t()));

    QString price = QString::number(trackMap.value("price").toMap().value("value").toDouble() * 0.01, 'g');
    price.append(" ");
    price.append(trackMap.value("price").toMap().value("symbol").toString());
    trackRecord.setValue(BPTracksIndexes::Price, price);

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
    trackRecord.setValue(BPTracksIndexes::Key, key);

    QSqlRecord record = _labelsModel->record();
    record.setValue("bpid", trackMap.value("label").toMap().value("id"));
    record.setValue("name", trackMap.value("label").toMap().value("name"));
    _labelsModel->insertRecord(-1, record);
    trackRecord.setValue(BPTracksIndexes::Label, record.value("bpid"));

    _tracksModel->insertRecord(-1, trackRecord);

    foreach (QVariant artist, trackMap.value("artists").toList()) {
        QVariant artistBpId = artist.toMap().value("id");

        QSqlRecord record = _artistsModel->record();
        record.setValue("bpid", artistBpId);
        record.setValue("name", artist.toMap().value("name"));
        _artistsModel->insertRecord(-1, record);

        if ( artist.toMap().value("type").toString() == "remixer") {
            record = _tracksRemixersLink->record();
            record.setValue(0, trackBpId);
            record.setValue(1, artistBpId);
            _tracksRemixersLink->insertRecord(-1, record);
        } else {
            record = _tracksArtistsLink->record();
            record.setValue(0, trackBpId);
            record.setValue(1, artistBpId);
            _tracksArtistsLink->insertRecord(-1, record);
        }
    }

    foreach (QVariant genre, trackMap.value("genres").toList()) {
        QVariant genreBpId = genre.toMap().value("id");

        QSqlRecord record = _genresModel->record();
        record.setValue("bpid", genreBpId);
        record.setValue("name", genre.toMap().value("name"));
        _genresModel->insertRecord(-1, record);

        record = _tracksGenresLink->record();
        record.setValue(0, trackBpId);
        record.setValue(1, genreBpId);
        _tracksGenresLink->insertRecord(-1, record);
    }

    return trackBpId;
}

void BPDatabase::importFiles(const QStringList &pathList)
{
    foreach(QString path, pathList){
        importFile(path);
    }
}

void BPDatabase::importFile(QString path)
{
    QSqlRecord rec = _libraryModel->record();
    rec.setValue("filePath", QVariant(path));

    if( ! _libraryModel->insertRecord(-1, rec)){
        qWarning() << "Unable to import file" << path << ":" << _libraryModel->lastError().text();
    }
}

void BPDatabase::initTables()
{
    QStringList sqlInitCommands = QStringList() <<
        "BEGIN TRANSACTION;" <<
        "PRAGMA foreign_keys = ON;" <<

        "CREATE TABLE Infos (key TEXT UNIQUE, value TEXT);" <<
        "INSERT INTO Infos (key, value) VALUES ('version', '0.1');" <<

        //Main library table
        "CREATE TABLE Library (uid INTEGER PRIMARY KEY, filePath TEXT, bpid INTEGER REFERENCES BPTracks(bpid), status TEXT, note TEXT);" <<

        // Main BeatPort infos tables
        "CREATE TABLE BPTracks  (bpid INTEGER PRIMARY KEY, name TEXT, mixName TEXT, title TEXT, label INTEGER REFERENCES BPLabels(bpid), key TEXT, bpm TEXT, releaseDate INTEGER, publishDate INTEGER, price TEXT, length TEXT, release TEXT, imageUrl TEXT, imagePath TEXT);" <<
        "CREATE TABLE BPArtists (bpid INTEGER PRIMARY KEY, name TEXT);" <<
        "CREATE TABLE BPGenres  (bpid INTEGER PRIMARY KEY, name TEXT);" <<
        "CREATE TABLE BPLabels  (bpid INTEGER PRIMARY KEY, name TEXT);" <<

        "CREATE TABLE BPTracksArtistsLink (trackId INTEGER REFERENCES BPTracks(bpid), artistId INTEGER REFERENCES BPArtists(bpid), PRIMARY KEY(trackId, artistId));" <<
        "CREATE TABLE BPTracksRemixersLink (trackId INTEGER REFERENCES BPTracks(bpid), artistId INTEGER REFERENCES BPArtists(bpid), PRIMARY KEY(trackId, artistId));" <<
        "CREATE TABLE BPTracksGenresLink (trackId INTEGER REFERENCES BPTracks(bpid), genreId INTEGER REFERENCES BPGenres(bpid), PRIMARY KEY(trackId, genreId));" <<
        "CREATE TABLE SearchResults (libId INTEGER REFERENCES Library(uid), trackId INTEGER REFERENCES BPTracks(bpid), PRIMARY KEY(libId, trackId));" <<

        "CREATE TABLE Infos (key TEXT PRIMARY KEY, value TEXT);" <<
        "INSERT INTO Infos VALUES('version',0.1);" <<

        "COMMIT;";

    QSqlDatabase db = dbObject();
    foreach(QString line, sqlInitCommands) {
        db.exec(line);
    }
}

QString BPDatabase::version()
{
    QSqlDatabase db = dbObject();
    QSqlQuery query("SELECT value FROM Infos WHERE key='version'", db);
    if( ! query.exec()) {
        return "-1";
    } else {
        query.next();
        return query.value(0).toString();
    }
}
