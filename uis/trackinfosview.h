#ifndef TRACKINFOSVIEW_H
#define TRACKINFOSVIEW_H

#include <QtGui>
#include <QtCore>

#include "src/librarymodel.h"
#include "src/bpdatabase.h"

namespace Ui {
class TrackInfosView;
}

class TrackInfosView : public QWidget
{
    Q_OBJECT
    
public:
    explicit TrackInfosView(QWidget *parent = 0);
    ~TrackInfosView();

    void setMapping(LibraryModel * model);

public slots:
    void setValuesForRow(QList<int> rows);
    void clearData(QList<int> indexes = QList<int>());
    void setWidgetsEnabled(bool, QList<int> indexes = QList<int>());

private slots:
    void updateValuesForRow(int row);

private:
    Ui::TrackInfosView *ui;
    QDataWidgetMapper * mapper;

    QMap<int, QLineEdit*> widgets;

};

#endif // TRACKINFOSVIEW_H
