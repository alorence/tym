/*****************************************************************************
Copyright 2012-2013 Antoine Lorence. All right reserved.

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

#include "exporttask.h"

#include "Logger.h"
#include "commons.h"
#include "dbaccess/bpdatabase.h"
#include "tools/utils.h"

ExportTask::ExportTask(const QList<QSqlRecord> &selectedRecords,
                                       const QString& filePath, bool exportPlaylist,
                                       QObject *parent) :
    Task(parent),
    _outputFile(filePath),
    _records(selectedRecords),
    _dbHelper(new BPDatabase("exportPlaylistTask", this)),
    _exportPlaylist(exportPlaylist)
{
    _hasMultiResults = false;
    QFileInfo fileInfo(_outputFile);
    if(fileInfo.isDir()) {
        LOG_WARNING(tr("ExportPlaylistTask must be configured with a file path instead of a dir path"));
    }
}

void ExportTask::run()
{
    if(_outputFile.exists()) {
        // TODO: Inform user that his file will be overwritten
        LOG_WARNING(tr("File %1 exists, it will be overwritten").arg(_outputFile.fileName()));
    }

    if( ! _outputFile.open(QIODevice::WriteOnly)) {
        LOG_WARNING(tr("Unable to open %1 in write mode").arg(_outputFile.fileName()));
    }

    QXmlStreamWriter xmlDoc(&_outputFile);
    xmlDoc.setAutoFormatting(true);
    xmlDoc.writeStartDocument("1.0", false);

    xmlDoc.writeStartElement("NML");
    // TODO: Configure the correct version number
    xmlDoc.writeAttribute("VERSION", "15");

    xmlDoc.writeEmptyElement("HEAD");
    xmlDoc.writeAttribute("COMPANY", "www.native-instruments.com");
    xmlDoc.writeAttribute("PROGRAM", "Traktor");

    xmlDoc.writeEmptyElement("MUSICFOLDERS");

    xmlDoc.writeStartElement("COLLECTION");
    xmlDoc.writeAttribute("ENTRIES", QString::number(_records.count()));
    for(QSqlRecord record : _records) {
        writeCollectionEntry(xmlDoc, record);
    }
    xmlDoc.writeEndElement(); // COLLECTION

    xmlDoc.writeEmptyElement("SETS");
    xmlDoc.writeAttribute("ENTRIES", "0");

    xmlDoc.writeStartElement("PLAYLISTS");
    xmlDoc.writeStartElement("NODE");
    xmlDoc.writeAttribute("NAME", "$ROOT");
    xmlDoc.writeAttribute("TYPE", "FOLDER");
    xmlDoc.writeStartElement("SUBNODES");
    xmlDoc.writeAttribute("COUNT", "1");
    xmlDoc.writeStartElement("NODE");
    xmlDoc.writeAttribute("NAME", QFileInfo(_outputFile).completeBaseName());
    xmlDoc.writeAttribute("TYPE", "PLAYLIST");
    xmlDoc.writeStartElement("PLAYLIST");
    if(_exportPlaylist) {
        xmlDoc.writeAttribute("ENTRIES", QString::number(_records.count()));
        xmlDoc.writeAttribute("TYPE", "LIST");

        for(QSqlRecord record : _records) {
            writePlaylistEntry(xmlDoc, record);
        }
    } else {
        xmlDoc.writeAttribute("ENTRIES", "0");
        xmlDoc.writeAttribute("TYPE", "LIST");
    }
    xmlDoc.writeEndElement(); // PLAYLIST
    xmlDoc.writeEndElement(); // NODE
    xmlDoc.writeEndElement(); // SUBNODES
    xmlDoc.writeEndElement(); // NODE
    xmlDoc.writeEndElement(); // PLAYLISTS

    xmlDoc.writeEndElement(); // NML
    xmlDoc.writeEndDocument();

    _outputFile.close();

    LOG_INFO(tr("File %1 has been correctly written").arg(_outputFile.fileName()));

    emit finished();
}

void ExportTask::writeCollectionEntry(QXmlStreamWriter &xmlDoc, const QSqlRecord &record)
{
    QFileInfo path(record.value(Library::FilePath).toString());
    QString bpid = record.value(Library::Bpid).toString();

    QSqlRecord trackInfos = _dbHelper->trackInformations(bpid);

    QString basePath = path.canonicalPath();
    int firstSlashIndex = basePath.indexOf('/');
    QString volume = basePath.mid(0, firstSlashIndex);
    QString tractorFormattedPath = basePath.append('/').mid(firstSlashIndex).replace('/', "/:");

    xmlDoc.writeStartElement("ENTRY");
    xmlDoc.writeAttribute("ARTIST", trackInfos.value(TrackFullInfos::Artists).toString());
    xmlDoc.writeAttribute("TITLE", trackInfos.value(TrackFullInfos::Title).toString());

    xmlDoc.writeEmptyElement("LOCATION");
    xmlDoc.writeAttribute("DIR", tractorFormattedPath);
    xmlDoc.writeAttribute("FILE", path.fileName());
    // FIXME #3: Mac OS Volume is not correctly written
    xmlDoc.writeAttribute("VOLUME", volume);

    xmlDoc.writeEmptyElement("ALBUM");
    xmlDoc.writeAttribute("TITLE", trackInfos.value(TrackFullInfos::Release).toString());

    xmlDoc.writeEmptyElement("INFO");
    xmlDoc.writeAttribute("CATALOG_NO", trackInfos.value(TrackFullInfos::Bpid).toString());
    xmlDoc.writeAttribute("GENRE", trackInfos.value(TrackFullInfos::Genres).toString());

    xmlDoc.writeAttribute("KEY", Utils::instance()->formatKey(trackInfos.value(TrackFullInfos::Key).toString()));

    xmlDoc.writeAttribute("LABEL", trackInfos.value(TrackFullInfos::LabelName).toString());
    xmlDoc.writeAttribute("MIX", trackInfos.value(TrackFullInfos::MixName).toString());
    QDateTime d = QDateTime::fromTime_t(trackInfos.value(TrackFullInfos::ReleaseDate).toInt());
    xmlDoc.writeAttribute("RELEASE_DATE", d.toString("yyyy/M/d"));
    xmlDoc.writeAttribute("REMIXER", trackInfos.value(TrackFullInfos::Remixers).toString());

    xmlDoc.writeEmptyElement("TEMPO");
    xmlDoc.writeAttribute("BPM", trackInfos.value(TrackFullInfos::Bpm).toString());

    xmlDoc.writeEndElement(); // ENTRY

}

void ExportTask::writePlaylistEntry(QXmlStreamWriter &xmlDoc, const QSqlRecord &record)
{
    QString traktorFormattedPath = record.value(Library::FilePath).toString().replace('/', "/:");

    xmlDoc.writeStartElement("ENTRY");

    xmlDoc.writeEmptyElement("PRIMARYKEY");
    xmlDoc.writeAttribute("KEY", traktorFormattedPath);
    xmlDoc.writeAttribute("TYPE", "TRACK");

    xmlDoc.writeEndElement(); // ENTRY
}
