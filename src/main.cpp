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

/*! \file
 *
 * Define the software entry point.
 * \sa main()
 */

/*! \mainpage
 *
 * Tag Your Music is a software which can search for audio files meta-tags from Internet,
 * store it and use it to operate some tasks :
 *  - Rename tracks on the disk following a common pattern
 *  - Output playlists files (nml format) to write all informations
 *
 * \sa SearchTask, RenameTask,
 *
 */
#include <QtWidgets/QApplication>
#include <QTextEdit>

#include <Logger.h>
#include <ConsoleAppender.h>

#include "gui/mainwindow.h"
#include "tools/langmanager.h"
#include "tools/utils.h"
#include "commons.h"
#include "config.h"

/*!
 * \brief Software entry point
 *
 * Configure logging tools, set some software properties, initialize mandatory
 * components, build the MainWindow and show it. This method uses the QApplication::exec()
 * method to launch the default Qt event loop
 * \param argc
 * \param argv
 * \return 0 if program exited normally
 * \sa MainWindow
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ConsoleAppender* consoleAppender = new ConsoleAppender();
#ifdef QT_DEBUG
    consoleAppender->setDetailsLevel(Logger::Trace); // Trace==0, default is Debug(1)
#else
    consoleAppender->setDetailsLevel(Logger::Info); // Info==2
#endif
    logger->registerAppender(consoleAppender);

    a.setApplicationName("tagyourmusic");
    a.setApplicationDisplayName("Tag Your Music");
    a.setOrganizationDomain("tagyourmusic.org");
    a.setApplicationVersion(TYM_VERSION);

    // Need to configure lang after setting application name (for QSettings)
    // and Logger (because LangManager display log message) has been initialized.
    LangManager::instance()->updateTranslationsFromSettings();

    // Clean user persistent storage from useless data
    Utils::instance()->cleanSomeOldStuff();

    LOG_INFO(QString("%1 is starting, version %2").arg(a.applicationDisplayName())
             .arg(a.applicationVersion()));
    LOG_DEBUG(QString("Compiled with Qt %1, run with Qt library version %2")
              .arg(QT_VERSION_STR).arg(qVersion()));

    {
        // Initialize some mandatory software items
        if( ! QDir(TYM_DATA_LOCATION).exists()) {
            QDir().mkpath(TYM_DATA_LOCATION);
        }
        if( ! QDir(TYM_ALBUMARTS_LOCATION).exists()) {
            QDir().mkpath(TYM_ALBUMARTS_LOCATION);
        }
        LOG_DEBUG(QString("Database storage: %1").arg(TYM_DATA_LOCATION));
        LOG_DEBUG(QString("Albums pictures storage: %2").arg(TYM_ALBUMARTS_LOCATION));
    }

    // Allow QItemSelection to be stored in a QVariant
    qRegisterMetaType<QItemSelection>("QItemSelection");
    qRegisterMetaType<Utils::StatusType>("Utils::StatusType");

    MainWindow w;
    w.show();
    int returnCode = a.exec();

    LOG_DEBUG(QString("Application ends with return code %1").arg(returnCode));

    // Delete all singletons used in the program
    LangManager::deleteInstance();
    Utils::deleteInstance();
    O1Beatport::deleteInstance();

    return returnCode;
}
