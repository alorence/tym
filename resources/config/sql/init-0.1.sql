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
CREATE TABLE BPTracksArtistsLink (trackId INTEGER REFERENCES BPTracks(bpid), artistId INTEGER REFERENCES BPArtists(bpid),
	PRIMARY KEY(trackId, artistId));
CREATE TABLE BPTracksRemixersLink (trackId INTEGER REFERENCES BPTracks(bpid), artistId INTEGER REFERENCES BPArtists(bpid),
	PRIMARY KEY(trackId, artistId));
CREATE TABLE BPTracksGenresLink (trackId INTEGER REFERENCES BPTracks(bpid), genreId INTEGER REFERENCES BPGenres(bpid),
	PRIMARY KEY(trackId, genreId));
CREATE TABLE SearchResults (libId INTEGER REFERENCES Library(uid), trackId INTEGER REFERENCES BPTracks(bpid),
	PRIMARY KEY(libId, trackId));

# Simplify library infos displaying
CREATE VIEW LibraryHelper AS SELECT l.*, count(sr.trackId) as results
FROM Library as l LEFT JOIN SearchResults as sr ON sr.libId = l.uid
GROUP BY l.uid;

# Simplify tracks infos reading
CREATE VIEW TrackFullInfos as SELECT 
	(SELECT DISTINCT group_concat( art.name, ', ') FROM BPArtists as art
		JOIN BPTracksArtistsLink as artl ON artl.artistId=art.bpid WHERE artl.trackId=tr.bpid) as artists,
	(SELECT DISTINCT group_concat( rmx.name, ', ') FROM BPArtists as rmx
		JOIN BPTracksRemixersLink as rmxl ON rmxl.artistId=rmx.bpid WHERE rmxl.trackId=tr.bpid) as remixers,
	(SELECT DISTINCT group_concat( genre.name, ', ') FROM BPGenres as genre
		JOIN BPTracksGenresLink as genrel ON genrel.genreId=genre.bpid WHERE genrel.trackId=tr.bpid) as genres,
	l.name as labelName, tr.* FROM BPTracks as tr 
		JOIN BPLabels as l ON l.bpid = tr.label;

# Simplify search results displaying and library element <-> track join
CREATE VIEW SearchResultsHelper as SELECT
		sr.libId as libId, tr.bpid as trId,
		(group_concat(a.name, ', ') || ' - ' || tr.title) as Track,
		lib.uid as defaultFor
	FROM
		BPTracks as tr
		JOIN SearchResults as sr ON tr.bpid = sr.trackId
		JOIN BPTracksArtistsLink as talink ON talink.trackId = sr.trackId
		JOIN BPArtists as a ON a.bpid = talink.artistId
		LEFT JOIN Library as lib ON lib.bpid = tr.bpid
	GROUP BY tr.bpid;

COMMIT;
