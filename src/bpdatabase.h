#ifndef BPDATABASE_H
#define BPDATABASE_H

#include <QtCore>
#include <QtSql>

#include "src/librarymodel.h"

namespace LibraryIndexes {
    enum {
        Uid = 0,
        FilePath = 1,
        Bpid = 2,
        Status = 3,
        Note = 4
    };
}
namespace BPTracksIndexes {
    enum {
        Bpid = 0,
        TrackName = 1,
        MixName = 2,
        Title = 3, // Full title, formatted as '%TrackName (%MixName)'
        Label = 4,
        Key = 5,
        Bpm = 6,
        ReleaseDate = 7,
        PublishDate = 8,
        Price = 9,
        Length = 10,
        Release = 11,
        ImageUrl = 12,
        ImagePath = 13
    };
}

class BPDatabase : public QObject
{
    Q_OBJECT

public:
    explicit BPDatabase(QObject *parent = 0);
    
    bool databaseConnect();
    QString version();

    static QSqlDatabase dbObject();


    LibraryModel * libraryModel() const;
    QSqlRelationalTableModel * tracksModel() const;
    QSqlRelationalTableModel * searchResultsModel() const;

public slots:
    void storeSearchResults(const QMap<int,QVariant>);
    QVariant storeTrack(const QVariant track);
    void importFile(QString filePath);
    void importFiles(const QStringList &);

private :
    LibraryModel *_libraryModel;
    QSqlRelationalTableModel *_tracksModel;
    QSqlTableModel *_artistsModel;
    QSqlTableModel *_genresModel;
    QSqlTableModel *_labelsModel;
    QSqlRelationalTableModel *_tracksArtistsLink;
    QSqlRelationalTableModel *_tracksGenresLink;
    QSqlRelationalTableModel *_tracksRemixersLink;
    QSqlRelationalTableModel *_searchResultsModel;
    void initTables();

};

#endif // BPDATABASE_H
