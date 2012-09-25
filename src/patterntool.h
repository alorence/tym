#ifndef PATTERNTOOL_H
#define PATTERNTOOL_H

#include <QtCore>
#include "searchprovider.h"

class PatternTool : public QObject
{
    Q_OBJECT
public:
    explicit PatternTool(QString pattern, QObject *parent = 0);

    QMap<QString, QString> parseValues(QString& source) const;
    
signals:
    
public slots:

private:
    QString pattern;
    QMap<QString, QString> indexesMap;
    QMap<int, QString> replacementMap;
    QRegExp rexp;
    
};

#endif // PATTERNTOOL_H
