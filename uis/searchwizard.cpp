#include "searchwizard.h"
#include "ui_searchwizard.h"

SearchWizard::SearchWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::SearchWizard)
{
    ui->setupUi(this);

    connect(ui->searchFromId, SIGNAL(toggled(bool)), this, SLOT(idSearchSelected(bool)));
    connect(ui->searchFromArtistTitle, SIGNAL(toggled(bool)), this, SLOT(titleArtistSearchSelected(bool)));
    connect(ui->customSearch, SIGNAL(toggled(bool)), this, SLOT(customSearchSelected(bool)));

    ui->searchFromId->setChecked(true);
}

SearchWizard::~SearchWizard()
{
    delete ui;
}

QString SearchWizard::pattern() const
{
    return ui->pattern->text();
}

SearchWizard::SearchType SearchWizard::searchType() const
{
    return type;
}

void SearchWizard::setPattern(QString value)
{
    ui->pattern->setText(value);
}

void SearchWizard::idSearchSelected(bool checked)
{
    if(checked ){
        setPattern("%ID%_%OTHER%.%EXT%");
        type = FromId;
    }
}

void SearchWizard::titleArtistSearchSelected(bool checked)
{
    if(checked) {
        setPattern("%ARTISTS%_%TITLE%.%EXT%");
        type = FromArtistTitle;
    }
}

void SearchWizard::customSearchSelected(bool checked)
{
    if(checked) {
        type = Custom;
    }
}
