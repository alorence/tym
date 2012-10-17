#ifndef TRACKINFOSVIEW_H
#define TRACKINFOSVIEW_H

#include <QtGui>
#include <QtCore>
#include <QtSql>

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

public slots:
    void clearData();
    void updateInfos(QVariant &bpid);

private:
    Ui::TrackInfosView *ui;
};

#endif // TRACKINFOSVIEW_H
