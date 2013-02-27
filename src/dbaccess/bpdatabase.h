/*****************************************************************************
Copyright 2013 Antoine Lorence. All right reserved.

This file is part of TYM (Tag Your Music).

TYM (Tag Your Music) is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TYM (Tag Your Music) is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TYM (Tag Your Music). If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef BPDATABASE_H
#define BPDATABASE_H

#include <QtCore>
#include <QtSql>

#include "commons.h"

class LibraryModel;
class SearchResultsModel;

/*!
 * \brief Encapsulate all database communication.
 */
class BPDatabase : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Create a QSqlDatabase with the name \a connectionName and register it in QSqlDatabse manager.
     * If connectionName is empty, the default connection is instanciated, and the constructor check if the tables
     * have been properly initialized. The databse connection is open in this constructor, and stored as a member of
     * this class.
     * \param connectionName The name of the current connection
     * \param parent
     */
    explicit BPDatabase(QString connectionName = "defaultConnection", QObject *parent = 0);
    /*!
     * \brief Close the databse connection and destroy all members
     */
    ~BPDatabase();

    /*!
     * \brief Control the state of the DB connection.
     * \return True if the DB has been initialized, i.e. if all tables have been created, and all system can work
     */
    const bool initialized() const;
    /*!
     * \brief Return the version on the database.
     * This version can be used to update database, if it is necessary after an update of the software for example.
     * \return The version of the database model, -1 if DB has not been initialized
     */
    const QString version() const;
    /*!
     * \brief Return the current QSqlDatabse connection.
     * \return The QSqlDatabse instance
     */
    QSqlDatabase dbObject() const;

    /*!
     * \brief Get all informations about a track.
     * \param bpid Beatport ID of the track
     * \return A QSqlRecord with all track informations
     */
    const QSqlRecord trackInformations(const QString &bpid) const;
    /*!
     * \brief Get all informations about a list of tracks.
     * If \a bpids list is empty, informations for all stored tracks are returned.
     * \param bpids Beatport IDs list of tracks.
     * \return A QSqlQuery with all tracks informations
     */
    const QSqlQuery tracksInformations(const QStringList &bpids = QStringList()) const;
    /*!
     * \brief Get all informations about a library entry.
     * If \a uids is empty, return informations for all entries.
     * \param uids A list of uids for entries requested
     * \return
     */
    const QSqlQuery libraryInformations(const QStringList &uids = QStringList()) const;
    /*!
     * \brief Delete entry from library.
     * Additionnaly, delete all the search results associated with all uids.
     * \param uids List of library uids to delete
     */
    const QSqlQuery resultsForTrack(const QString &libId) const;
    /*!
     * \brief Delete the search result associated with the library uid and the track bpid.
     * \param libId Library Id
     * \param trackId BPTracks entry's bpid
     */
    void deleteLibraryEntry(QStringList uids) const;
    /*!
     * \brief Get all results for file corresponding to libId passed in parameter
     * \param libId libraryy entry uid
     * \return a set of results
     */
    void deleteSearchResult(const QString &libId, const QString &trackId) const;

    /*!
     * \brief Update Library entry with filePath \a oldFilePath to \a newFilePath
     * \param oldFilePath
     * \param newFilePath
     */
    void renameFile(const QString &oldFilePath, const QString &newFilePath) const;

    friend class LibraryModel;

public slots:
    /*!
     * \brief Store results contained in \a result json value into database.
     * Parse the Json result and register informations in database (Track, Artists, Labels, etc). If \a result
     * contains only 1 track as an object, the link between this track and the library entry is automatically
     * done. If \a result contains a list of tracks (even only 1), the track are only added to the database.
     * The SearchResult table is updated with a link between Library entry \a libId and each result.
     * \param libId Library uid the \a result correspond
     * \param result The Json result, from Beatport public API
     */
    void storeSearchResults(const QString &libId, const QJsonValue &result) const;
    /*!
     * \brief Insert a new entry in Library, with path \a filePath
     * \param filePath Full path of the track on the disk
     */
    void importFile(const QString &filePath) const;
    /*!
     * \brief Import many entries in library
     *\param filePaths List of full paths
     */
    void importFiles(const QStringList &filePaths) const;
    /*!
     * \brief Update Library entry status with \a status
     * \param uid Uid of Library entry
     * \param status The new full status value, as a combination of Statuses flags
     * \sa Library::FileStatus
     */
    void updateLibraryStatus(const QString &uid, const Library::FileStatus &status) const;
    /*!
     * \brief Update Library entry to link it with a tracks from BPTracks table.
     * \param libUid Library entry's uid
     * \param bpid BPTracks entry's bpid
     */
    void setLibraryTrackReference(const QString &libUid, const QString &bpid) const;

signals:
    /*!
     * \brief Emitted when a Library entry has been modified
     * \param uid Library uid of the entry
     */
    void libraryEntryUpdated(const QString &uid = QString()) const;
    /*!
     * \brief Emitted when a Library entry has just been linked to a BPTracks entry.
     * \param uid Library uid of the entry
     */
    void referenceForTrackUpdated(const QString &uid) const;
    /*!
     * \brief Emitted when a search result has been stored.
     * \param uid Library entry's uid which has just been updated
     */
    void searchResultStored(const QString& uid) const;

private :
    /*!
     * \brief Initialize tables in databse
     * \return true if initialization has been correctly performed
     */
    const bool initTables() const;
    /*!
     * \brief Store a trackin databse, with all its informations, from Json document \a track
     * \param track Json content to store
     * \return Bpid of track stored
     */
    const QString storeTrack(const QJsonValue &track) const;

    QSqlDatabase _dbObject;
    bool _dbInitialized;

    QMutex *_dbMutex;
};

#endif // BPDATABASE_H
