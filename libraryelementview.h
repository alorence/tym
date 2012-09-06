#ifndef LIBRARYELEMENTVIEW_H
#define LIBRARYELEMENTVIEW_H

#include <QtGui>
#include <QtCore>

#include "librarymodel.h"
#include "bpdatabase.h"

namespace Ui {
class LibraryElementView;
}

class LibraryElementView : public QWidget
{
    Q_OBJECT
    
public:
    explicit LibraryElementView(QWidget *parent = 0);
    ~LibraryElementView();

    void setMapping(LibraryModel * model);

public slots:
    void setValuesForRow(QList<int> rows);
    void clearData(QList<int> indexes = QList<int>());
    void setWidgetsEnabled(bool, QList<int> indexes = QList<int>());

private slots:
    void updateValuesForRow(int row);

private:
    Ui::LibraryElementView *ui;
    QDataWidgetMapper * mapper;

    QMap<int, QLineEdit*> widgets;

};

#endif // LIBRARYELEMENTVIEW_H
