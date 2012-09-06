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
    } else {
        qDebug() << "Error : " << db.lastError().text();
    }
}

QSqlDatabase BPDatabase::dbObject()
{
    return QSqlDatabase::database();
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

       "INSERT INTO Library VALUES(NULL,'E:\\Library\\FLAC\\Deep\\Carl Craig - At Les (Christian Smith Hypnotica Mix).flac', NULL, NULL, NULL);" <<
       "INSERT INTO Library VALUES(NULL,'E:\\Library\\FLAC\\Deep\\Max Cooper - Gravitys Rainbow (Original Mix).flac', NULL, NULL, NULL);" <<
       "INSERT INTO Library VALUES(NULL,'E:\\Library\\FLAC\\Deep\\Pachanga Boys - Time (Original Mix).flac', NULL, NULL, NULL);" <<
       "INSERT INTO Library VALUES(NULL,'E:\\Library\\FLAC\\Electro House\\Glowinthedark - Jump (Original Mix).flac', NULL, NULL, NULL);" <<

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
