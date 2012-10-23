#ifndef COMMONS_H
#define COMMONS_H

namespace LibraryIndexes {
    enum Index {
        Uid = 0,
        FilePath,
        Bpid,
        Status,
        Note
    };
}
namespace BPTracksIndexes {
    enum Index {
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
    enum Index {
        Bpid = 0,
        Track
    };
}
namespace FileStatus {
    enum Status {
        New = 0,
        ResultsAvailable = 1,
        FileNotFound = 16
    };
}

#endif // COMMONS_H
