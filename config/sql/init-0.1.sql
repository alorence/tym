BEGIN TRANSACTION;

PRAGMA foreign_keys = ON;

CREATE TABLE Infos (key TEXT UNIQUE, value TEXT);
INSERT INTO Infos (key, value) VALUES ('version', '0.1');

# Main library table
CREATE TABLE Library (uid INTEGER PRIMARY KEY, filePath TEXT, bpid INTEGER REFERENCES BPTracks(bpid), status TEXT);

# Main BeatPort infos tables
CREATE TABLE BPTracks  (bpid INTEGER PRIMARY KEY, name TEXT, mixName TEXT, title TEXT,
	label INTEGER REFERENCES BPLabels(bpid), key TEXT, bpm TEXT, releaseDate INTEGER,
	publishDate INTEGER, price TEXT, length TEXT, release TEXT, image INTEGER);
CREATE TABLE BPArtists (bpid INTEGER PRIMARY KEY, name TEXT);
CREATE TABLE BPGenres  (bpid INTEGER PRIMARY KEY, name TEXT);
CREATE TABLE BPLabels  (bpid INTEGER PRIMARY KEY, name TEXT);

# N-n join tables
CREATE TABLE BPTracksArtistsLink (trackId INTEGER REFERENCES BPTracks(bpid), artistId INTEGER REFERENCES BPArtists(bpid), PRIMARY KEY(trackId, artistId));
CREATE TABLE BPTracksRemixersLink (trackId INTEGER REFERENCES BPTracks(bpid), artistId INTEGER REFERENCES BPArtists(bpid), PRIMARY KEY(trackId, artistId));
CREATE TABLE BPTracksGenresLink (trackId INTEGER REFERENCES BPTracks(bpid), genreId INTEGER REFERENCES BPGenres(bpid), PRIMARY KEY(trackId, genreId));
CREATE TABLE SearchResults (libId INTEGER REFERENCES Library(uid), trackId INTEGER REFERENCES BPTracks(bpid), PRIMARY KEY(libId, trackId));

COMMIT;