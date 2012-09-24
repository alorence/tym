#ifndef SELECTSEARCHSOURCE_H
#define SELECTSEARCHSOURCE_H

#include <QDialog>

namespace Ui {
class SelectSearchSource;
}

class SelectSearchSource : public QDialog
{
    Q_OBJECT
    
public:
    explicit SelectSearchSource(QWidget *parent = 0);
    ~SelectSearchSource();
    
private:
    Ui::SelectSearchSource *ui;
};

#endif // SELECTSEARCHSOURCE_H
