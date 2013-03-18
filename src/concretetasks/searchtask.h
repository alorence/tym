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

#ifndef SEARCHTHREAD_H
#define SEARCHTHREAD_H

#include <QSqlRecord>
#include "commons.h"
#include "task.h"
#include "wizards/searchwizard.h"

class Task;
class BPDatabase;
class SearchProvider;

/*!
 * \brief Define a task to operate search on Beatport public API
 */
class SearchTask : public Task
{
    Q_OBJECT

public:
    /*!
     * \brief Construct the task. If searchPattern is empty, a manual search is launched using searchTerms. If searchTerms is empty too,
     * an error is displayed.
     * \param selectedRecords List of QSqlRecord corresponding to Library entries user select before launching the search
     * \param searchPattern Format supposed to define filenames, used to extract informations used to build a search request
     * \param searchPattern Search terms to use in case of manual search
     * \param parent
     */
    explicit SearchTask(const QList<QSqlRecord> &selectedRecords, const QString &searchPattern = QString(), const QStringList &searchTerms = QStringList(), QObject *parent = 0);
    ~SearchTask();

public slots:
    /*!
     * \brief Extract informations from selected records and build 1 or more requests to send to Beatport API.
     *
     * This methodrun the network requests, and connect response to SearchTask::checkCountResults() to know when it
     * have to emit the finished() signal. Network request's results are connected to BPDatabase::storeSearchResults()
     * to store results into SQLite database.
     */
    void run();
    /*!
     * \brief Used to check number of results received, and emit the finished() signal when last result has arrived.
     */
    void checkCountResults();

private:
    /*!
     * \brief Try to find the better result for each selected tracks, and link it to the track.
     */
    void selectBetterResult();

    QString _searchPattern;
    QList<QSqlRecord> _selectedRecords;
    QStringList _searchTerms;
    QMap<QString, QMap<TrackFullInfos::Indexes, QString> > _trackParsedInformation;
    BPDatabase* _dbHelper;
    SearchProvider* _search;

    int _numTracksToUpdate;
};

#endif // SEARCHTHREAD_H
