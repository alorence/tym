#ifndef SEARCHWIZARD_H
#define SEARCHWIZARD_H

#include <QWizard>
#include <QtCore>

namespace Ui {
class SearchWizard;
}

class SearchWizard : public QWizard
{
    Q_OBJECT
    
public:
    enum SearchType {
        FromId,
        FromArtistTitle,
        Custom
    };

    explicit SearchWizard(QWidget *parent = 0);
    ~SearchWizard();
    QString pattern() const;
    SearchType searchType() const;

public slots:
    void setPattern(QString value);

private slots:
    void idSearchSelected(bool);
    void titleArtistSearchSelected(bool);
    void customSearchSelected(bool);

private:
    Ui::SearchWizard *ui;
    SearchType type;
};

#endif // SEARCHWIZARD_H
