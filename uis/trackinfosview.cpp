#include "trackinfosview.h"
#include "ui_trackinfosview.h"

TrackInfosView::TrackInfosView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackInfosView)
{
    ui->setupUi(this);
}

TrackInfosView::~TrackInfosView()
{
    delete ui;
}

void TrackInfosView::updateInfos(QVariant & bpid)
{
    QSqlQuery query;
    QString queryString = "SELECT group_concat(a.name, ', ') as artists, "
            "group_concat(r.name, ', ') as remixers, "
            "group_concat(g.name, ', ') as genres, tr.* "
            "FROM BPTracks as tr "
            "JOIN BPTracksArtistsLink as talink ON tr.bpid = talink.trackId "
            "JOIN BPArtists as a ON a.bpid = talink.artistId "
            "LEFT JOIN BPTracksRemixersLink as trlink ON tr.bpid = trlink.trackId "
            "LEFT JOIN BPArtists as r ON r.bpid = trlink.artistId "
            "JOIN BPTracksGenresLink as tglink ON tr.bpid = tglink.trackId "
            "JOIN BPGenres as g ON g.bpid = tglink.genreId "
            "JOIN BPLabels as l ON l.bpid = tr.label "
            "WHERE tr.bpid=:bpid GROUP BY a.bpid AND r.bpid AND g.bpid";

/*
SELECT group_concat( a.name, ', ') as artists,
group_concat(r.name, ', ') as remixers,
group_concat( g.name, ', ') as genres, tr.*
FROM BPTracks as tr
JOIN BPTracksArtistsLink as talink ON tr.bpid = talink.trackId
JOIN BPArtists as a ON a.bpid = talink.artistId
LEFT JOIN BPTracksRemixersLink as trlink ON tr.bpid = trlink.trackId
LEFT JOIN BPArtists as r ON r.bpid = trlink.artistId
JOIN BPTracksGenresLink as tglink ON tr.bpid = tglink.trackId
JOIN BPGenres as g ON g.bpid = tglink.genreId
JOIN BPLabels as l ON l.bpid = tr.label
WHERE tr.bpid=3682187
GROUP BY tr.bpid
*/
    query.prepare(queryString);
    query.bindValue(":bpid", bpid);
    if( ! query.exec() ) {
        qWarning() << "Unable to get track informations :" << query.lastError().text();
    }

    query.next();
    qDebug() << query.record();

}

void TrackInfosView::clearData()
{
    qDebug() << "clear data";
}

