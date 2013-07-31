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

#ifndef LANGMANAGER_H
#define LANGMANAGER_H

#include <QtCore>

/*!
 * \brief Singleton class used to manage translations files supported by the application.
 * This class build a list of translations files (*.qm) by searching in the same directory than
 * the executable itself (when program is launched from an IDE) and in a "lang" subfolder (Win)
 * or the <Bundle>/Content/Resources/lang folder (MacOS). Once this list is built, this class
 * provides methods to access this list and to dynamically update current localization for
 * the program.
 */
class LangManager : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Get the unique instance of this class (Singleton design pattern)
     * \return The instance of LangManager
     */
    static LangManager *instance();
    /*!
     * \brief Destroy the current instance and set the internal pointer to "nullptr"
     */
    static void destroy();

    /*!
     * \brief Provides a QMap of the translator objects managed by this class.
     * Keys are strings corresponding to the language. Values are pointers to QTranslator instances.
     * \return A QMap<QString, QTranslator*>
     */
    QMap<QString, QTranslator*> translationsFiles() const;
    /*!
     * \brief Returns all translations available as a QList of QString.
     * \return
     */
    QMap<QString, QString> translationsAvailable() const;
    /*!
     * \brief Update current QApplication with the QTranslator corresponding to the given string.
     * \param lang
     * \return true if everything worked as expected
     */
    bool updateTranslation(const QString &lang);

public slots:
    /*!
     * \brief Check in current application settings to update translation according to user choice.
     */
    void updateTranslationsFromSettings();

private:
    /*!
     * \brief Initialize the manager. This constructor is private.
     * Search for available translations (*.qm files) in specific directories and build maps
     * used to store translations names and Qtranslator instances.
     * \param parent
     */
    explicit LangManager(QObject *parent = 0);
    ~LangManager();
    QMap<QString, QTranslator*> _translatorMap;
    QMap<QString, QString> _langMap;
    QString _currentTranslation;

    static LangManager *_instance;
    static QMutex _mutex;
};

#endif // LANGMANAGER_H
