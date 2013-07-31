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
    //: Fill this with your language (ex: English, Deutch, Fran�ais, etc)
    tr("__LANG__");
    //: Fill this with the country your language is for (United-States, France, etc.)
    tr("__COUNTRY__");

    QStringList searchDirs;
#ifdef Q_OS_MAC
    searchDirs << qApp->applicationDirPath() + "../Resources/lang"
#else
    searchDirs << qApp->applicationDirPath() + "/lang"
#endif
               << qApp->applicationDirPath();

    QDir searchDir;
    searchDir.setFilter(QDir::Files);
    searchDir.setNameFilters(QStringList() << "*.qm");

    QFileInfoList files;
    for(QString dir : searchDirs) {
        searchDir.setCurrent(dir);
        files = searchDir.entryInfoList();
        if(files.count()) {
            LOG_INFO(QString("%1 translations found in the directory %2")
                     .arg(files.count())
                     .arg(searchDir.absolutePath()));
            break;
        }
    }

    for(QFileInfo f : files) {
        QString key = f.baseName();

        QTranslator* transl = new QTranslator();
        transl->load(f.fileName(), f.absolutePath());
        _translatorMap.insert(key, transl);

        QString langName = transl->translate("LangManager", "__LANG__")
                + " (" + transl->translate("LangManager", "__COUNTRY__") + ")";
        _langMap.insert(key, langName);
    }
}

LangManager::~LangManager()
{
    qDeleteAll(_translatorMap);
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
    return _translatorMap;
}

QMap<QString, QString> LangManager::translationsAvailable() const
{
    return _langMap;
}

bool LangManager::updateTranslation(const QString &lang)
{
    if(!_currentTranslation.isEmpty()) {
        qApp->removeTranslator(_translatorMap[_currentTranslation]);
    }

    if(_translatorMap.contains(lang)) {
        qApp->installTranslator(_translatorMap[lang]);
        _currentTranslation = lang;
        return true;
    } else {
        LOG_WARNING(QString("Trying to set translation \"%1\" which is unknown by the manager")
                    .arg(lang));
    }

    return false;
}

void LangManager::updateTranslationsFromSettings()
{
    QSettings settings;
    QString langBaseName = settings.value(TYM_PATH_LANGUAGE, TYM_DEFAULT_LANGUAGE).toString();


    if(!(langBaseName.isEmpty() || TYM_DEFAULT_LANGUAGE == langBaseName)
            && _langMap.contains(langBaseName)) {
        LOG_DEBUG(QString("%1 found as preferred language in the settings").arg(langBaseName));
        updateTranslation(langBaseName);
    }
}