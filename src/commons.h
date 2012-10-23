#ifndef COMMONS_H
#define COMMONS_H

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
namespace SearchResultsIndexes {
    enum {
        Bpid = 0,
        Track
    };
}
namespace FileStatus {
    enum {
        New = 0,
        ResultsAvailable = 1,
        FileNotFound = 16
    };
}

#endif // COMMONS_H
