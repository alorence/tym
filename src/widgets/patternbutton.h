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

#ifndef PATTERNBUTTON_H
#define PATTERNBUTTON_H

#include <QPushButton>
#include <QAction>
#include <QLineEdit>

class PatternTool;

/*!
 * \brief Define a button to help user to build a file pattern
 */
class PatternButton : public QPushButton
{
    Q_OBJECT

public:
    explicit PatternButton(const PatternTool &patterntool, QLineEdit *line, QWidget *parent = 0);
    ~PatternButton();

private slots:
    /*!
     * \brief Write the text corresponding to the given action in the lineEdit.
     * \param action The instance of menu antry action selected by user
     */
    void menuActionTriggered(QAction *action);

private:
    QMap<QAction*, QString> _patternElementMap;
    QLineEdit *_lineEdit;
};

#endif // PATTERNBUTTON_H
