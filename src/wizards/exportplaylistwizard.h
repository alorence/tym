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

#ifndef EXPORTPLAYLISTWIZARD_H
#define EXPORTPLAYLISTWIZARD_H

#include <QWizard>
#include <QtSql>

namespace Ui {
class ExportPlaylistWizard;
}

class WidgetAppender;

class ExportPlaylistWizard : public QWizard
{
    Q_OBJECT
    
public:
    /*!
     * \brief ExportPlaylistWizard
     * \param selected
     * \param parent
     */
    explicit ExportPlaylistWizard(const QList<QSqlRecord> &selected, QWidget *parent = 0);
    ~ExportPlaylistWizard();

    void showEvent(QShowEvent *) override;
    void initializePage(int id) override;

    enum MainOption {
        UniquePlaylist,
        MultiPlaylists
    };

private slots:
    /*!
     * \brief Change some gui otpions according to user selection
     */
    void updateMainOptionElements();

    /*!
     * \brief Save options selected by user to restore it next time the wizard
     * will be launched
     */
    void saveSelectedOptions();

    /*!
     * \brief Open a dialog to let user select path and name for his unique playlist
     */
    void on_actionSelect_playlist_name_triggered();

    /*!
     * \brief Print the last line in the console
     */
    void printEndText();

private:
    Ui::ExportPlaylistWizard *ui;
    QSettings _settings;
    QList<QSqlRecord> _selectedRecords;

    enum WizardPages {
        OptionPage,
        ResultPage
    };
    WidgetAppender* _widgetAppender;
};

#endif // EXPORTPLAYLISTWIZARD_H
