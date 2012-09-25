#include "patterntool.h"

PatternTool::PatternTool(QString pattern, QObject *parent) :
    QObject(parent),
    pattern(pattern)
{
    QStringList list = pattern.split('%', QString::SkipEmptyParts);
    QStringList regExpList;

    for(int i = 0 ; i < list.count() ; ++i) {

        QString elt = list.at(i);

        if(elt == "ID" || elt == "id") {
            regExpList << "([0-9]+)";
        } else if(elt == "ARTISTS" || elt == "artists") {
            regExpList << "([a-zA-Z0-9_ ',]+)";
        } else if(elt == "TITLE" || elt == "title") {
            regExpList << "([a-zA-Z0-9_ ',]+)";
        } else if(elt == "EXT" || elt == "ext") {
            regExpList << "([a-zA-Z]{2,4})";
        } else if(elt == "OTHER" || elt == "other") {
            regExpList << "(.+)";
        } else {
            regExpList << QString("(?%1)").arg(elt.replace(".", "\\."));
        }
    }
    regExpList.prepend("^");
    regExpList.append("$");

    rexp = QRegExp(regExpList.join(""), Qt::CaseInsensitive, QRegExp::RegExp2);
}

QMap<QString, QString> PatternTool::parseValues(QString &source) const
{
    QMap<QString, QString> result;

    return result;
}
