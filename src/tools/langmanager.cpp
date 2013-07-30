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

#include "Logger.h"
#include "langmanager.h"
#include "commons.h"

LangManager * LangManager::_instance = nullptr;
QMutex LangManager::_mutex;

LangManager::LangManager(QObject *parent) :
    QObject(parent)
{
    QStringList searchDirs;
    searchDirs << qApp->applicationDirPath()
#ifdef Q_OS_MAC
               << qApp->applicationDirPath() + "../Resources/lang";
#else
               << qApp->applicationDirPath() + QDir::separator() + "lang";
#endif

    QDir searchDir;
    searchDir.setFilter(QDir::Files);
    searchDir.setNameFilters(QStringList() << "*.qm");

    QFileInfoList files;
    for(QString dir : searchDirs) {
        searchDir.setCurrent(dir);
        files = searchDir.entryInfoList();
        if(files.count()) {
            break;
        }
    }

    for(QFileInfo f : files) {

        QTranslator* transl = new QTranslator();
        transl->load(f.fileName(), searchDir.canonicalPath());
        _qmFiles.insert(f.baseName(), transl);
    }
}

LangManager::~LangManager()
{
    qDeleteAll(_qmFiles);
}

LangManager *LangManager::instance()
{
    _mutex.lock();
    if(_instance == nullptr) {
        _instance = new LangManager(qApp);
    }
    _mutex.unlock();
    return _instance;
}

void LangManager::destroy()
{
    _mutex.lock();
    delete _instance;
    _instance = nullptr;
    _mutex.unlock();
}

QMap<QString, QTranslator *> LangManager::translationsFiles() const
{
    return _qmFiles;
}

QList<QString> LangManager::translationsAvailable() const
{
    return _qmFiles.keys();
}

bool LangManager::updateTranslation(const QString &lang) const
{
    qApp->removeTranslator(_qmFiles[lang]);

    if(_qmFiles.contains(lang)) {
        qApp->installTranslator(_qmFiles[lang]);
        return true;
    }

    return false;
}

void LangManager::updateTranslationsFromSettings() const
{
    QSettings settings;
    QString langBaseName = settings.value(TYM_PATH_LANGUAGE, TYM_DEFAULT_LANGUAGE).toString();

    LOG_DEBUG(tr("Update with %1").arg(langBaseName));

    if(!langBaseName.isEmpty() || TYM_DEFAULT_LANGUAGE == langBaseName) {
        updateTranslation(langBaseName);
    }
}
