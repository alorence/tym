#ifndef BPDATABASE_H
#define BPDATABASE_H

#include <QtSql>
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
        TrackName = 2,
        MixName = 3,
        Title = 4, // Full title, formatted as '%TrackName (%MixName)'
        Label = 13,
        Key = 15,
        Bpm = 16,
        ReleaseDate = 5,
        PublishDate = 6,
        Price = 7,
        Length = 8,
        Release = 12,
        ImageUrl = 14,
        ImagePath = 14
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

private :
    void initTables();
};

#endif // BPDATABASE_H
