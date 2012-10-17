#ifndef BPDATABASE_H
#define BPDATABASE_H

#include <QtCore>
#include <QtSql>

#include "src/librarymodel.h"

namespace LibraryIndexes {
    enum {
        Uid = 0,
        FilePath,
        Bpid,
        Status,
        Note
    };
}
namespace BPTracksIndexes {
    enum {
        Bpid = 0,
        TrackName,
        MixName,
        Title, // Full title, formatted as '%TrackName (%MixName)'
        Label,
        Key,
        Bpm,
        ReleaseDate,
        PublishDate,
        Price,
        Length,
        Release,
        ImageUrl,
        ImagePath
    };
}

class BPDatabase : public QObject
{
    Q_OBJECT

public:
    explicit BPDatabase(QObject *parent = 0);

    bool databaseConnect();
    QString version();

    static const QSqlDatabase dbObject();

    LibraryModel * libraryModel() const;
    QSqlQueryModel * searchModel() const;

public slots:
    void storeSearchResults(const QMap<int,QVariant>);
    QVariant storeTrack(const QVariant track);
    void importFile(QString filePath);
    void importFiles(const QStringList &);
    bool setLibraryTrackReference(int row, QVariant bpid);
    void updateSearchResults(const QModelIndex&,const QModelIndex&);

private :
    LibraryModel *_libraryModel;
    void initTables();

    QSqlQueryModel * _searchModel;
    QSqlQuery _searchQuery;

    QSqlRecord basicLibraryRecord;
};

#endif // BPDATABASE_H
