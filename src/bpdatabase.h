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

    static QSqlDatabase dbObject();

    LibraryModel * libraryModel() const;
//    QSqlRelationalTableModel * tracksModel() const;
//    QSqlRelationalTableModel * searchResultsModel() const;

public slots:
    void storeSearchResults(const QMap<int,QVariant>);
    QVariant storeTrack(const QVariant track);
    void importFile(QString filePath);
    void importFiles(const QStringList &);
    bool setLibraryTrackReference(int row, QVariant bpid);

private :
    LibraryModel *_libraryModel;
    void initTables();

    QSqlRecord basicLibraryRecord;
};

#endif // BPDATABASE_H
